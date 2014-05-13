
module Gemgem
  class << self
    attr_accessor :dir, :spec, :spec_create
  end

  module_function
  def gem_tag    ; "#{spec.name}-#{spec.version}"     ; end
  def gem_path   ; "#{pkg_dir}/#{gem_tag}.gem"        ; end
  def spec_path  ; "#{dir}/#{spec.name}.gemspec"      ; end
  def pkg_dir    ; "#{dir}/pkg"                       ; end
  def escaped_dir; @escaped_dir ||= Regexp.escape(dir); end

  def init dir, &block
    self.dir = dir
    $LOAD_PATH.unshift("#{dir}/lib")
    ENV['RUBYLIB'] = "#{dir}/lib:#{ENV['RUBYLIB']}"
    ENV['PATH']    = "#{dir}/bin:#{ENV['PATH']}"
    self.spec_create = block
  end

  def create
    spec = Gem::Specification.new do |s|
      s.date        = Time.now.strftime('%Y-%m-%d')
      s.files       = gem_files
      s.test_files  = test_files
      s.executables = bin_files
    end
    spec_create.call(spec)
    self.spec = spec
  end

  def write
    File.open(spec_path, 'w'){ |f| f << split_lines(spec.to_ruby) }
  end

  def split_lines ruby
    ruby.gsub(/(.+?)\s*=\s*\[(.+?)\]/){ |s|
      if $2.index(',')
        "#{$1} = [\n  #{$2.split(',').map(&:strip).join(",\n  ")}]"
      else
        s
      end
    }
  end

  def strip_path path
    strip_home_path(strip_cwd_path(path))
  end

  def strip_home_path path
    path.sub(ENV['HOME'], '~')
  end

  def strip_cwd_path path
    path.sub(Dir.pwd, '.')
  end

  def git *args
    `git --git-dir=#{dir}/.git #{args.join(' ')}`
  end

  def sh_git *args
    Rake.sh('git', "--git-dir=#{dir}/.git", *args)
  end

  def sh_gem *args
    Rake.sh(Gem.ruby, '-S', 'gem', *args)
  end

  def glob path=dir
    Dir.glob("#{path}/**/*", File::FNM_DOTMATCH)
  end

  def all_files
    @all_files ||= fold_files(glob).sort
  end

  def fold_files files
    files.inject([]){ |r, path|
      if File.file?(path) && path !~ %r{/\.git(/|$)}  &&
         (rpath = path[%r{^#{escaped_dir}/(.*$)}, 1])
        r << rpath
      elsif File.symlink?(path) # walk into symlinks...
        r.concat(fold_files(glob(File.expand_path(path,
                                                  File.readlink(path)))))
      else
        r
      end
    }
  end

  def gem_files
    @gem_files ||= all_files.reject{ |f|
      f =~ ignored_pattern && !git_files.include?(f)
    }
  end

  def test_files
    @test_files ||= gem_files.grep(%r{^test/(.+?/)*test_.+?\.rb$})
  end

  def bin_files
    @bin_files ||= gem_files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  end

  def git_files
    @git_files ||= if File.exist?("#{dir}/.git")
                     git('ls-files').split("\n")
                   else
                     []
                   end
  end

  def ignored_files
    @ignored_files ||= all_files.grep(ignored_pattern)
  end

  def ignored_pattern
    @ignored_pattern ||= if gitignore.empty?
                           /^$/
                         else
                           Regexp.new(expand_patterns(gitignore).join('|'))
                         end
  end

  def expand_patterns pathes
    # http://git-scm.com/docs/gitignore
    pathes.flat_map{ |path|
      # we didn't implement negative pattern for now
      Regexp.escape(path).sub(%r{^/}, '^').gsub(/\\\*/, '[^/]*')
    }
  end

  def gitignore
    @gitignore ||= if File.exist?(path = "#{dir}/.gitignore")
                     File.read(path).lines.
                       reject{ |l| l == /^\s*(#|\s+$)/ }.map(&:strip)
                   else
                     []
                   end
  end
end

namespace :gem do

desc 'Install gem'
task :install => [:build] do
  Gemgem.sh_gem('install', Gemgem.gem_path)
end

desc 'Build gem'
task :build => [:spec] do
  require 'fileutils'
  require 'rubygems/package'
  gem = nil
  Dir.chdir(Gemgem.dir) do
    gem = Gem::Package.build(Gem::Specification.load(Gemgem.spec_path))
    FileUtils.mkdir_p(Gemgem.pkg_dir)
    FileUtils.mv(gem, Gemgem.pkg_dir) # gem is relative path, but might be ok
  end
  puts "\e[35mGem built: \e[33m" \
       "#{Gemgem.strip_path("#{Gemgem.pkg_dir}/#{gem}")}\e[0m"
end

desc 'Generate gemspec'
task :spec do
  Gemgem.create
  Gemgem.write
end

desc 'Release gem'
task :release => [:spec, :check, :build] do
  Gemgem.module_eval do
    sh_git('tag', Gemgem.gem_tag)
    sh_git('push')
    sh_git('push', '--tags')
    sh_gem('push', Gemgem.gem_path)
  end
end

task :check do
  ver = Gemgem.spec.version.to_s

  if ENV['VERSION'].nil?
    puts("\e[35mExpected "                                  \
         "\e[33mVERSION\e[35m=\e[33m#{ver}\e[0m")
    exit(1)

  elsif ENV['VERSION'] != ver
    puts("\e[35mExpected \e[33mVERSION\e[35m=\e[33m#{ver} " \
         "\e[35mbut got\n         "                         \
         "\e[33mVERSION\e[35m=\e[33m#{ENV['VERSION']}\e[0m")
    exit(2)
  end
end

end # of gem namespace

desc 'Run tests in memory'
task :test do
  next if Gemgem.test_files.empty?

  Gemgem.test_files.each{ |file| require "#{Gemgem.dir}/#{file[0..-4]}" }
  Test::Unit::Runner.autorun
end

desc 'Trash ignored files'
task :clean => ['gem:spec'] do
  next if Gemgem.ignored_files.empty?

  require 'fileutils'
  trash = File.expand_path("~/.Trash/#{Gemgem.spec.name}")
  puts "Move the following files into:" \
       " \e[35m#{Gemgem.strip_path(trash)}\e[33m"

  Gemgem.ignored_files.each do |file|
    from = "#{Gemgem.dir}/#{file}"
    to   = "#{trash}/#{File.dirname(file)}"
    puts Gemgem.strip_path(from)

    FileUtils.mkdir_p(to)
    FileUtils.mv(from, to)
  end

  print "\e[0m"
end

task :default do
  # Is there a reliable way to do this in the current process?
  # It failed miserably before between Rake versions...
  exec "#{Gem.ruby} -S #{$PROGRAM_NAME} -f #{Rake.application.rakefile} -T"
end

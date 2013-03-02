
require 'pathname'

module Gemgem
  class << self
    attr_accessor :dir, :spec
  end

  module_function
  def create
    yield(spec = Gem::Specification.new{ |s|
      s.rubygems_version = Gem::VERSION
      s.date             = Time.now.strftime('%Y-%m-%d')
      s.files            = gem_files
      s.test_files       = gem_files.grep(%r{^test/(.+?/)*test_.+?\.rb$})
      s.executables      = Dir['bin/*'].map{ |f| File.basename(f) }
      s.require_paths    = %w[lib]
    })
    spec
  end

  def gem_tag
    spec.version.to_s
  end

  def gem_filename
    "#{spec.name}-#{spec.version}.gem"
  end

  def write
    File.open("#{dir}/#{spec.name}.gemspec", 'w'){ |f|
      f << split_lines(spec.to_ruby) }
  end

  def split_lines ruby
    ruby.gsub(/(.+?)\[(.+?)\]/){ |s|
      if $2.index(',')
        "#{$1}[\n  #{$2.split(',').map(&:strip).join(",\n  ")}]"
      else
        s
      end
    }
  end

  def all_files
    @all_files ||= find_files(Pathname.new(dir)).map{ |file|
      if file.to_s =~ %r{\.git/|\.git$}
        nil
      else
        file.to_s
      end
    }.compact.sort
  end

  def gem_files
    @gem_files ||= all_files - ignored_files
  end

  def ignored_files
    @ignored_file ||= all_files.select{ |path| ignore_patterns.find{ |ignore|
      path =~ ignore && !git_files.include?(path)}}
  end

  def git_files
    @git_files ||= if File.exist?("#{dir}/.git")
                     `git ls-files`.split("\n")
                   else
                     []
                   end
  end

  # protected
  def find_files path
    path.children.select(&:file?).map{|file| file.to_s[(dir.size+1)..-1]} +
    path.children.select(&:directory?).map{|dir| find_files(dir)}.flatten
  end

  def ignore_patterns
    @ignore_files ||= expand_patterns(
      gitignore.split("\n").reject{ |pattern|
        pattern.strip == ''
      }).map{ |pattern| %r{^([^/]+/)*?#{Regexp.escape(pattern)}(/[^/]+)*?$} }
  end

  def expand_patterns pathes
    pathes.map{ |path|
      if path !~ /\*/
        path
      else
        expand_patterns(
          Dir[path] +
          Pathname.new(File.dirname(path)).children.select(&:directory?).
            map{ |prefix| "#{prefix}/#{File.basename(path)}" })
      end
    }.flatten
  end

  def gitignore
    if File.exist?(path = "#{dir}/.gitignore")
      File.read(path)
    else
      ''
    end
  end
end

namespace :gem do

desc 'Install gem'
task :install => [:build] do
  sh("#{Gem.ruby} -S gem install pkg/#{Gemgem.gem_filename}")
end

desc 'Build gem'
task :build => [:spec] do
  sh("#{Gem.ruby} -S gem build #{Gemgem.spec.name}.gemspec")
  sh("mkdir -p pkg")
  sh("mv #{Gemgem.gem_filename} pkg/")
end

desc 'Release gem'
task :release => [:spec, :check, :build] do
  sh("git tag #{Gemgem.gem_tag}")
  sh("git push")
  sh("git push --tags")
  sh("#{Gem.ruby} -S gem push pkg/#{Gemgem.gem_filename}")
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
  $LOAD_PATH.unshift('lib')
  Dir['./test/**/test_*.rb'].each{ |file| require file[0..-4] }
  Test::Unit::Runner.autorun
end

task :default do
  puts `#{Gem.ruby} -S #{$PROGRAM_NAME} -T`
end

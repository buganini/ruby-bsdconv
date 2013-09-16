# encoding: utf-8

require "#{dir = File.dirname(__FILE__)}/task/gemgem"

Gemgem.dir = dir
($LOAD_PATH << File.expand_path("#{Gemgem.dir}/lib")).uniq!

desc 'Generate gemspec'
task 'gem:spec' do
  Gemgem.spec = Gemgem.create do |s|
    s.name    = 'ruby-bsdconv'
    s.version = '11.1.0'
    s.extensions  = 'ext/ruby-bsdconv/extconf.rb'
    s.authors     = ['Buganini Q']
    s.email       = 'buganini@gmail.com'
    s.homepage    = 'https://github.com/buganini/ruby-bsdconv'
    s.license     = 'BSD'
    s.summary     = 'ruby wrapper for bsdconv'
    s.description =
      "#{s.summary}. bsdconv is a BSD licensed charset/encoding converter" \
      " library with more functionalities than libiconv"
  end

  Gemgem.write
end

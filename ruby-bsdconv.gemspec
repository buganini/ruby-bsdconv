# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "ruby-bsdconv"
  s.version = "9.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Buganini Q"]
  s.date = "2012-12-03"
  s.description = "ruby wrapper for bsdconv. bsdconv is a BSD licensed charset/encoding converter library with more functionalities than libiconv"
  s.extensions = ["ext/ruby-bsdconv/extconf.rb"]
  s.files = [
  ".gitignore",
  "README",
  "Rakefile",
  "example/example.rb",
  "example/example2.rb",
  "example/example3.rb",
  "ext/ruby-bsdconv/bsdconv.c",
  "ext/ruby-bsdconv/extconf.rb",
  "ruby-bsdconv.gemspec",
  "task/gemgem.rb"]
  s.homepage = "https://github.com/buganini/ruby-bsdconv"
  s.require_paths = ["lib"]
  s.rubygems_version = "1.8.23"
  s.summary = "ruby wrapper for bsdconv"

  if s.respond_to? :specification_version then
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end

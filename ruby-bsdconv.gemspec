# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "ruby-bsdconv"
  s.version = "10.0.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Buganini Q"]
  s.date = "2013-04-24"
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
  "task/gemgem.rb",
  "test/test_basic.rb"]
  s.homepage = "https://github.com/buganini/ruby-bsdconv"
  s.require_paths = ["lib"]
  s.rubygems_version = "2.0.0"
  s.summary = "ruby wrapper for bsdconv"
  s.test_files = ["test/test_basic.rb"]
end

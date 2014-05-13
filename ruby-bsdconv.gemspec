# -*- encoding: utf-8 -*-
# stub: ruby-bsdconv 11.3.1 ruby lib
# stub: ext/ruby-bsdconv/extconf.rb

Gem::Specification.new do |s|
  s.name = "ruby-bsdconv"
  s.version = "11.3.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Buganini Q"]
  s.date = "2014-05-13"
  s.description = "ruby wrapper for bsdconv. bsdconv is a BSD licensed charset/encoding converter library with more functionalities than libiconv"
  s.email = "buganini@gmail.com"
  s.extensions = ["ext/ruby-bsdconv/extconf.rb"]
  s.files = [
  ".gitignore",
  "README.md",
  "Rakefile",
  "doc/api.rst",
  "example/example_chunk.rb",
  "example/example_conv.rb",
  "example/example_conv_file.rb",
  "example/example_ctl.rb",
  "example/example_insert_replace.rb",
  "example/example_modules.rb",
  "ext/ruby-bsdconv/bsdconv.c",
  "ext/ruby-bsdconv/extconf.rb",
  "ruby-bsdconv.gemspec",
  "task/gemgem.rb",
  "test/test_basic.rb"]
  s.homepage = "https://github.com/buganini/ruby-bsdconv"
  s.licenses = ["BSD"]
  s.rubygems_version = "2.2.2"
  s.summary = "ruby wrapper for bsdconv"
  s.test_files = ["test/test_basic.rb"]
end

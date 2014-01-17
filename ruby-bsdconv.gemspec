# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "ruby-bsdconv"
  s.version = "11.2.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Buganini Q"]
  s.date = "2014-01-18"
  s.description = "ruby wrapper for bsdconv. bsdconv is a BSD licensed charset/encoding converter library with more functionalities than libiconv"
  s.email = "buganini@gmail.com"
  s.extensions = ["ext/ruby-bsdconv/extconf.rb"]
  s.files = [
  ".gitignore",
  "README.md",
  "Rakefile",
  "doc/api.rst",
  "example/example_chunk.rb",
  "example/example_codecs.rb",
  "example/example_conv.rb",
  "example/example_conv_file.rb",
  "example/example_ctl.rb",
  "example/example_insert_replace.rb",
  "ext/ruby-bsdconv/Makefile",
  "ext/ruby-bsdconv/bsdconv.c",
  "ext/ruby-bsdconv/bsdconv.o",
  "ext/ruby-bsdconv/bsdconv.so",
  "ext/ruby-bsdconv/extconf.rb",
  "ext/ruby-bsdconv/mkmf.log",
  "ruby-bsdconv.gemspec",
  "task/gemgem.rb",
  "test/test_basic.rb"]
  s.homepage = "https://github.com/buganini/ruby-bsdconv"
  s.licenses = ["BSD"]
  s.require_paths = ["lib"]
  s.rubygems_version = "1.8.23"
  s.summary = "ruby wrapper for bsdconv"
  s.test_files = ["test/test_basic.rb"]

  if s.respond_to? :specification_version then
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end

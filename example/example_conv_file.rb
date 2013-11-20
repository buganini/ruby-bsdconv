#!/usr/bin/env ruby

require 'bsdconv'

c = Bsdconv.new(ARGV[0])

if c.nil?
	abort(Bsdconv.error())
end

c.conv_file(ARGV[1], ARGV[2])

p c
p c.counter

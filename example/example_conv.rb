#!/usr/bin/env ruby

require 'bsdconv'

c = Bsdconv.new(ARGV[0])

if c.nil?
	abort(Bsdconv.error())
end

s = ""
c.init
while $stdin.gets
	s += $_
end
puts c.conv(s)

p c.counter

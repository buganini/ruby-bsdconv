#!/usr/bin/env ruby

require 'bsdconv'

c = Bsdconv.new(ARGV[0])

if c.nil?
	abort('conversion error')
end

c.init
while $stdin.gets
	puts c.conv_chunk($_)
end
puts c.conv_chunk_last('')

p c.info

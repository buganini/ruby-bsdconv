#!/usr/bin/env ruby

require 'bsdconv'

c = Bsdconv.new(ARGV[0])

c.init
while $stdin.gets
	puts c.conv_chunk($_)
end
puts c.conv_chunk_last('')

p c.info

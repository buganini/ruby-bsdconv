#!/usr/bin/env ruby

require 'bsdconv'

c=Bsdconv.new('utf-8:utf-8')

if c.nil?
	abort(Bsdconv.error())
end

c.insert_phase('full',Bsdconv::INTER, 1)
puts c.conv('test')

p Bsdconv.codec_check(Bsdconv::FROM, "_utf-8")
p Bsdconv.codec_check(Bsdconv::INTER, "_utf-8")
puts 'From:'
p Bsdconv.codecs_list(Bsdconv::FROM)
puts 'Inter:'
p Bsdconv.codecs_list(Bsdconv::INTER)
puts 'To:'
p Bsdconv.codecs_list(Bsdconv::TO)

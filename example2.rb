#!/usr/bin/env ruby

require 'bsdconv'

c=Bsdconv.new('utf-8:utf-8')

c.insert_phase('full',Bsdconv::INTER, 1)
puts c.conv('test')

p Bsdconv.codec_check(Bsdconv::FROM, "_utf-8")
p Bsdconv.codec_check(Bsdconv::INTER, "_utf-8")
p Bsdconv.codecs_list

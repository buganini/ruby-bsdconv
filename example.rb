require 'bsdconv'

conv = bsdconv.new('utf-8,ascii:upper:utf-8,ascii')
puts conv.conv('test test')

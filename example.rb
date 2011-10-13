require 'bsdconv'

c = Bsdconv.new('utf-8,ascii:upper:utf-8,ascii')
puts c.conv('test test')

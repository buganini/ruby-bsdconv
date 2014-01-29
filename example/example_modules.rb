#!/usr/bin/env ruby

require 'bsdconv'

p Bsdconv.module_check(Bsdconv::FROM, "_utf-8")
p Bsdconv.module_check(Bsdconv::INTER, "_utf-8")
puts 'Filter:'
p Bsdconv.modules_list(Bsdconv::FILTER)
puts 'From:'
p Bsdconv.modules_list(Bsdconv::FROM)
puts 'Inter:'
p Bsdconv.modules_list(Bsdconv::INTER)
puts 'To:'
p Bsdconv.modules_list(Bsdconv::TO)

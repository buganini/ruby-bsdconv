# encoding: utf-8

require 'test/unit'
require 'bsdconv'

class TestBasic < Test::Unit::TestCase
  def test_basic
    c = Bsdconv.new('big5:utf-8')
    c.init
    assert_equal '許功蓋',
      c.conv_chunk("\263\134\245\134\273\134").force_encoding('utf-8')
  end
end

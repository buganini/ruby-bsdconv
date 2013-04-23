# encoding: utf-8

require 'test/unit'
require 'bsdconv'

class TestBasic < Test::Unit::TestCase
  def test_basic
    c = Bsdconv.new('big5:utf-8')
    c.init
    assert_equal '許功蓋',
      c.conv_chunk("\263\134\245\134\273\134").force_encoding('utf-8')

    sin="utf-8:utf-8,ascii"
    sout=Bsdconv.insert_phase(sin, "upper", Bsdconv::INTER, 1)
    assert_equal "UTF-8:UPPER:UTF-8,ASCII", sout

    sin=sout
    sout=Bsdconv.replace_phase(sin, "full", Bsdconv::INTER, 1)
    assert_equal "UTF-8:FULL:UTF-8,ASCII", sout

    sin=sout
    sout=Bsdconv.replace_codec(sin, "big5", 2, 1)
    assert_equal "UTF-8:FULL:UTF-8,BIG5", sout

    sin=sout
    sout=Bsdconv.insert_codec(sin, "ascii", 0, 1)
    assert_equal "UTF-8,ASCII:FULL:UTF-8,BIG5", sout
  end
end

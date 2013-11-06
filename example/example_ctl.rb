#!/usr/bin/env ruby

require 'bsdconv'

score,path=Bsdconv::mktemp("score.XXXXXX")
list=open("characters_list.txt","w+")

c=Bsdconv.new('utf-8:score-train:null')
c.init

c.ctl(Bsdconv::CTL_ATTACH_SCORE, score, 0);
c.ctl(Bsdconv::CTL_ATTACH_OUTPUT_FILE, list, 0);

open(ARGV[0], "r") { |fp|
	while !fp.eof?
		c.conv_chunk(fp.read(1024))
	end
	c.conv_chunk_last("")
}

File.unlink(path)

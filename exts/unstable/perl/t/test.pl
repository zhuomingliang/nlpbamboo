#!/usr/bin/env perl

use ExtUtils::testlib;
use bamboo;
use Data::Dumper;

my $h = bamboo::init("crf_seg", "");
print bamboo::setopt($h, 0, "你好朋友");
print bamboo::parse($h);
bamboo::clean($h);

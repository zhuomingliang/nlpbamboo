#!/usr/bin/env perl

use ExtUtils::testlib;
use bamboo;
use Data::Dumper;

my $h = bamboo::init("");
print bamboo::parse($h, "你好朋友");
bamboo::clean($h);

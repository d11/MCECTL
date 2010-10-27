#!/usr/bin/env perl
# Build tags file
use strict;
use warnings;
use IPC::Run3;

my $base_dir = '/home/dan/4thYear/MCECTL';

my @cmd = (
   'ctags',
   '-f', "$base_dir/tags",
   '-R',
   "$base_dir/src/",
);

run3[ @cmd ];

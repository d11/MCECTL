#!/usr/bin/env perl
use strict;
use warnings;
use IPC::Run3;

my $src_dir = $ENV{MBASE};

my @dest_dirs = (
   'linux.ox.ac.uk:~/backup/4thYear',
   'm07dh@ecs.ox.ac.uk:~/backup/4thYear',
);

for my $dest_dir (@dest_dirs) {

   run3 [
      'rsync',
      '-avz',
      $src_dir,
      $dest_dir,
   ];

}

#rsync -avz . linux.ox.ac.uk:~/backup/4thYear
# rsync -avz . m07dh@ecs.ox.ac.uk:~/backup/4thYear

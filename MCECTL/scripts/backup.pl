#!/usr/bin/env perl
use strict;
use warnings;
use IPC::Run3;
use Shell::Source;

my $sh = Shell::Source->new(
   shell => "sh",
   file  => $ENV{HOME}."/.keychain/".$ENV{HOSTNAME}."-sh"
);
$sh->inherit;

my $src_dir = $ENV{MBASE};

if ( (!$src_dir) || (! -x $src_dir) ) {
   $src_dir = '/home/dan/4thYear';
   #die "Couldn't find source dir";
}

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

   if ( $? != 0 ) {
      print "Rsync to $dest_dir failed!";
      next;
   }

}

#rsync -avz . linux.ox.ac.uk:~/backup/4thYear
# rsync -avz . m07dh@ecs.ox.ac.uk:~/backup/4thYear

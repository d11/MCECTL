#!/usr/bin/env perl
# Script for converting from JimpleToPDSolver output to MCECTL input language.
# Reads from STDIN and writes to STDOUT
use strict;
use warnings;
use Data::Dumper;

# Ensures the names and symbols use only the allowed characters
sub normalize {
   my $input = shift;
   my $out = lc($input);
   if ($out eq '#') {
      $out = '_';
   }
   return $out;
}

# Find the start of the rules list
while (<> !~ /Rules:/) { }

# Read and store the list of rules
my @rules;
my $line = "";
while (1) {
   chomp($line = <>);
   if ( $line ~~ /Propositions:/ ) {
      last;
   }
   my @words = split(/ +|;/,$line);
   if (!scalar @words) {
      next
   }
   my $state1 = shift @words;
   my $stack1 = shift @words;
   if (shift @words ne '->') {
      warn "Invalid rule?";
      warn $state1;
      warn $stack1;
      warn Dumper(\@words);
      next;
   }
   my $state2  = shift @words;
   my $stack2a = shift @words;
   my $stack2b = shift @words;
   unshift @rules, {
      from_state => normalize($state1),
      from_stack => normalize($stack1),
      to_state   => normalize($state2),
      to_stack_a => $stack2a,
      to_stack_b => $stack2b
      };
}

# Read the states & corresponding valuations
my @states;
while (1) {
   if (not ($line = <>)) {
      last;
   }
   chomp($line);
   my @words = split(/ |;/,$line);
   if (!scalar @words) {
      next
   }
   my $state1 = shift @words;
   my $stack1 = shift @words;
   unshift @states, {
      _state => normalize($state1),
      _stack => normalize($stack1),
      _vars  => [ map { normalize($_) } @words ]
   }
}

# Begin writing the PDS out again
print "PDS jimple_pds {\n";

# Write states
for my $state (@states) {
   print "   STATE ( " . $state->{_state}
       . "[" . $state->{_stack} . "] : "
       . join(",",@{$state->{_vars}}) . " )\n"
}

# Write rules
for my $rule (@rules) {
   my $action_name = "a";
   my $effect;
   if ($rule->{to_stack_b}) {
      $effect = "PUSH ".normalize($rule->{to_stack_b});
   } elsif ($rule->{to_stack_a}  && $rule->{to_stack_a} ne '_') {
      $effect = "REWRITE ".normalize($rule->{to_stack_a});
   } else {
      $effect = "POP";
   }

   print "   ACTION ( $action_name : "
         . $rule->{from_state} . "[" . $rule->{from_stack} . "]"
         . " -> " . $rule->{to_state} . "[$effect])\n";
}

print "}\n";

#!/usr/bin/env perl
use strict;
use warnings;
use Data::Dumper;

while (<> !~ /Rules:/) { }

my @rules;

my $line = "";
while ($line !~ /Propositions:/) {
   chomp($line = <>);
   my @words = split(/ |;/,$line);
   if (!scalar @words) {
      next
   }
   my $state1 = shift @words;
   my $stack1 = shift @words;
   if (shift @words ne '->') {
      warn "invalid rule?";
      warn $state1;
      warn $stack1;
      warn Dumper(\@words);
      next;
   }
   my $state2 = shift @words;
   my $stack2a = shift @words;
   my $stack2b = shift @words;
   unshift @rules, {
      from_state => $state1,
      from_stack => $stack1,
      to_state => $state2,
      to_stack_a => $stack2a,
      to_stack_b => $stack2b
   };
}


print "PDS jimple_pds {\n";

for my $rule (@rules) {
   my $action_name = "a";
   my $effect;
   if ($rule->{to_stack_b} && $rule->{to_stack_b} ne '_') {
      $effect = "PUSH ".$rule->{to_stack_b};
   } elsif ($rule->{to_stack_a} ) {
      $effect = "REWRITE ".$rule->{to_stack_a};
   } else {
      $effect = "POP";
   }

   print "ACTION ( $action_name : "
         . $rule->{from_state} . "[" . $rule->{from_stack} . "]"
         . " -> " . $rule->{to_state} . "[$effect])\n";
}

print "}\n";

while ($line) {
   chomp($line = <>);
   my @words = split(/ |;/,$line);
}

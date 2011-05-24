#!/usr/bin/env perl
#
# Black box testing for the model-checker, using the standard Perl test harness
# for convenience
#

use Test::More 'no_plan';
use Test::Deep;
use IPC::Run3;
use Data::Dumper;


my $model_checker = "./MCECTL-REPL";

# Run a single check, by piping input into the model-checker executable
sub model_check {
   my $ectl = shift;
   my $in_results = 0;
   my $results = {};

   # Subroutine recieves one output line at a time, and watches for check
   # results
   my $process_results = sub {
      my $line = shift;
      if ($line eq "Results: {\n") {
         $in_results = 1; return;
      }
      if (not $in_results) { return; }
      if ($line eq "}\n") {
         $in_results = 0; return;
      }
      if ($line =~ /\s*([<,>\w]+):\s*(T|F).*/) {
         $results->{$1} = $2 eq 'T' ? 1 : 0
      }
      warn $line;
   };
   run3( 
      [ $model_checker, '--file', '-'],
      \$ectl,
      \&$process_results
   );
   return $results;
}

# Format a state
sub state {
   my $data = shift;
   return "   STATE ( $data )";
};

# Format an action
sub action {
   my $data = shift;
   return "   ACTION ( $data )";
};
 
# Format an automaton
sub automaton {
   my $automaton = shift;
   my $type = $automaton->{type};
   my $name = $automaton->{name};
   return join(
      "\n", "$type $name {",
      (map { state($_) } @{$automaton->{states}}),
      (map { action($_) } @{$automaton->{rules}}),
      "}\n"
   );
};

# Format a formula
sub formula {
   my $formula = shift;
   my $name   = $formula->{name};
   my $inside = $formula->{formula};
   return "FORMULA $name {\n   $inside\n}\n";
};

# Run a single check
sub test_one {
   my $rh_test = shift;
   my $test_name = $rh_test->{name};
   my $formula   = $rh_test->{formula} ;
   my $expected  = $rh_test->{expected};
   my $system    = $rh_test->{system} ;
   
   my $automata  = join("\n", map { automaton($_) } @{$rh_test->{automata}} );
   my $check = ":check(" . $formula->{name} . ", " . $system->{name} . ")\n";

   my $input = automaton($system) . $automata . formula( $formula ) . $check;
   warn $input;

   my $results = model_check($input);

   cmp_deeply($results, $expected, $test_name);
}

# Name a group of tests the same way
sub test_suite {
   my $name = shift;
   my $ra_tests = shift;
   return [ map { $_->{name} = "[$name] " . $_->{name}; $_ } @{$ra_tests} ];
}


my $until_tests = test_suite("Until", [
   {
      name     => "1. LTS, basic non-pushdown",
      formula  => { name => "f1", formula => "E( p U[a1] q )" },
      system   => {
         type => "LTS", 
         name => "t1", 
         states => [ 's1: p', 's2: q', 's3:' ],
         rules => [ 'a: s1 -> s2', 'a: s2 -> s3' ]
      },
      automata => [
         {
            type => "PDA",
            name => "a1",
            states => [ '*t1' ],
            rules => [ 'a: t1[_] -> t1[REWRITE _]' ]
         }
      ],
      expected => { s1 => 1, s2 => 1, s3 => 0 }
   },
   {
      name     => "2. LTS, five states, pushdown with one symbol",
      formula  => { name => "phi8", formula => "E( 1 U[a8] p )" },
      system   => {
         type => "LTS", 
         name => "t13", 
         states => ['t0 : ' ,'t1 : ' ,'t2 : ' ,'t3 : ' ,'t4 : ' ,'t5 : p'],
         rules => [ 'a:t0->t1' ,'b:t1->t2' ,'a:t2->t3' ,'b:t3->t4' ,'c:t4->t5']
      },
      automata => [
         {
            type => "PDA",
            name => "a8",
            states => [ 's1', '*s2' ],
            rules => [
               'a: s1[_] -> s1[PUSH s]',
               'b: s1[s] -> s1[POP]',
               'c: s1[_] -> s2[REWRITE _]'
            ]
         }
      ],
      expected => { t0 => 1, t1 => 0, t2 => 1, t3 => 0, t4 => 1, t5 => 0}
   },
   {
      name => "3. PDS, basic pushdown",
      formula => { name => "f1", formula => "E( 1 U[dfa] p )" },
      system => {
         type => "PDS",
         name => "pds1",
         states => ['t1[_]:', 't1[s]:','t1[t]:','t1[u]: p' ],
         rules => [
            'a: t1[_] -> t1[PUSH s]',
            'a: t1[s] -> t1[PUSH t]',
            'a: t1[t] -> t1[PUSH u]'
         ]
      },
      automata => [
         {
            type => "DFA",
            name => "dfa",
            states => ['s1', '*s2'],
            rules => [
               'a: s1 -> s2',
               'a: s2 -> s1'
            ]
         }
      ],
      expected => { "<t1,_>" => 1, "<t1,s>" => 0, "<t1,t>" => 1, "<t1,u>" => 0 }
   }
#   {
#      name     => "#2 - ",
#      formula  => { name => "", formula => "" },
#      system   => ,
#      automata => [],
#      expected => { }
#   },
]);

my $release_tests = test_suite("Release", [
   {
      name     => "1. LTS, non-pushdown; basic check",
      formula  => { name => "f1", formula => "E( 0 R[a2] p )" },
      system   => {
         type => "LTS", 
         name => "t1", 
         states => [ 's1: ', 's2: ', 's3: p' ],
         rules => [ 'a: s1 -> s2', 'a: s1 -> s3' ]
      },
      automata => [
         {
            type => "PDA",
            name => "a2",
            states => [ 't1', '*t2' ],
            rules => [ 'a: t1[_] -> t2[REWRITE _]' ]
         }
      ],
      expected => { s1 => 1, s2 => 1, s3 => 1}
   },
   {
      name     => "2. LTS, non-pushdown; path escapes via b/fail state",
      formula  => { name => "f1", formula => "E( p R[ra4] q )" },
      system   => {
         type => "LTS", 
         name => "t4d", 
         states => [ 's1: ', 's2: q', 's3: ' ],
         rules => [ 'a: s1 -> s2', 'b: s2 -> s3', 'c: s3 -> s3' ]
      },
      automata => [
         {
            type => "PDA",
            name => "ra4",
            states => [ 't1', 't2', '*t3' ],
            rules => [ 'a: t1[_] -> t2[REWRITE _]', 'b: t2[_] -> t3[REWRITE _]' ]
         } 
      ],
      expected => { s1 => 0, s2 => 1, s3 => 1 }
   },
   {
      name => "3. LTS, non-pushdown; requires infinite path",
      formula => { name => "f1", formula => "E( 0 R[a1] ! p )"},
      system => {
         type => "LTS",
         name => "t1",
         states => ['s1: ', 's2: ', 's3: ' , 's4: p'],
         rules => [ 'a: s1 -> s2', 'b: s2 -> s3', 'a: s3 -> s2' ,'b: s2 -> s4' ]
      },
      automata => [ 
         {
            type => "PDA",
            name => "a1",
            states => ["*t1"],
            rules => ['a: t1[_] -> t1[REWRITE _]', 'b: t1[_] -> t1[REWRITE _]' ]
         }
      ],
      expected => { s1 => 1, s2 => 1, s3 => 1, s4 => 0 }
   },

   {
      name => "4. LTS, non-pushdown; all-until",
      formula => { name => "f1", formula => "A( p U[a1] q )"},
      system => {
         type => "LTS",
         name => "t1",
         states => ['s1: p', 's2: q', 's3: q' , 's4: p', 's5: q', 's6: p'],
         rules => [ 'a: s1 -> s2', 'a: s1 -> s3', 'a: s4 -> s5' ,'a: s4 -> s6' ]
      },
      automata => [ 
         {
            type => "PDA",
            name => "a1",
            states => ["*t1"],
            rules => ['a: t1[_] -> t1[REWRITE _]' ]
         }
      ],
      expected => { s1 => 1, s2 => 1, s3 => 1, s4 => 0, s5 => 1, s6 => 0 }
   },
   {
      name => "5. LTS, pushdown; all-until",
      formula => { name => "f1", formula => "A( p U[a1] q )"},
      system => {
         type => "LTS",
         name => "t1",
         states => ['s1: p', 's2: q', 's3: q' , 's4: p', 's5: q', 's6: p'],
         rules => [ 
            'b: s1 -> s1' , 'a: s1 -> s2' , 'a: s1 -> s3' , 
            'b: s4 -> s4' , 'a: s4 -> s5' , 'a: s4 -> s6'
         ]
      },
      automata => [ 
         {
            type => "PDA",
            name => "a1",
            states => ["t1", 't2', '*t3'],
            rules => ['b: t1[_] -> t2[PUSH s]','a: t2[s] -> t3[POP]'  ]
         }
      ],
      expected => { s1 => 1, s2 => 1, s3 => 1, s4 => 0, s5 => 1, s6 => 0 }
   },
   {
      name => "6. LTS, pushdown; all-until 2",
      formula => { name => "f1", formula => "A( ! p U[a1] p )"},
      system => {
         type => "LTS",
         name => "t1",
         states => ['s1: ', 's2: ', 's3: ' , 's4: p', 's5: q', 's6: p', 's7:'],
         rules => [ 
            'a: s1 -> s2' , 'b: s2 -> s3' , 'c: s3 -> s4' , 
            'c: s5 -> s6' , 'c: s5 -> s7' 
         ]
      },
      automata => [ 
         {
            type => "PDA",
            name => "a1",
            states => ["t1", '*t2'],
            rules => [
               'a: t1[_] -> t1[PUSH s]',
               'a: t1[s] -> t1[PUSH s]',
               'b: t1[s] -> t1[POP]',
               'c: t1[_] -> t2[REWRITE _]'  
            ]
         }
      ],
      expected => { s1 => 1, s2 => 1, s3 => 1, s4 => 1, s5 => 0, s6 => 1, s7 => 0 }
   }

]);

my $tests = [ @{$until_tests}, @{$release_tests} ];

for my $check (@{$tests}) {
   test_one($check);
}

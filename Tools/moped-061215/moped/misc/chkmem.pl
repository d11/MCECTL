#!/usr/bin/perl -w

%loc = ();
$silent = $ARGV[0];

while (<STDIN>) {
	print if $silent;
	tr/\015\012//d;
	$loc{$1} = $2 if (/malloc on (0x[0-9a-f]*) at (.*)/);
	if (/free on (0x[0-9a-f]*) at (.*)/) {
		printf "illegal free on location %s at %s\n",
			$1,$2 unless $silent || defined($loc{$1});
		delete $loc{$1};
	}
	printf "nil freed at $1\n" if (/free on \(nil\) at (.*)/);
}

for (keys %loc) {
	printf "location %s not freed (allocated at %s)\n",
		$_, $loc{$_};
}

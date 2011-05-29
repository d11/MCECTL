#!/usr/bin/env perl
use IPC::Run3;

if ( $ENV{MPROJECT} eq '' ) {
   print "MPROJECT environment variable was not set!\n";
   exit 1;
}

my $doc_dir = $ENV{MPROJECT} . "/doc";

my $pdf_viewer = "/usr/bin/evince";
my $doc_name = "Dissertation";
my $input_tex = "$doc_dir/$doc_name.tex";

if (-x $input_tex) {
   die "$input_tex doesn't exist";
}

run3 [
   'pdflatex',
   "-shell-escape",
   "-output-directory=$doc_dir",
   $input_tex,
];

my $output_pdf = "$doc_dir/$doc_name.pdf";

my $result = $?;

if ( $result != 0 ) {
   die "LaTeX failed! ($result)\n";
}

print "Success!\n";

run3 [
   $pdf_viewer,
   $output_pdf,
];


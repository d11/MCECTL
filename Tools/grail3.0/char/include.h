
#include	<stdlib.h>
#include	<iostream.h>
#include	<fstream.h>
#include	<strstream.h>
#include	<string.h>
#include	<ctype.h>

#include      "../classes/bits/include.h"
#include      "../classes/state/include.h"
#include      "../classes/array/include.h"
#include      "../classes/set/include.h"
#include      "../classes/pool/include.h"
#include      "../classes/list/include.h"
#include      "../classes/string/include.h"
#include      "../classes/pair/include.h"
#include      "../classes/inst/include.h"
#include      "../classes/re/include.h"
#include      "../classes/fm/include.h"
#include      "../classes/fl/include.h"
#include      "../classes/mealy/include.h"
#include      "../classes/afa/dnf/include.h"
#include      "../classes/afa/include.h"


char	string<char>::separator = '\0';
char	string<int>::separator = ' ';
char	string<unsigned int>::separator = ' ';
char	string<long>::separator = ' ';
char	string<unsigned long>::separator = ' ';
char	string<float>::separator = ' ';
char	string<double>::separator = ' ';

#ifdef          WATCOM
#include      "dosnames.h"
#endif
 
#ifndef         WATCOM
#include      "names.h"
#endif



typedef	void	(*PF)();			// pointer to function

extern	PF	set_new_handler(PF);

void 
new_error()
{
	cerr << "new failed\n";
	exit(-1);
}

//	This code copyright (c) by the Grail project. 
//	No commercial use permitted without written consent.  
//	February 1995 
 
#include	"grail.h" 
#include	"lexical.h" 
 
void	get_one(mealy<char, char>& a, int argc, char** argv, char* my_name); 
void	get_two(mealy<char, char>& a, mealy<char, char>& b, int argc, char** argv, char* my_name); 
void	get_one(re<pair<char, char> >& a, int argc, char** argv, char* my_name); 
void	get_two(re<pair<char, char> >& a, re<pair<char, char> >& b, int argc, char** argv, char* my_name); 
 
int 
main(int argc, char** argv) 
{ 
	char*		my_name; 
	mealy<char, char>	a; 
 
	set_new_handler(&new_error);		// error handler for new 
 
	// get my name 
 
	for (int i=strlen(argv[0]); i>0; --i) 
		if ((argv[0][i] == '/') || (argv[0][i] == '\\')) 
		{ 
			++i; 
			break; 
		} 
 
	my_name = argv[0] + i; 
 
	// handle DOS names 
 
	// do the operation that I'm named 
 
	if (strcmp(my_name, fmcomp) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		a.complete(); 
		cout << a; 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmcment) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		a.complement(); 
		cout << a;		 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmcat) == 0) 
	{ 
		mealy<char, char>	a; 
		mealy<char, char>	b; 
		get_two(a, b, argc, argv, my_name); 
		a ^= b; 
		cout << a; 
		return 0; 
	} 
 
	if (strcmp(my_name, fmcross) == 0) 
	{ 
		mealy<char, char>	a; 
		mealy<char, char>	b; 
		mealy<char, char>	c; 
		get_two(a, b, argc, argv, my_name); 
		c.cross_product(a, b); 
		cout << c; 
		return 0; 
	} 
 
	if (strcmp(my_name, fmenum) == 0) 
	{ 
		int			i = 100; 
		set<string<pair<char, char> > >	results;	 
 
		// extract number, if present 
 
		if (argc > 1 && strcmp(argv[1], "-n") == 0) 
		{ 
			if (argc < 3) 
			{ 
				cerr << "usage: " << my_name <<  
						" [-n size] mealy\n"; 
				return 1; 
			} 
 
			i = atoi(argv[2]); 
			argv[1] = argv[3]; 
			argc -= 2; 
		} 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
	 
		int j = a.enumerate(i, results); 
 
		// print number of requested results or total set, whichever 
		// is smaller 
 
 
		if (j < i) 
			i = j; 
 
		for (j=0; j<i; ++j) 
			cout << results[j] << "\n"; 
 
		return 0; 
	} 
	 
	if (strcmp(my_name, fmexec) == 0) 
	{ 
		int	 		i = 0; 
		string<char>		str; 
		string<char>		results; 
		fstream			fio; 
		istrstream		ist(argv[argc-1]); 
		mealy<char, char>	a; 
 
		// handle arguments 
 
		switch(argc) 
		{ 
			case 4: 
				if (strcmp(argv[1], "-d") != 0) 
				{ 
					cerr << "usage: " << my_name <<  
						" [-d] mealy string\n"; 
					return 1; 
				} 
 
				i = 1; 
				fio.open(argv[2], ios::in); 
				if (!fio) 
				{ 
					cerr << my_name << ": can't open "  
						<< argv[2] << "\n"; 
					return 1; 
				}; 
				fio >> a; 
				fio.close(); 
 
				ist >> str;	 
				break; 
 
			case 3: // automaton is arriving on stdin 
 
				if (strcmp(argv[1], "-d") == 0) 
				{ 
					ist >> str; 
					i = 1; 
					cin >> a; 
					break; 
				} 
 
				fio.open(argv[1], ios::in); 
				if (!fio) 
				{ 
					cerr << my_name << ": can't open " <<  
							argv[1] << "\n"; 
					return 1; 
				}; 
				fio >> a; 
				fio.close(); 
 
				ist >> str; 
				break; 
			case 2: 
				ist >> str; 
				cin >> a; 
				break; 
 
			default: 
				cerr << "usage: " << my_name <<  
						" [-d] mealy string\n"; 
				return 1; 
		} 
 
		if (a.deterministic_execution(str, results, i)) 
			cout  << "accepted\n"; 
		else 
			cout  << "not accepted\n"; 
 
		cout << results << endl; 
		return 0; 
 
	} 
 
	if (strcmp(my_name, fmmin) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		if (!a.is_deterministic()) 
		{ 
			cerr << "can't minimize nfm\n";	 
			return 1; 
		}	 
		a.min_by_partition(); 
		cout << a;		 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmminrev) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		if (!a.is_deterministic()) 
		{ 
			cerr << "can't minimize nfm\n";	 
			return 1; 
		}	 
		a.reverse(); 
		a.subset(); 
		a.reverse(); 
		a.subset(); 
		cout << a; 
		return 0; 
	} 
 
	if (strcmp(my_name, fmplus) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		a.plus(); 
		cout << a;		 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmrenum) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		if (!a.canonical_numbering()) 
		{ 
			cerr << my_name << ": can't renumber nfm\n"; 
			return 1; 
		} 
 
		cout << a;		 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmreverse) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		a.reverse(); 
		cout << a;		 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmreach) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		a.reachable_fm(); 
		cout << a;		 
		return 0;		 
	} 
 
	if (strcmp(my_name, fmstar) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a,  argc, argv, my_name); 
		a.star(); 
		cout << a; 
		return 0; 
	} 
 
	if (strcmp(my_name, fmtore) == 0) 
	{ 
		mealy<char, char>	a; 
		re<pair<char, char> >	r; 
 
		get_one(a, argc, argv, my_name); 
		r.fmtore(a); 
		cout << r << "\n"; 
		return 0; 
	} 
 
	if (strcmp(my_name, fmunion) == 0) 
	{ 
		mealy<char, char>	a; 
		mealy<char, char>	b; 
		get_two(a, b, argc, argv, my_name); 
		a += b; 
		cout << a;		 
		return 0;		 
	} 
 
 
	if (strcmp(my_name, fmdeterm) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		a.subset(); 
		cout << a;		 
		return 0;		 
	} 
	 
	if (strcmp(my_name, iscomp) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		if (a.is_complete()) 
		{ 
			cout << "complete\n"; 
			return 0;		 
		} 
		cout << "not complete\n"; 
		return 1;		 
	} 
 
	if (strcmp(my_name, isdeterm) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		if (a.is_deterministic()) 
		{ 
			cout << "deterministic\n"; 
			return 0;		 
		} 
		cout << "nondeterministic\n"; 
		return 1;		 
	} 
 
	if (strcmp(my_name, isomorph) == 0) 
	{ 
		mealy<char, char>	a; 
		mealy<char, char>	b; 
		get_two(a, b, argc, argv, my_name); 
 
		a.canonical_numbering();  
		b.canonical_numbering(); 
 
		if (a == b) 
		{ 
			cout << "isomorphic\n"; 
			return 0;		 
		} 
 
		cout << "nonisomorphic\n"; 
		return 1;		 
	} 
 
	if (strcmp(my_name, isuniv) == 0) 
	{ 
		mealy<char, char>	a; 
		get_one(a, argc, argv, my_name); 
		if (a.is_universal()) 
		{ 
			cout << "universal\n"; 
			return 0;		 
		} 
		cout << "nonuniversal\n"; 
		return 1;		 
	} 
 
	if (strcmp(my_name, recat) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		re<pair<char, char> >	r2; 
		get_two(r1, r2, argc, argv, my_name); 
		r1 ^= r2; 
		cout << r1 << "\n"; 
		return 0; 
	} 
 
	if (strcmp(my_name, remin) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		get_one(r1, argc, argv, my_name); 
		cout << r1 << "\n"; 
		return 0;		 
	} 
 
	if (strcmp(my_name, reunion) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		re<pair<char, char> >	r2; 
		get_two(r1, r2, argc, argv, my_name); 
		r1 += r2; 
		cout << r1 << "\n";		 
		return 0;		 
	} 
 
	if (strcmp(my_name, restar) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		get_one(r1,  argc, argv, my_name); 
		r1.star(); 
		cout << r1 << "\n"; 
		return 0; 
	} 
 
	if (strcmp(my_name, retofm) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		mealy<char, char>	a;
		get_one(r1,  argc, argv, my_name); 
		r1.retofm(a); 
		cout << a; 
		return 0; 
	} 
 
	if (strcmp(my_name, isnull) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		get_one(r1, argc, argv, my_name); 
		if (r1.is_empty_string()) 
			cout << "is empty string\n"; 
		else 
			cout << "is not empty string\n"; 
		return 0;		 
	} 
 
	if (strcmp(my_name, isempty) == 0) 
	{ 
		re<pair<char, char> >	r1; 
		get_one(r1, argc, argv, my_name); 
		if (r1.is_empty_set()) 
			cout << "is empty set\n"; 
		else 
			cout << "is not empty set\n"; 
		return 0;		 
	} 
 
	cerr << my_name << ": no such grail function" << endl; 
	return 1;		 
}; 
 
void 
get_two(mealy<char, char>& a, mealy<char, char>& b, int argc, char** argv, char* my_name) 
{ 
	fstream	f_arg; 
	fstream	g_arg; 
 
	// handle arguments 
 
	switch(argc) 
	{ 
	case 2:	 
		cin >> b; 
		f_arg.open(argv[1], ios::in); 
		if (!f_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[1] << "\n"; 
			exit(1); 
		}; 
		f_arg >> a; 
		break; 
 
	case 3: 
		f_arg.open(argv[1], ios::in); 
		if (!f_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[1] << "\n"; 
			exit(1); 
		}; 
		f_arg >> a; 
		f_arg.close(); 
 
		// we should be able to reuse f_arg, but for some reason 
		// it doesn't work under Watcom 9.5 
 
		g_arg.open(argv[2], ios::in); 
		if (!g_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[2] << "\n"; 
			exit(1); 
		}; 
 
		g_arg >> b; 
		break; 
 
	default: 
		cerr << my_name << ": requires two mealy's\n"; 
		exit(1); 
	} 
} 
 
void 
get_one(mealy<char, char>& a, int argc, char** argv, char* my_name) 
{ 
	fstream		f_arg; 
 
	if (argc > 2) 
	{ 
		cerr << "usage: " << my_name << " mealy\n"; 
		exit(1); 
	} 
 
	if (argc == 2) 
	{	 
		f_arg.open(argv[1], ios::in); 
		if (!f_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[1] << "\n"; 
			exit(1); 
		}; 
		f_arg >> a; 
	} 
	else 
		cin >> a; 
}; 
 
void 
get_two(re<pair<char, char> >& a, re<pair<char, char> >&b, int argc, char** argv, char* my_name) 
{ 
	fstream	f_arg; 
	fstream	g_arg; 
 
	// handle arguments 
 
	switch(argc) 
	{ 
	case 2:	 
		cin >> b; 
		f_arg.open(argv[1], ios::in); 
		if (!f_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[1] << "\n"; 
			exit(1); 
		}; 
		f_arg >> a; 
		break; 
 
	case 3: 
		f_arg.open(argv[1], ios::in); 
		if (!f_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[1] << "\n"; 
			exit(1); 
		}; 
		f_arg >> a; 
		f_arg.close(); 
 
                // we should be able to reuse f_arg, but for some reason 
                // it doesn't work under Watcom 9.5 
 
		g_arg.open(argv[2], ios::in); 
		if (!g_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[2] << "\n"; 
			exit(1); 
		}; 
 
		g_arg >> b; 
		break; 
 
	default: 
		cerr << my_name << ": requires two re's\n"; 
		exit(1); 
	} 
} 
 
void 
get_one(re<pair<char, char> >& a, int argc, char** argv, char* my_name) 
{ 
	fstream		f_arg; 
 
	if (argc > 2) 
	{ 
		cerr << "usage: " << my_name << " re\n"; 
		exit(1); 
	} 
 
	if (argc == 2) 
	{	 
		f_arg.open(argv[1], ios::in); 
		if (!f_arg) 
		{ 
			cerr << my_name << ": can't open " << argv[1] << "\n"; 
			exit(1); 
		}; 
		f_arg >> a; 
	} 
	else 
		cin >> a; 
}; 
 

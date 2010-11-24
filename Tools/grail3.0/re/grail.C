//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	January 1996

#include	"grail.h"
#include	"lexical.h"

void	get_one(fm<re<char> >& a, int argc, char** argv, char* my_name);
void	get_two(fm<re<char> >& a, fm<re<char> >& b, int argc, char** argv, char* my_name);
void	get_one(re<re<char> >& a, int argc, char** argv, char* my_name);
void	get_two(re<re<char> >& a, re<re<char> >& b, int argc, char** argv, char* my_name);
void    get_one(fl<re<char> >& a, int argc, char** argv, char* my_name);
void    get_two(fl<re<char> >& a, fl<re<char> >& b, int argc, char** argv, char* my_name);
void    get_two(fl<re<char> >& a, string<re<char> >& b, int argc,
                char** argv, char* my_name);


int
main(int argc, char** argv)
{
	char*		my_name;

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
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.complete();
		cout << a;
		return 0;		
	}

	if (strcmp(my_name, fmcment) == 0)
	{
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.complement();
		cout << a;		
		return 0;		
	}

	if (strcmp(my_name, fmcat) == 0)
	{
		fm<re<char> > a;
		fm<re<char> > b;
		get_two(a, b, argc, argv, my_name);
		a ^= b;
		cout << a;
		return 0;
	}

	if (strcmp(my_name, fmcross) == 0)
	{
		fm<re<char> > a;
		fm<re<char> > b;
		fm<re<char> > c;
		get_two(a, b, argc, argv, my_name);
		c.cross_product(a, b);
		cout << c;
		return 0;
	}

	if (strcmp(my_name, fmenum) == 0)
	{
		int			i = 100;
		set<string<re<char> > >	results;	

		// extract number, if present

		if (argc > 1 && strcmp(argv[1], "-n") == 0)
		{
			if (argc < 3)
			{
				cerr << "usage: " << my_name << 
						" [-n size] fm\n";
				return 1;
			}

			i = atoi(argv[2]);
			argv[1] = argv[3];
			argc -= 2;
		}
		fm<re<char> > a;
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
		int	 	i = 0;
		string<re<char> >	str;
		fstream		fio;
		istrstream	ist(argv[argc-1]);
		fm<re<char> >	a;

		// handle arguments

		switch(argc)
		{
			case 4:
				if (strcmp(argv[1], "-d") != 0)
				{
					cerr << "usage: " << my_name << 
						" [-d] fm string\n";
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
						" [-d] fm string\n";
				return 1;
		}

		if (a.member_of_language(str, i))
			cout  << "accepted\n";
		else
			cout  << "not accepted\n";
		return 0;

	}

	if (strcmp(my_name, fmmin) == 0)
	{
		fm<re<char> > a;
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
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.reverse();
		a.subset();
		a.reverse();
		a.subset();
		cout << a;
		return 0;
	}

	if (strcmp(my_name, fmplus) == 0)
	{
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.plus();
		cout << a;		
		return 0;		
	}

	if (strcmp(my_name, fmrenum) == 0)
	{
		fm<re<char> > a;
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
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.reverse();
		cout << a;		
		return 0;		
	}

	if (strcmp(my_name, fmreach) == 0)
	{
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.reachable_fm();
		cout << a;		
		return 0;		
	}

	if (strcmp(my_name, fmstats) == 0)
	{
		fm<re<char> > a;
		get_one(a,  argc, argv, my_name);
		a.stats(cout);
		return 0;
	}

	if (strcmp(my_name, fmstar) == 0)
	{
		fm<re<char> > a;
		get_one(a,  argc, argv, my_name);
		a.star();
		cout << a;
		return 0;
	}

        if (strcmp(my_name, fmtofl) == 0)
        {
	        fl<re<char> >        l;
		fm<re<char> >	a;
                get_one(a, argc, argv, my_name);
                if (!a.is_finite())
                {
                        cerr << my_name << ": not a finite language\n";
                        return 1;
                }
                a.fmtofl(l);
                cout << l;
                return 0;
        }

	if (strcmp(my_name, fmtore) == 0)
	{
		re<re<char> >	r;

		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		r.fmtore(a);
		cout << r << "\n";
		return 0;
	}

	if (strcmp(my_name, fmunion) == 0)
	{
		fm<re<char> > a;
		fm<re<char> > b;
		get_two(a, b, argc, argv, my_name);
		a += b;
		cout << a;		
		return 0;		
	}


	if (strcmp(my_name, fmdeterm) == 0)
	{
		fm<re<char> > a;
		get_one(a, argc, argv, my_name);
		a.subset();
		cout << a;		
		return 0;		
	}

        if (strcmp(my_name, flappend) == 0)
        {
		fl<re<char> > 	l;
		string<re<char> >	s;
                get_two(l, s, argc, argv, my_name);
                l.append(s);
                cout << l;
                return 0;
        }

        if (strcmp(my_name, flexec) == 0)
        {
                string<re<char> >    str;
                fstream         fio;
                istrstream      ist(argv[argc-1]);
		fl<re<char> >	l;

                // handle arguments
                switch(argc)
                {
                        case 3:
                                fio.open(argv[1], ios::in);
                                if (!fio)
                                {
                                        cerr << my_name << ": can't open " <<
                                                        argv[1] << "\n";
                                        return 1;
                                };
                                fio >> l;
                                fio.close();

                                ist >> str;
                                break;
                        case 2:
                                ist >> str;
                                cin >> l;
                                break;

                        default:
                                cerr << "usage: " << my_name <<
                                                " fl string\n";
                                return 1;
                }

                if (l.member_of_language(str))
                {
                        cout  << "accepted\n";
                        return 0;
                }
                cout  << "not accepted\n";
                return 0;
        }

        if (strcmp(my_name, flfilter) == 0)
        {
		fl<re<char> >		l;
		fm<re<char> >		a;
                fstream         	fio;

                // handle arguments
                switch(argc)
                {
                        case 3:
                                fio.open(argv[1], ios::in);
                                if (!fio)
                                {
                                        cerr << my_name << ": can't open " <<
                                                        argv[1] << "\n";
                                        return 1;
                                };
                                fio >> l;
                                fio.close();

                                fio.open(argv[2], ios::in);
                                if (!fio)
                                {
                                        cerr << my_name << ": can't open " <<
                                                        argv[1] << "\n";
                                        return 1;
                                };
                                fio >> a;
                                fio.close();
                                break;
                        case 2:
                                fio.open(argv[1], ios::in);
                                if (!fio)
                                {
                                        cerr << my_name << ": can't open " <<
                                                        argv[1] << "\n";
                                        return 1;
                                };
                                fio >> a;
                                fio.close();

                                cin >> l;
                                break;

                        default:
                                cerr << "usage: " << my_name <<
                                                " fl fm\n";
                                return 1;
                }
                a.flfilter(l);
                cout << l;
                return 0;
        }

        if (strcmp(my_name, fllq) == 0)
        {
		fl<re<char> >	l;
		string<re<char> >	s;
                get_two(l, s, argc, argv, my_name);
                l.left_quotient(s);
                cout << l;
                return 0;
        }

        if (strcmp(my_name, flprepend) == 0)
        {
		fl<re<char> >	l;
		string<re<char> >	s;
                get_two(l, s, argc, argv, my_name);
                l.prepend(s);
                cout << l;
                return 0;
        }

        if (strcmp(my_name, flprod) == 0)
        {
		fl<re<char> >	l;
		fl<re<char> >	m;
                get_two(l, m, argc, argv, my_name);
                l *= m;
                cout << l;
                return 0;
        }

        if (strcmp(my_name, flreverse) == 0)
        {
		fl<re<char> >	l;
                get_one(l, argc, argv, my_name);
                l.reverse();
                cout << l;
                return 0;
        }

        if (strcmp(my_name, flrq) == 0)
        {
		fl<re<char> >	l;
		string<re<char> >	s;
                get_two(l, s, argc, argv, my_name);
                l.right_quotient(s);
                cout << l;
                return 0;
        }

        if (strcmp(my_name, fltofm) == 0)
        {
		fl<re<char> >	l;
		fm<re<char> >	a;
                get_one(l, argc, argv, my_name);
                a.fltofm(l);
                cout << a;
                return 0;
        }

        if (strcmp(my_name, fltore) == 0)
        {
		fl<re<char> >	l;
		re<re<char> >	r;
                get_one(l, argc, argv, my_name);
                r.fltore(l);
                cout << r << endl;
                return 0;
        }

        if (strcmp(my_name, flunion) == 0)
        {
		fl<re<char> >	l;
		fl<re<char> >	m;
                get_two(l, m, argc, argv, my_name);
                l += m;
                cout << l;
                return 0;
        }

	if (strcmp(my_name, iscomp) == 0)
	{
		fm<re<char> > a;
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
		fm<re<char> > a;
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
		fm<re<char> > a;
		fm<re<char> > b;
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
		fm<re<char> > a;
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
		re<re<char> > r1;
		re<re<char> > r2;
		get_two(r1, r2, argc, argv, my_name);
		r1 ^= r2;
		cout << r1 << "\n";
		return 0;
	}

	if (strcmp(my_name, remin) == 0)
	{
		re<re<char> > r1;
		get_one(r1, argc, argv, my_name);
		cout << r1 << "\n";
		return 0;		
	}

	if (strcmp(my_name, reunion) == 0)
	{
		re<re<char> > r1;
		re<re<char> > r2;
		get_two(r1, r2, argc, argv, my_name);
		r1 += r2;
		cout << r1 << "\n";		
		return 0;		
	}

	if (strcmp(my_name, restar) == 0)
	{
		re<re<char> > r1;
		get_one(r1,  argc, argv, my_name);
		r1.star();
		cout << r1 << "\n";
		return 0;
	}

        if (strcmp(my_name, retofl) == 0)
        {
		fl<re<char> >	l;
		re<re<char> >	r;
                get_one(r,  argc, argv, my_name);
                if (!r.is_finite())
                {
                        cerr << my_name << ": not a finite language\n";
                        return 1;
                }
                r.retofl(l);
                cout << l;
                return 0;
        }


	if (strcmp(my_name, retofm) == 0)
	{
		re<re<char> > r1;
		fm<re<char> > a;
		get_one(r1,  argc, argv, my_name);
		r1.retofm(a);
		cout << a;
		return 0;
	}

	if (strcmp(my_name, isnull) == 0)
	{
		re<re<char> > r1;
		get_one(r1, argc, argv, my_name);
		if (r1.is_empty_string())
			cout << "is empty string\n";
		else
			cout << "is not empty string\n";
		return 0;		
	}

	if (strcmp(my_name, isempty) == 0)
	{
		re<re<char> > r1;
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
get_two(fm<re<char> >& a, fm<re<char> >& b, int argc, char** argv, char* my_name)
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
		cerr << my_name << ": requires two fm's\n";
		exit(1);
	}
}

void
get_one(fm<re<char> >& a, int argc, char** argv, char* my_name)
{
	fstream		f_arg;

	if (argc > 2)
	{
		cerr << "usage: " << my_name << " fm\n";
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
get_two(re<re<char> >& a, re<re<char> >&b, int argc, char** argv, char* my_name)
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
get_one(re<re<char> >& a, int argc, char** argv, char* my_name)
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

void
get_two(fl<re<char> >& a, fl<re<char> >&b, int argc, char** argv, char* my_name)
{
        fstream f_arg;
        fstream g_arg;

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
                cerr << my_name << ": requires two fl's\n";
                exit(1);
        }
}

void
get_one(fl<re<char> >& a, int argc, char** argv, char* my_name)
{
        fstream         f_arg;

        if (argc > 2)
        {
                cerr << "usage: " << my_name << " fl\n";
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
}

void
get_two(fl<re<char> >& a, string<re<char> >&b, int argc, char** argv, char* my_name)
{
        fstream f_arg;
        fstream g_arg;
        istrstream      ist(argv[argc-1]);

        // handle arguments

        switch(argc)
        {
        case 2:
                cin >> a;
                ist >> b;
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

                ist >> b;
                break;

        default:
                cerr << my_name << ": requires an fl and a string\n";
                exit(1);
        }
}


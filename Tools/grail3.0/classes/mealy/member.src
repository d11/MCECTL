//	This code copyright (c) by the Grail project. 
//	No commercial use permitted without written consent.  
//	February 1995 
 
template <class Input, class Output> 
int 
mealy<Input, Output>::execute(const string<Input>& wd, set<string<Output> >& output, const int verbose) const 
{ 
	int					i; 
	int					j; 
	int					k; 
	set<state>				end; 
	set<state>				temp2; 
	mealy<Input, Output>			bunch; 
	mealy<Input, Output>			temp; 
	set<pair<string<Output>, state> >	working; 
	set<pair<string<Output>, state> >	next; 
	Input					r; 
 
	// begin with start states 
 
	set<state> current = start_states; 
 
	// do for all characters 
 
	for (i=0; i<wd.size(); ++i) 
	{ 
		r = wd[i]; 
 
		// get the target set 
 
		bunch.clear(); 
 
		for (j=0; j<current.size(); ++j) 
		{ 
			// note that we are overriding fm's select 
			// with mealy's select 
		 
			select(r, current[j], SOURCE, temp);	 
			bunch.arcs += temp.arcs; 
		} 
 
		// if there are no states, return 
 
		if (bunch.size() == 0) 
		{ 
			if (verbose) 
			      cout << "no states acccessible on " << r << "\n"; 
			return 0; 
		} 
 
		// else, target set is the new start set 
 
		bunch.sinks(current); 
 
		if (verbose) 
			cout << "on " << r << " take instructions\n" << bunch; 
 
		// if nothing so far, initialize working 
 
		if (working.size() == 0) 
		{ 
			pair<string<Output>, state>	p; 
			string<Output>			s; 
 
			for (k=0; k<bunch.size(); ++k) 
			{ 
				s = bunch[k].get_label().get_right(); 
				p.assign(s, bunch[k].get_sink()); 
				working += p; 
			}	 
		} 
 
		// otherwise, create the next working set 
 
		next.clear();	 
		for (k=0; k<bunch.size(); ++k) 
		{ 
			pair<string<Output>, state>	p; 
			string<Output>			s; 
 
			for (j=0; j<working.size(); ++j) 
			{ 
				if (working[j].get_right() ==  
						bunch[k].get_source()) 
				{ 
					s = working[j].get_left(); 
					s += bunch[k].get_label().get_right(); 
					p.assign(s, bunch[k].get_sink()); 
					next += p; 
				} 
			} 
		} 
		working.clear(); 
		working = next; 
	} 
 
	// exhausted the string; now test for a final state in the set 
 
	bunch.sinks(temp2); 
	end.intersect(final_states, temp2); 
 
	if (end.size() != 0) 
	{ 
		for (i=0; i<working.size(); ++i) 
			output += working[i].get_left(); 
 
		if (verbose) 
			cout << "terminate on final states " << end << "\n"; 
		return 1; 
	} 
	else 
		return 0; 
} 

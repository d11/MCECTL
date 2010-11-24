//	This code copyright (c) by the Grail project. 
//	No commercial use permitted without written consent.  
//	February 1995 
 
template <class Input, class Output> 
ostream& 
operator<<(ostream& os, const mealy<Input, Output>& a) 
{ 
	int				i; 
	inst<pair<Input, Output> >	t; 
 
	// output start pseudo-instructions 
 
	for (i=0; i<a.start_states.size(); ++i) 
	{ 
		t.make_start(a.start_states[i]); 
		os << t << "\n"; 
	} 
 
	// output normal instructions 
 
	for (i=0; i<a.size(); ++i) 
		os << a[i] << "\n"; 
 
	// output final pseudo-instructions 
 
	for (i=0; i<a.final_states.size(); ++i) 
	{ 
		t.make_final(a.final_states[i]); 
		os << t << "\n"; 
	} 
 
	return os; 
} 

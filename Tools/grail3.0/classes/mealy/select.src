//	This code copyright (c) by the Grail project 
//	No commercial use permitted without written consent.  
//	February 1995 
 
template <class Input, class Output> 
mealy<Input, Output>& 
mealy<Input, Output>::select(const Input& r, const state& x, int opt,  
	mealy<Input, Output>& s) const 
{ 
	int	i; 
 
	s.clear(); 
 
	// some selections can be done more easily 
 
	if ((arcs.is_sorted() == 1) && (opt == SOURCE) && (arcs.size() > 16)) 
	{ 
		if ((i = find_first(x)) == -1) 
			return s; 
 
		for (; i<arcs.size(); ++i) 
		{ 
			if (!arcs[i].sourceis(x)) 
				break; 
			if (arcs[i].get_label().get_left() == r) 
				s.arcs.disjoint_union(arcs[i]); 
		} 
 
		return s; 
	} 
 
	for (i=0; i<arcs.size(); ++i) 
	{ 
		if (arcs[i].get_label().get_left() != r) 
			continue; 
 
		if ((opt == SOURCE || opt == EITHER) && arcs[i].sourceis(x)) 
		{ 
			s.arcs.disjoint_union(arcs[i]); 
			continue; 
		} 
 
		if ((opt == EITHER || opt == SINK) && arcs[i].sinkis(x)) 
			s.arcs.disjoint_union(arcs[i]); 
	} 
 
	return s; 
} 
 
template <class Input, class Output> 
int 
mealy<Input, Output>::find_first(const state& r) const 
{ 
	int	left = 0; 
	int	right = arcs.size(); 
	int	probe; 
	state	s1; 
 
	for (;;) 
	{ 
		probe = (right - left)/2 + left; 
 
		s1 = arcs[probe].get_source(); 
 
		// back up to first occurrence 
 
		if (s1 == r) 
		{ 
			while (s1 == r) 
				s1 = arcs[--probe].get_source(); 
 
			++probe; 
			return probe; 
		} 
 
		if (probe == left) 
		{ 
			if (probe < arcs.size()-1 &&  
				arcs[probe+1].get_source() == r) 
					return probe+1; 
 
			break; 
		} 
 
		if (s1 < r) 
			left = probe; 
 
		if (s1 > r) 
			right = probe; 
	} 
	return -1; 
} 

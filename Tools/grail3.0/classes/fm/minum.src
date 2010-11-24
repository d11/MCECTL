//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	September 1995

// *************************************************
// MH 98/09/09  This code does not appear to be used
// *************************************************

template <class Label>
int
fm<Label>::minimal_numbering()
{
	bits	map(max_state().value());

	// set a bit map

	for (int i=0; i<arcs.size(); ++i)
	{
		map.set(arcs[i].get_source().value());
		map.set(arcs[i].get_sink().value());
	}

	// for every zero in the map, renumber 
	// the max-valued transitions

	for (int i=0; i<map.size(); ++i)
	{
		if (map[i] != 0) 
			continue;

		for (j=0; j:wq

}

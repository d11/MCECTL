//	This code copyright (c) by the Grail project. 
//	No commercial use permitted without written consent.  
//	February 1995 
 
template <class Input, class Output> 
istream& 
operator>>(istream& is, pair<Input, Output>& p) 
{ 
	char token; 
	Input	i;
	Output	o;

	// get the opening bracket 
 
	is >> token; 
 
	is >> i;

	// get the comma 
 
	is >> token; 
 
	is >> o; 
 
	// get the closing bracket 
 
	is >> token; 

	p.assign(i, o);;	 
	return  is; 
} 

//	This code copyright (c) by the Grail project. 
//	No commercial use permitted without written consent.  
//	February 1995 
 
#ifndef		MEALY_CLASS_DEFINITION 

template <class In, class Out> 
class mealy : public fm<pair<In, Out> > 
{
#ifdef	WATCOM
	friend ostream&
	operator<<(ostream&, const mealy<In, Out>&);
#endif

	// protected functions 
 
	protected: 
 
	int		find_first(const state&) const; 
 
	// public functions 
 
	public: 
  
			mealy() { ; } 
			// mealy(const mealy<In, Out>&) { ; }
			// ~mealy() { ; }		 
	mealy<In, Out>&	operator=(const mealy<In, Out>& x); 
	int		execute(const string<In>&, set<string<Out> >&,
				const int) const; 
	int		deterministic_execution(const string<In>&,  
				string<Out>&, const int) const; 
	mealy<In, Out>&	select(const In&, const state&, int,  
				mealy<In, Out>&) const; 
}; 
 
#define		MEALY_CLASS_DEFINITION 
#endif 

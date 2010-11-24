//	This code copyright (c) by the Grail project. 
//	No commercial use permitted without written consent.  
//	February 1995 
 
#ifndef		PAIR_CLASS_DEFINITION 
 
template <class Input, class Output> 
class pair  
{
	private: 
 
		Input		left; 
		Output		right; 
 
	public: 
				pair() { ; } 
				pair(const pair<Input, Output>& p) 
				{ left = p.left; right = p.right; } 
				~pair()	{ ; } 
	void			assign(const Input& i, const Output& o) 
				{ left = i; right = o; } 
	pair<Input, Output>&	operator=(const pair<Input, Output>& l) 
				{ left = l.left; right = l.right; return *this;} 
	int			operator==(const pair<Input, Output>&) const; 
	int			operator!=(const pair<Input, Output>& p) const 
				{ return *this == p ? 0 : 1; } 
	int			operator<(const pair<Input, Output>&) const; 
	int			operator>(const pair<Input, Output>& p) const 
				{ return *this < p ? 0 : 1; } 
	Input			get_left() const
				{ return left; } 
	Output			get_right() const
				{ return right; } 
}; 
 
#define	PAIR_CLASS_DEFINITION 
#endif 

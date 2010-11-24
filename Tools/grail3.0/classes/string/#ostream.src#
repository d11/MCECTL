//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	July 1995

template <class S>
ostream&
operator<<(ostream& os, const string<S>& s)
{
	if (s.size() == 0)
		return os;

	for (int i=0; i<s.size()-1; ++i)
	{
		os << s[i];
		if (s.separator != '\0')
			os << s.separator;
	}
	os<<s[s.size() - 1];
	return os; 
}

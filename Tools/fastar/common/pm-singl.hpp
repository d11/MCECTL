/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef PM_SINGLE_HPP
#define PM_SINGLE_HPP
#define IN_PM_SINGLE_HPP

#include "string.hpp"

// A (pure virtual, abstract) single-keyword pattern matching class.

class PMSingle {
public:
	// To do some matching, pass an input String and a pointer
	// to a function, which takes an int, and returns an int, to
	// the member function match(). The member function will
	// then call back its parameter with the index of the
	// character to the right of the match. Matching can be
	// aborted by returning FALSE in the call back function.
	virtual void match( const String& S,
			bool callBack (int) ) = 0;
};

#undef IN_PM_SINGLE_HPP
#endif


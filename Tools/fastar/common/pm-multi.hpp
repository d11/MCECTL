/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef PM_MULTI_HPP
#define PM_MULTI_HPP
#define IN_PM_MULTIPLE_HPP

#include "string.hpp"
//#include "set.hpp"
#include <set>
#include <assert.h>

// A (pure virtual, abstract) multiple-keyword pattern matching class.

class PMMultiple {
public:
	// In order to perform matching, pass an input String and a
	// pointer to a function (which takes an int and a const
	// Set<String>& and returns an int) to member function
	// match(). It will call the function back with the index
	// of the character to the right of the match, and the set
	// of keywords that matched. To abort matching, the call
	// back function must return FALSE to match().
	virtual void match( const String& S,
		bool callBack (int, const std::set<String>&) ) = 0;
};

#undef IN_PM_MULTIPLE_HPP
#endif


/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef PM_HPP
#define PM_HPP
#define IN_PM_HPP

// A general (pure virtual, abstract) pattern matching class.

class PM {
	// In concrete classes derived from PM, the constructor will take
	// some type of pattern (a single keyword, a set of keywords, a
	// regular expression, etc.).

	// The matching is done by passing an input String and a pointer
	// to a function to the matcher member function. The arguments
	// to the function depend upon the type of pattern matcher.
};

#undef IN_PM_HPP
#endif


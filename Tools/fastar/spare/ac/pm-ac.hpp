/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef PM_AC_HPP
#define PM_AC_HPP
#define IN_PM_AC_HPP

#include "alphabet.hpp"
#include "pm-multi.hpp"
#include "state.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include <assert.h>
#include <iostream>

// A multiple-keyword pattern matching class for the
// Aho-Corasick pattern matching algorithms.
// The template parameter must be one of the ACMachine... classes.

template<class T>
class PMAC : public PMMultiple {
public:
	PMAC( const std::set<String>& P );
	PMAC( const PMAC& M );

	virtual void match( const String& S,
		bool callBack (int, const std::set<String>&) );

	bool c_inv() const;
	template<class T2>
		friend std::ostream& operator<<( std::ostream& os, const PMAC<T2>& r ); // forward
private:
	#include "pm-ac_p.hpp"
};

#include "pm-ac_i.hpp"

#undef IN_PM_AC_HPP
#endif


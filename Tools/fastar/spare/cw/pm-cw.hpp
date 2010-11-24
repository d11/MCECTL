/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef PM_CW_HPP
#define PM_CW_HPP
#define IN_PM_CW_HPP

#include "alphabet.hpp"
#include "tries.hpp"
#include "cwout.hpp"
#include "pm-multi.hpp"
//#include "set.hpp"
#include <set>
#include "string.hpp"
#include <assert.h>
#include <iostream>

// A multiple-keyword pattern matching class for the
// Commentz-Walter family of pattern matchers.
// The template parameter must be a Commentz-Walter shifter,
// ie. one of the CWShift... classes.

template<class T>
class PMCW : public PMMultiple {
public:
	PMCW( const std::set<String>& P );
	PMCW( const PMCW<T>& r );

	// The actual matching function.
	virtual void match( const String& S,
		bool callBack (int, const std::set<String>&) );

	bool c_inv() const;
	template<class T2>
		friend std::ostream& operator<<( std::ostream& os, const PMCW<T2> r ); // forward
private:
	#include "pm-cw_p.hpp"
};

#include "pm-cw_i.hpp"

#undef IN_PM_CW_HPP
#endif


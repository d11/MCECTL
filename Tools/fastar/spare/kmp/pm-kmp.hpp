/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef PM_KMP_HPP
#define PM_KMP_HPP
#define IN_PM_KMP_HPP

#include "alphabet.hpp"
#include "pm-singl.hpp"
#include "string.hpp"
#include "failidx.hpp"
#include <assert.h>
#include <iostream>

// A concrete single-keyword pattern matching class for
// the Knuth-Morris-Pratt pattern matching algorithm.

class PMKMP : public PMSingle {
public:
	PMKMP( const String& kw );
	PMKMP( const PMKMP& r );

	// The matcher.
	virtual void match( const String& S, bool callBack (int) );
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const PMKMP& t );
private:
	#include "pm-kmp_p.hpp"
};

#include "pm-kmp_i.hpp"

#undef IN_PM_KMP_HPP
#endif


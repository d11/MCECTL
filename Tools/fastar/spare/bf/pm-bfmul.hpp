/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef PM_BFMUL_HPP
#define PM_BFMUL_HPP
#define IN_PM_BFMUL_HPP

#include "com-misc.hpp"
#include "pm-multi.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include <assert.h>
#include <iostream>

// A Brute-Force multi keyword pattern matcher.

class PMBFMulti : public PMMultiple {
public:
	PMBFMulti( const std::set<String>& keywords );

	// Perform the actual matching.
	virtual void match( const String& S,
		bool callBack (int, const std::set<String>&) );

	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const PMBFMulti& r );
private:
	#include "pm-bfmul_p.hpp"
};

#include "pm-bfmul_i.hpp"

#undef IN_PM_BFMUL_HPP
#endif


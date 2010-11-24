/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef PM_BFSIN_HPP
#define PM_BFSIN_HPP
#define IN_PM_BFSIN_HPP

#include "com-misc.hpp"
#include "pm-singl.hpp"
#include "string.hpp"
#include <assert.h>
#include <iostream>

// A Brute-Force single keyword pattern matcher.

class PMBFSingle : public PMSingle {
public:
	PMBFSingle( const String& keyword );

	// Perform the actual matching.
	virtual void match( const String& S,
			bool callBack (int) );

	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const PMBFSingle& r );
private:
	#include "pm-bfsin_p.hpp"
};

#include "pm-bfsin_i.hpp"

#undef IN_PM_BFSIN_HPP
#endif


/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWCHARRLA_HPP
#define CWCHARRLA_HPP
#define IN_CWCHARRLA_HPP

#include "alphabet.hpp"
#include "com-misc.hpp"
#include "state.hpp"
//#include "set.hpp"
#include <set>
#include "symbolto.hpp"
#include "tries.hpp"
#include <assert.h>
#include <iostream>

// Character shift function for right lookahead in CW.

class CharRLA {
public:
	CharRLA( const RTrie& t, const std::set<String>& P );
	CharRLA( const CharRLA& r );

	// The shift itself.
	int operator[]( const char a ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CharRLA& r );
private:
	#include "cwcharrl_p.hpp"
};

#include "cwcharrl_i.hpp"

#undef IN_CWCHARRLA_HPP
#endif


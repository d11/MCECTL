/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWCHARBM_HPP
#define CWCHARBM_HPP
#define IN_CWCHARBM_HPP

#include "alphabet.hpp"
#include "com-misc.hpp"
#include "state.hpp"
//#include "set.hpp"
#include <set>
#include "symbolto.hpp"
#include "tries.hpp"
#include <assert.h>
#include <iostream>

// Character shift function for the CW-BM algorithm.

class CharBM {
public:
	CharBM( const RTrie& t, const std::set<String>& P );
	CharBM( const CharBM& r );

	// The shift itself.
	int operator[]( const char a ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CharBM& r );
private:
	#include "cwcharbm_p.hpp"
};

#include "cwcharbm_i.hpp"

#undef IN_CWCHARBM_HPP
#endif


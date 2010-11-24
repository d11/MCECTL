/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACGAMMA_HPP
#define ACGAMMA_HPP
#define IN_ACGAMMA_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "symbolto.hpp"
#include "tries.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// The optimized Aho-Corasick transition function.

class Gamma {
public:
	Gamma( const FTrie& t, const FFail& f );
	Gamma( const Gamma& r );

	// The transition function itself.
	State image( const State q, const char a ) const;
	// What is the size of this function.
	int size() const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const Gamma& t );
private:
	#include "acgamma_p.hpp"
};

#include "acgamma_i.hpp"

#undef IN_ACGAMMA_HPP
#endif


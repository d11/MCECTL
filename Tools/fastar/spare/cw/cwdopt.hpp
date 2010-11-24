/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWDOPT_HPP
#define CWDOPT_HPP
#define IN_CWDOPT_HPP

#include "com-misc.hpp"
#include "state.hpp"
#include "stateto.hpp"
#include "symbolto.hpp"
#include "tries.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// d_opt shift function.

class DOpt {
public:
	DOpt( const RTrie& t, const RFail& f );
	DOpt( const DOpt& r );

	// The shift itself.
	int shift( const State q, const char a ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const DOpt& r );
private:
	#include "cwdopt_p.hpp"
};

#include "cwdopt_i.hpp"

#undef IN_CWDOPT_HPP
#endif


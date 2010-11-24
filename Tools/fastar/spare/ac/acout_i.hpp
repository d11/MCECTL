/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_ACOUT_CPP) || defined(INLINING)

INLINE ACOutput::ACOutput( const ACOutput& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE const std::set<String>& ACOutput::operator[]( const State q ) const {
	assert( c_inv() );
	assert( FIRSTSTATE <= q && q < rep.size() );
	return( rep[q] );
}

INLINE int ACOutput::size() const {
	assert( c_inv() );
	return( rep.size() );
}

INLINE bool ACOutput::c_inv() const {
	return( rep.c_inv() );
}

#endif


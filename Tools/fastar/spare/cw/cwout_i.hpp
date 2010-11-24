/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWOUT_CPP) || defined(INLINING)

INLINE int CWOutput::isKeyword( const State q ) const {
	assert( c_inv() );
	return( rep[q] != 0 );
}

INLINE const String& CWOutput::operator[]( const State q ) const {
	assert( c_inv() );
	assert( isKeyword( q ) );
	return( *rep[q] );
}

#endif


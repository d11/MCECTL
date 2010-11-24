/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $

// This stuff is always inline. To out-of-line them, create
// an alphabet.cpp file.

// These two are just the identity function for now.
// Change them to support other alphabets, e.g. a,c,t,g.

inline char alphabetNormalize( const char a ) {
	return( a );
/*	switch( a ) {
	case 0:		return( 0 );
	case 'e':	return( 1 );
	case 'h':	return( 2 );
	case 'i':	return( 3 );
	case 'r':	return( 4 );
	case 's':	return( 5 );
	case 'y':	return( 6 );
	default:	assert( !"Incorrect character" );
	}
*/
}

inline char alphabetDenormalize( const char a ) {
	return( a );
/*	switch( a ) {
	case 0:		return( 0 );
	case 1:		return( 'e' );
	case 2:		return( 'h' );
	case 3:		return( 'i' );
	case 4:		return( 'r' );
	case 5:		return( 's' );
	case 6:		return( 'y' );
	default:	assert( !"Incorrect image" );
	}
*/
}


/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.2.2.1 $
// $Date: 2004/05/27 12:13:12 $

// This stuff is always inline.

inline int min( const int a, const int b ) {
	return( a < b ? a : b );
}

inline int max( const int a, const int b ) {
	return( a < b ? b : a );
}

inline bool shorter( const String a, const String b ) {
	return( a.length() < b.length() );
}

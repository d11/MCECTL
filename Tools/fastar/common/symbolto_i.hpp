/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"
#include "com-io.hpp"

template<class T>
INLINE SymbolTo<T>::SymbolTo() :
		rep( ALPHABETSIZE ) {
	assert( c_inv() );
}

template<class T>
INLINE SymbolTo<T>::SymbolTo( const SymbolTo<T>& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

template<class T>
INLINE T& SymbolTo<T>::map( const int index ) {
	assert( c_inv() );
//	return( rep.at( index ) );
	return( rep[index] );
}

template<class T>
INLINE const T& SymbolTo<T>::operator[]( const int index ) const {
	assert( c_inv() );
	return( rep[index] );
}

template<class T>
INLINE bool SymbolTo<T>::c_inv() const {
//	return( rep.size() == ALPHABETSIZE && rep.c_inv() );
	return( rep.size() == ALPHABETSIZE );
}

template<class T>
std::ostream& operator<<( std::ostream& os, const SymbolTo<T>& t ) {
	assert( t.c_inv() );
	os << "SymbolTo = (\n" << t.rep << ")\n";
	assert( t.c_inv() );
	return( os );
}


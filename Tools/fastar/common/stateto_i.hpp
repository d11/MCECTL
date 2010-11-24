/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"
#include "com-io.hpp"

template<class T>
INLINE StateTo<T>::StateTo( const int size ) :
		rep( size ) {
	assert( c_inv() );
}

template<class T>
INLINE StateTo<T>::StateTo( const StateTo<T>& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

template<class T>
INLINE T& StateTo<T>::map( const State index ) {
	assert( c_inv() );
//	return( rep.at( index ) );
	return( rep[index] );
}

template<class T>
INLINE const T& StateTo<T>::operator[]( const State index ) const {
	assert( c_inv() );
	return( rep[index] );
}

template<class T>
INLINE void StateTo<T>::setSize( const int s ) {
	assert( c_inv() );
//	rep.setSize( s );
	rep.resize( s );
	assert( c_inv() );
}

template<class T>
INLINE int StateTo<T>::size() const {
	assert( c_inv() );
	return( rep.size() );
}

template<class T>
INLINE bool StateTo<T>::c_inv() const {
//	return( rep.c_inv() );
	return( true );
}

template<class T>
std::ostream& operator<<( std::ostream& os, const StateTo<T>& t ) {
	assert( t.c_inv() );
	os << "StateTo = (\n" << t.rep << ")\n";
	assert( t.c_inv() );
	return( os );
}


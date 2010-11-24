/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef SYMBOLTO_HPP
#define SYMBOLTO_HPP
#define IN_SYMBOLTO_HPP

#include "alphabet.hpp"
//#include "array.hpp"
#include <vector>
#include <assert.h>
#include <iostream>

// Provide mappings from a symbol (in the alphabet) to a T.

template<class T>
class SymbolTo {
public:
	// The capacity must be [0..ALPHABETSIZE).
	SymbolTo();
	SymbolTo( const SymbolTo<T>& r );

	// Some mapping related members.
	// A changeable map.
	T& map( const int index );
	// A constant mapping.
	const T& operator[]( const int index ) const;
	bool c_inv() const;
	template<class T2>
		friend std::ostream& operator<<( std::ostream& os, const SymbolTo<T2>& t ); // forward
private:
	#include "symbolto_p.hpp"
};

#include "symbolto_i.hpp"

#undef IN_SYMBOLTO_HPP
#endif


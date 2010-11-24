/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACOUT_HPP
#define ACOUT_HPP
#define IN_ACOUT_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include <iterator>
#include "tries.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// The Aho-Corasick output function.

class ACOutput {
public:
	ACOutput( const std::set<String>& P, const FTrie& t, const FFail& f );
	ACOutput( const ACOutput& r );

	// The output function itself.
	const std::set<String>& operator[]( const State q ) const;
	// What is the size of this function?
	int size() const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const ACOutput& t );
private:
	#include "acout_p.hpp"
};

#include "acout_i.hpp"

#undef IN_ACOUT_HPP
#endif


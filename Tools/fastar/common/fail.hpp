/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef FAIL_HPP
#define FAIL_HPP
#define IN_FAIL_HPP

#include "alphabet.hpp"
#include "state.hpp"
#include "stateto.hpp"
#include "trie.hpp"
#include <assert.h>
#include <iostream>

// Failure functions. For the client level, see fails.hpp.
// The template parameter must be one of the traversers. When
// the `forward' traverser is used, the failure function is
// constructed from the forward trie, and likewise for the
// `reverse' traverser. This yields the forward and reverse
// failure functions.

template<class T>
class Fail {
public:
	Fail( const Trie<T>& r );
	Fail( const Fail<T>& r );

	// Some failure function stuff.
	// What is the failure of State q?
	State operator[]( const State q ) const;
	// What is the size of the failure function?
	int size() const;
	bool c_inv() const;
	template<class T2>
		friend std::ostream& operator<<( std::ostream& os, const Fail<T2>& t ); // forward
private:
	#include "fail_p.hpp"
};

#include "fail_i.hpp"

#undef IN_FAIL_HPP
#endif


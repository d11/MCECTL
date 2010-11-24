/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACEFTRIE_HPP
#define ACEFTRIE_HPP
#define IN_ACEFTRIE_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "symbolto.hpp"
#include "tries.hpp"
#include <assert.h>
#include <iostream>

// The extended forward trie, for use in the failure
// function Aho-Corasick algorithm.

class EFTrie {
public:
	EFTrie( const FTrie& t );
	EFTrie( const EFTrie& r );

	// The trie stuff.
	State image( const State q, const char a ) const;
	// What is the size of the domain.
	int size() const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const EFTrie& t );
private:
	#include "aceftrie_p.hpp"
};

#include "aceftrie_i.hpp"

#undef IN_ACEFTRIE_HPP
#endif


/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef TRIES_HPP
#define TRIES_HPP
#define IN_TRIES_HPP

#include "trie.hpp"
#include "stravfwd.hpp"
#include "stravrev.hpp"

// Define a couple of different types of tries.

// Forward tries:
typedef Trie<STravFWD> FTrie;
// Reverse tries:
typedef Trie<STravREV> RTrie;

#undef IN_TRIES_HPP
#endif


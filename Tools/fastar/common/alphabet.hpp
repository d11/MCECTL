/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef ALPHABET_HPP
#define ALPHABET_HPP
#define IN_ALPHABET_HPP

#include <limits.h>
#include <assert.h>

// Functions used to take care of having different alphabets.
// The alphabet is considered to consist only of the positive
// characters.
// Ensure that alphabetNormalize( 0 ) = 0.
// Change them to support other alphabets, e.g. a,c,t,g.

// Map the alphabet to [0..ALPHABETSIZE).
char alphabetNormalize( const char a );

// Inverse of alphabetNormalize().
char alphabetDenormalize( const char a );

// The size of the alphabet.
const int ALPHABETSIZE = (CHAR_MAX);
//const int ALPHABETSIZE = 7; // 0 plus e, h, i, r, s, y

#include "alphabet_i.hpp"

#undef IN_ALPHABET_HPP
#endif


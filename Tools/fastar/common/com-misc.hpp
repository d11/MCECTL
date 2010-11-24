/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts and FIRE Lite class libraries.
// $Revision: 1.2.2.2 $
// $Date: 2004/06/01 07:45:51 $
#ifndef COM_MISC_HPP
#define COM_MISC_HPP
#define IN_COM_MISC_HPP

#include <limits.h>
#include "string.hpp"

// Give some miscellaneous functions for SPARE Parts and
// FIRE Lite.
int min( const int a, const int b );
int max( const int a, const int b );
bool shorter( const String a, const String b );
#include "com-misc_i.hpp"

// And the units of these operators.
const int PLUSINFINITY = INT_MAX;
const int MINUSINFINITY = INT_MIN;

#undef IN_COM_MISC_HPP
#endif


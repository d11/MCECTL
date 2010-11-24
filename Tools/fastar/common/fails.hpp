/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef FAILS_HPP
#define FAILS_HPP
#define IN_FAILS_HPP

#include "fail.hpp"
#include "stravfwd.hpp"
#include "stravrev.hpp"

// Define a couple of different types of failure function.

// Forward failure function:
typedef Fail<STravFWD> FFail;
// Reverse failure function:
typedef Fail<STravREV> RFail;

#undef IN_FAILS_HPP
#endif


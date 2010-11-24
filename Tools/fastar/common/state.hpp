/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts and FIRE Lite class libraries.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef STATE_HPP
#define STATE_HPP
#define IN_STATE_HPP

// Define a State to be just an integer.
// Used in tries and so on.
typedef int State;

// Define a first State.
const State FIRSTSTATE = 0;
// Define an invalid State.
const State INVALIDSTATE = -1;

#undef IN_STATE_HPP
#endif


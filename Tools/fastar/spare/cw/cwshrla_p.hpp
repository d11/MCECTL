/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $

	// The order of the following members is critical to the
	// correctness of the constructors.

	// The following is wasted space, since it is only used in
	// the ctor.
	RFail *fr;

	// The actual implementation.
	DOpt dopt;
	D2 d2;
	CharRLA rla;


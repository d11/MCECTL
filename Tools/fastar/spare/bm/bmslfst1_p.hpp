/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $

	// The order of these declarations is critical:
	// Got to keep track of the index of the last char in the kw.
	const int lastIndex;
	// The character to skip on.
	// a is not normalized.
	const char a;
	// The distance to skip.
	// The following table _is_ normalized.
	SymbolTo<int> distance;


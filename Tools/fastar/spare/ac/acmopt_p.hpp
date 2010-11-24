/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $

	// The order of the following is critical, since it affects
	// the constructor order!

	// In order to construct the above functions, we need the
	// following; the wasteage is a tradeoff for speed.
	FTrie *trie;
	FFail *fail;

	// The actual implementation.
	Gamma gf;
	ACOutput out;


//	This code copyright (c) by the Grail project
//	No commercial use permitted without written consent.
//	June 1995

bits::bits(const int size) 
{
	block = sizeof(unsigned int) * 8;

	max = (size / block) + 1;
	b = new unsigned int[max + 1];
	b[max] = 0;
	clear();

	sz = 0;
}

bits::bits(const bits& f) 
{
	// test for self assignment
	if (this == &f)
		return;

	// otherwise, allocate and initialize
	block = f.block;
	max = f.max;
	sz = f.sz;
	b = new unsigned int[max + 1];
	b[max] = 0;
	memcpy(b, f.b, sz * sizeof(unsigned int));
}










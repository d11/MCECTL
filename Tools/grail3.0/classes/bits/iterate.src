//	This code copyright (c) by the Grail project
//	No commercial use permitted without written consent.
//	September 1995

int
bits::next(int index) const
{
	int	i;

	++index;

	if (index >= sz * block)
		return -1;

	int offset = index / block;

	// if current block is nonzero, we must check from 
	// previous index position

	if (b[offset] != 0)
	{
		unsigned int temp = b[offset];
		unsigned int mask = 1;
/*		cout << "temp is ";
		for (int n=0; n<block; ++n)
			if (temp & (mask << n))
				cout << "1";
			else
				cout << "0";
	cout << "; looking for the one after " << index % block  << endl;  */
		for (int local = index % block; local < block; ++local)
			if (temp & ((unsigned int) mask << local))
			{
				/* cout << "...and found " << local << endl; */
				return (block * offset) + local;
			}
	}

	// otherwise, scan blocks

	for (++offset; offset < sz && b[offset] == 0; ++offset)
		;

	// are we at the end of the bits?

	if (offset == sz)
		return -1;

	// if not, return the first one bit

	unsigned int temp = b[offset];

	for (i=0; i<sizeof(int) * 8; ++i)
	{
		if ((temp & 1) == 1)
			break;
		temp = temp >> 1;
	}
			
	return(block * offset + i);
}


unsigned long crcTable[256];

unsigned long crcReflect (unsigned long x, int bits)
{
	int i;
	unsigned long v = 0, b = 1 << (bits - 1);

	for (i = 0; i < bits; i++)
	{
		if (x & 1) v += b;
		x >>= 1; b >>= 1;
	}
	return v;
}

void crcInit(void)
{
	unsigned long crcpol = 0x04c11db7;
	unsigned long i, j, k;

	for (i = 0; i < 256; i++)
	{
		k = crcReflect(i,8) << 24;
		for (j = 0; j < 8; j++)
			k = (k << 1) ^ ((k & 0x80000000)? crcpol : 0);
		crcTable[i] = crcReflect(k,32);
	}
}

unsigned long crcCompute (unsigned char *ptr, int len)
{
	unsigned long crcval = 0xffffffff;
	while (len--)
		crcval = (crcval >> 8) ^ crcTable[(crcval & 0xff) ^ *ptr++];
	return crcval;
}

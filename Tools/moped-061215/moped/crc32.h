/***************************************************************************/
/* Functions for computing the CRC32 value of a block of data.		   */

#ifndef CRC32_H
#define CRC32_H

extern void crcInit ();
extern unsigned long crcCompute (unsigned char*,int);

#endif

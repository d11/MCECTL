//	This code copyright (c) by the Grail project
//	No commercial use permitted without written consent.
//	June 1995

/***************************************************************************

  File:  classes/bits/bits.h
  -----

  Description:
  ------------
    This file contains the declaration for the bits class.  bits implements
    the code necessary to maintain a bitmap of any given size.  The size of
    the bitmap can be increased, individual bits can be cleared or set,
    groups of bits can be compared, and sections of the bitmap can be 
    extracted into a new bitmap.

    An instance of a bitmap can be declared in the following way:
      bits sample_bitmap(128);        // bitmap of size 128

  Revision History:
  -----------------
    The Grail Project               Initial version of source code
    M.Hoeberechts  98/05/25         Added header and comments

 ***************************************************************************/


#ifndef	BITS_CLASS_DEFINITION

class bits 
{
private:

	friend ostream& operator<<(ostream&, const bits&);

	unsigned int*	b;		// array of bits
	int		sz;		// number of used blocks
	int		max;		// allocated number of blocks
	int		block;		// number of bits per block

public:
			bits(const int i = 96);
			// constructor:  allocate a bitmap of the size 
			//  specified by the parameter.  All bits are initially
			//  set to 0.

			bits(const bits&);
			// copy constructor:  make the new bitmap an exact 
			//  copy of the parameter bitmap

			~bits() 
			// destructor:  free all memory used by the bitmap
			{ delete [] b; }

	int		operator[](int) const;
			// This function returns the value of the bit at the
  			//  specified index.  The return value will be 1 if
  			//  the bit is set and 0 otherwise.

	void		operator=(const bits&);
			// assignment operator:  assigns this bitmap to contain
			//  an exact copy of the bits in the parameter bitmap

	int		operator==(const bits&) const;
			// test for equality:  returns 1 if the bitmaps 
 			//  contain the same number of identically set bits
			//  and 0 otherwise

	int		operator<(const bits&) const;
			// less than operator:  returns 1 if this bitmap is
			//  less than the parameter bitmap and 0 otherwise

	int		operator>(const bits& b) const;
			// greater than operator:  returns 1 if this bitmap is
			//  less than the parameter bitmap and 0 otherwise

	bits&		operator+=(const bits&);
			// this function appends the bits in the parameter
			//  bitmap to the existing bits in this bitmap.  More
			//  space is allocated for the bitmap if necessary.

	// no definition for this function!?  MH 98/05/29
	bits&		operator+=(const int&);

	void		clear();
			// sets all the bits in this bitmap to 0

	void		clear(const int v)
			// clears the bit specified by the parameter index.
			//  The parameter must be a valid index since no
			//  checking is done in this function.
			{ b[v / block] &= ~((unsigned int) 1 << (v % block)); }

	void		increase(const int i);
			// increases the size of the bitmap by the number of
			//  bits specified by the parameter.  The new bits
			//  will be after any existing bits in the index.

	void		extract(bits&, const int) const;
			// copy the maximum number of bits which the parameter
         		//  bitmap can store from this bitmap starting at the
			//  bit specified by the integer parameter times the
			//  size of the parameter bitmap.

	int		member(const bits&) const;
			// attempts to find a section of bits in this bitmap
			//  which exactly matches the bits in the parameter
			//  bitmap.  Returns the index (in terms of sections
			//  of the size of the parameter bitmap) if a match
			//  is found, -1 otherwise.

	int		next(int index) const;
			// this function attempts to find a set bit after 
			//  index in this bitmap.  It returns the index of the
			//  bit if one is found and -1 otherwise.

#ifdef	ATT
	void		Set(const int v)
#else
	void		set(const int v)
#endif
			// sets the bit specified by the parameter index.
			//  The parameter must be a valid index since no
			//  checking is done in this function.
			{ b[v / block] |= ((unsigned int) 1 << (v % block)); }

	int		size(void) const 
			// returns the number of bits which are currently
			//  in use in the bitmap
			{ return sz * block; }
};

#define	BITS_CLASS_DEFINITION
#endif


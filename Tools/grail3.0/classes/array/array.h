//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	August 1994


/***************************************************************************

  File:  classes/array/array.h
  -----

  Description:
  ------------
    This file contains the type definition for a dynamic array template
    class.  The template class has one parameter, Item, which specifies
    the type of item to store in the array.  The array will grow as
    required to allow an unlimited number of items to be added.

    If the default constructor is used, the array starts with a maximum 
    size of ARRAY_SIZE (declared in classes/array/defs.h) elements, and the 
    size is doubled each time the maximum is exceeded.  The other constructors
    let you specify the size of the array or copy an existing array in order
    to initialize the new array.  New items are appended to the end of the 
    array.  When an item is deleted, the rest of the array elements are 
    compacted, but the array is not resized.  In both cases the order of 
    the elements is checked to determine whether the array is still sorted.

    Memory allocation for the arrays is done using a pool of arrays.  Instead
    of creating and destroying arrays, a small pool of arrays are maintained
    so that instantiating a new array consists of reusing an existing array
    (if one of the required size exists), and destroying an array returns it 
    to the pool.  This implementation greatly reduces the need to allocate 
    and free memory which provides substatial savings at run time, especially 
    when many temporary arrays are used.

    An example of a declaration using the array template is the following:
      array<char> char_array;         // create an array of characters

    Template functions for inputting and outputting arrays are also 
    defined (see classes/array/ostream.src and classes/array/istream.src).
    The function headers are the following:

	template <class Item>
	ostream&
	operator<<(ostream& os, const array<Item>& s)

	template <class Item>
	istream&
	operator>>(istream& is, array<Item>& s)


    The class specified for Item must have the following operators
    defined:

    Operator:                 Used In Function:
    ---------                 -----------------
    <                         compare, contain, <, member, -+, (sort)
    >                         merge, +=
    ==                        compare, member, -=, unique, (sort)
    !=                        ==, <
    =                         assign, -=, +=, remove, unique
    >>                        operator>>
    <<                        operator<<


  Revision History:
  -----------------
    D.Raymond                       Initial version of source code
    M.Hoeberechts  98/02/04         Added header and comments

 ***************************************************************************/




#ifndef		ARRAY_CLASS_DEFINITION

#include	<iostream.h>
#include	<ctype.h>
#include	<stdlib.h>

template <class Item>
class array {

protected:

	Item*		p;			// array of Items
	int		max;			// maximum size
	int		sz;			// current size
	int		sorted;			// is the array sorted?

	static array<Item>*     pool;           // pool of available arrays 
        static int              pool_sz;        // number of arrays in pool

// required for compilation with version 1.0 of xlC
#ifndef	XLC
        static int              max_pool;       // maximum number of arrays
#endif

public:
			array();
                        // constructor: creates an array of size ARRAY_SIZE

			array(int i);
                        // constructor: creates array of size i

			array(const array<Item>&);
                        // constructor: creates a new array which contains
                        //  a copy of the parameter array's elements

			~array();
                        // destructor:  returns the array to the pool of
                        //  available arrays.
                        
	array<Item>&	clear()
                        // clear the contents of this array
			{ sz = 0; sorted = 1; return *this; }

 	int		contain(const array<Item>&) const;
                        // returns 1 if all the elements in the parameter
                        //  array are contained in this array, 0 otherwise

	int		is_sorted() const
                        // returns 1 if this array is sorted, 0 otherwise
			{ return sorted; }

	static	int	compare(const Item*, const Item*);
                        // returns -1 if the first item is less than the
                        //  second item, 0 if they are equal, and 1
                        //  otherwise

	int		member(const Item&) const;
                        // returns the index of the parameter item if it is
                        //  found in the array, or -1 otherwise

	int		allocated() const
                        // returns the maximum number of elements which this
                        //  array can currently hold
			{ return max; }

	array<Item>&	operator=(const array<Item>&);
                        // assigns this array to contain the same elements
                        //  as the parameter array.  If the parameter array
                        //  contains more elements than this array can
                        //  currently hold, the size of this array is changed
                        //  to match the size of the parameter array.

	array<Item>&	operator=(const Item&);	
                        // assigns this array to contain only the parameter
                        //  element

	int		operator==(const array<Item>&) const;	
                        // returns 1 if this array contains all the same 
                        //  elements as the parameter array, 0 otherwise
                        
	int		operator!=(const array<Item>& l) const
                        // returns 1 if this array contains different elements
                        //  than the parameter array, 0 otherwise
			{ return *this == l ? 0 : 1; }
             
	array<Item>&	operator+=(const array<Item>&);
                        // array union: adds the contents of the parameter 
                        //  array to the contents of this array.  The maximum 
                        //  size of this array is increased if necessary.

 	array<Item>&	operator+=(const Item&);
                        // adds the parameter item to the array.  If the
                        //  array already contains the maximum number of 
                        //  elements, its size is doubled before the new
                        //  item is added.

 	array<Item>&	operator-=(const array<Item>&);
                        // array difference: removes any elements which are
                        //  in the parameter array and this array from this
                        //  array.  After this operation, the contents of the
                        //  arrays will be disjoint collections of items.

 	array<Item>&	operator-=(const Item&);
                        // removes the parameter item from the array.  If the
                        //  parameter item was not in the array, the array
                        //  contents are unchanged
 
 	int		operator<(const array<Item>&) const;
                        // returns 1 if the contents of this array are less
                        //  than the contents of the parameter array, 0
                        //  otherwise.  This array is judged to be less than
                        //  if it contains less elements than the parameter
                        //  array or if an element is encountered (in 
                        //  sequential order), which is less than the
                        //  corresponding element in the parameter array.

 	int		operator>(const array<Item>& s) const
                        // returns 1 if the contents of this array are greater
                        //  or equal to the contents of the parameter array, 0
                        //  otherwise.  This array is judged to be greater than
                        //  if it contains more elements than the parameter
                        //  array or if an element is encountered in the
                        //  parameter array (in sequential order), which is 
                        //  less than the corresponding element in this array.
			{ return s < *this ? 1 : 0; }

 	array<Item>&	remove(int i);
                        // removes the element at index i from this array.

 	void		merge(const array<Item>&);
                        // merges this array with the parameter array.  Both
                        //  arrays must be sorted before calling this
                        //  procedure.

	Item&		operator[](int i) const
                        //  returns the ith element of this array.
			{ return p[i]; }

	int		size() const
                        // returns the number of elements currently contained
                        //  in this array
			{ return sz; }

	void		sort() const;
                        // sorts the elements of this array according to the
                        //  compare function for items.  qsort is used to
                        //  perform the sort.

	void		swap(array<Item>& a)
                        // swaps the contents of this array with the contents
                        //  of the parameter array.
			{ int tmp = a.max; a.max = max; max = tmp;
			  tmp = a.sorted; a.sorted = sorted; sorted = tmp;
			  tmp = a.sz; a.sz = sz; sz = tmp;
			  Item* ptr = a.p; a.p = p; p =ptr; }

	array<Item>&	unique();
                        // sort the array, then remove any duplicates which 
                        //  are in this array and compact the remaining 
                        //  elements.

	void		unsorted()
                        // sets the status of this array to unsorted regardless
                        //  of the order of the elements.
			{ sorted = 0; }

private:
	void		pool_extract(const int);
                        // removes the array specified by the parameter from
                        //  the pool of available arrays.

};

#define	ARRAY_CLASS_DEFINITION
#endif

//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	December 1995

/***************************************************************************

  File:  pool.h
  -----

  Description:
  ------------
    This file contains the type definition for a pool class.  pool provides
    dynamic memory management for classes that have large numbers of small 
    objects.  pool is a template class that manages a set of fixed arrays
    of its argument type.  The arrays are allocated according to powers of 
    two.  A new array is allocated only when all elements of smaller arrays
    are already in use.  pool uses a bitmap to keep track of the elements
    that are in use.  As elements are used, the bits in the bitmap are set;
    as elements are returned to the pool, the bits are cleared.

    pool is a template class with one parameter.  To create a pool for a 
    given class, pool must be instantiated with the class as the template 
    parameter.  The following declaration would create a pool of Items:
      pool<Item> item_pool;

    In addition, the new and delete operators need to be overloaded for the
    specified class in the following manner:

      void*
      Item::operator new (size_t)
      {
        return item_pool.get_member();
      }

      void
      Item::operator delete (void* p)
      {
        item_pool.return_member(p);
      }
      

  Revision History:
  -----------------
    R.Robson, D.Raymond             Initial version of source code
    M.Hoeberechts  98/05/26         Added header and comments (adapted from
                                    Release Notes for Grail version 2.5 by
                                    Darrell Raymond and Roger Robson)

 ***************************************************************************/



#ifndef		POOL_CLASS_DEFINITION

#define		POOL_SIZE	128

template <class S>
class pool
{
private:

	array<S*>	p;	// array of pointers to 2^n-sized blocks
	bits		b;	// which elements are in use
	int		sz;	// next element to allocate
	int		sp;	// how sparse we are

public:
			pool() 
                        // default constructor:  initialize the pool to 
                        //  contain one array of size POOL_SIZE and have
                        //  no allocated elements
			{ p += new S[POOL_SIZE]; b.clear(); sz = sp = 0; }

			pool(const pool<S>& s):p(s.p),b(s.b),sz(s.sz),sp(s.sp)
                        // copy constructor:  initialize the pool to be a
                        //  copy of the parameter pool.
			{ }

			~pool()
                        // destructor (deallocation of memory will be handled
                        //  by array destructor)
			{ }

	pool<S>&	operator=(const pool<S>& s) 
			// assignment operator:  copy the contents of the
       			//  parameter pool to this pool.  Returns a reference
  			//  to this pool.
			{ p = s.p; b = s.b; sz = s.sz; sp = s.sp;
			  return *this; }

	int		operator==(const pool<S>&) const;
          		// test for equality:  returns true if this pool is
 			//  equal to the parameter pool (manages the same
                        //  set of arrays) and false otherwise.

	int		operator!=(const pool<S>& s) const
			// test for inequality:  returns true is this pool is
 			//  not equal to the parameter pool and false 
			//  otherwise.
			{ return *this == s ? 0 : 1; }

	int		operator<(const pool<S>&) const;
			// less than operator:  returns true if this pool is
             		//  less than the parameter pool and false otherwise.
 			//  Comparison is done by an arbitrary comparison of
			//  bits and pointer values.

	int		operator>(const pool<S>& s) const
			// greater than operator:  returns true if this pool is
             		//  greater than the parameter pool and false 
			//  otherwise.  Comparison is done by an arbitrary 
			//  comparison of bits and pointer values.
			{ if (*this == s) 
			    return 0; 
			  else if (*this < s) 
			    return 0;
			  else return 1; }

	void*		get_member();
                        // returns a pointer to a free chunk of memory which
     			//  can hold an item of type S.  This function 
			//  implements the 'new' operation.  The memory is
			//  allocated from one of the arrays in the pool if 
                        //  possible, but otherwise a new array which is twice
			//  the size of the last array is allocated.

	void		return_member(void*);
			// returns the chunk of memory pointed to by the 
			//  parameter to the pool of free memory.  This
			//  function implements the 'delete' operation.

};

#define	POOL_CLASS_DEFINITION
#endif

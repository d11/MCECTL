//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	June 1995



/***************************************************************************

  File:  classes/set/set.h
  -----

  Description:
  ------------
    This file contains the type definition for a set template
    class.  The template class has one parameter, Item, which specifies
    the type of item to store in the set.  The set will grow as
    required to allow an unlimited number of items to be added.  
    A set is a list of unique elements, and thus new elements are checked
    for uniqueness before addition.  The elements are maintained in an
    arbitrary order unless they are explicitly sorted by the user of
    the class.

    Functions are provided to add, compare and retrieve items, and to
    sort, merge, compare, intersect, clear and perform the union operation
    on sets.

    A set can be instantiated in the following way:
    set<int> integer_set;           // create a set of integers

    set is a subclass of array and inherits the following functions from
    the array template class (see classes/array/array.h):

	array<Item>&	clear()
                        // clear the contents of this set

	static	int	compare(const Item*, const Item*);
                        // returns -1 if the first item is less than the
                        //  second item, 0 if they are equal, and 1
                        //  otherwise

	int		allocated() const
                        // returns the maximum number of elements which this
                        //  set can currently hold

 	array<Item>&	remove(int i);
                        // removes the element at index i from this set.

 	void		merge(const array<Item>&);
                        // merges this set with the parameter set.  Both
                        //  sets must be sorted before calling this
                        //  procedure.

	Item&		operator[](int i) const
                        //  returns the ith element of this set.

	int		size() const
                        // returns the number of elements currently contained
                        //  in this set

	void		sort() const;
                        // sorts the elements of this set according to the
                        //  compare function for items.  qsort is used to
                        //  perform the sort.

	void		swap(array<Item>& a)
                        // swaps the contents of this set with the contents
                        //  of the parameter set.

	array<Item>&	unique();
                        // removes any duplicates which are in this set and
                        //  compacts the remaining elements.

	void		unsorted()
                        // sets the status of this set to unsorted regardless
                        //  of the order of the elements.


    Template functions for inputting and outputting sets are also 
    defined (see classes/set/ostream.src and classes/array/istream.src).
    The function headers are the following:

	template <class Item>
	ostream&
	operator<<(ostream& os, const set<Item>& s)

	template <class Item>
	istream&
	operator>>(istream& is, array<Item>& s)


    set is a friend to list.  The class specified for Item must have the 
    following operators defined:

    Operator:                 Used In Function:
    ---------                 -----------------
    <                         compare, contain, <, member, -+, (sort)
    >                         merge, +=
    ==                        compare, member, -=, unique, (sort)
    !=                        ==, <
    =                         assign, -=, +=, remove, unique
    <<                        operator<<
    >>                        operator>>


  Revision History:
  -----------------
    D.Raymond                       Initial version of source code
    M.Hoeberechts  98/05/30         Added header and comments

 ***************************************************************************/


#ifndef			SET_CLASS_DEFINITION

#include	<iostream.h>
#include	<ctype.h>
#include	<stdlib.h>

template <class Item> class array;
template <class Item> class set;
template <class Item> class list;

template <class Item>
class set : public array<Item>
{

public:

	//  constructor/destructor not necessary (array class
	//  takes care of creating and destroying the set)
       	// 		set() { ; }
     	// 		set(const set<Item>&) { ; }
	// 		~set() { }

	int		contain(const set<Item>&) const;
			// containment:  returns 1 if all the elements in the 
			//  parameter set are contained theis set, 0 otherwise

	set<Item>&	disjoint_union(const set<Item>& s)
			// fast union:  adds all the elements in the parameter
			//  set.  The new items are not checked for uniqueness,
			//  so it is the programmer's responsibility to ensure
			//  that the sets are disjoint before calling this
			//  function.  Returns a reference to this set.
#ifndef	WATCOM
			{ (array<Item> &) *this += s; return *this; }
#endif
#ifdef	WATCOM
			{ array<Item>::operator+=(s); return *this; }
#endif

	set<Item>&	disjoint_union(const Item& s)
			// fast union:  adds the paramter item to this if set.
			//  The new item is not checked for uniqueness, so it
			//  is the programmer's responsibility to ensure that
			//  the new item is not already an element of this set
			//  before calling this function.  Returns a reference
			//  to this set.
#ifndef	WATCOM
			{ (array<Item> &) *this += s; return *this; }
#endif
#ifdef	WATCOM
			{ array<Item>::operator+=(s); return *this; }
#endif

	set<Item>&	intersect(const set<Item>&, const set<Item>&);	
			// intersection: after the execution of this function,
			//  this set will contain the intersection of the
			//  parameter sets, which are unchanged.  The function
			//  returns a reference to this set.

	set<Item>&	operator=(const set<Item>& s)
			// assignment:  assigns this set to contain the same
			//  elements as the parameter set.
#ifndef	WATCOM
			{ (array<Item> &) *this = s; return *this; }
#endif
#ifdef	WATCOM
			{ array<Item>::operator=(s); return *this; }
#endif

	set<Item>&	operator=(const Item& i)
			// assigns this set to contain only the parameter
			//  element.
#ifndef	WATCOM
			{ (array<Item> &) *this = i; return *this; }
#endif
#ifdef	WATCOM
			{ array<Item>::operator=(i); return *this; }
#endif

	int		operator==(const set<Item>&) const;	
			// test for equality:  returns 1 if the parameter
			//  set contains the same elements as this set, 0
			//  otherwise.

	int		operator!=(const set<Item>& s) const
			// test for inequality:  returns 1 if the parameter
			//  set does not contain the same elements as this
			//  set, 0 otherwise.
			{ return *this == s ? 0 : 1; }

	int		operator<(const set<Item>&) const;	
			// less than:  returns 1 if this set is less than the
			//  parameter set (contains less elements or an 
			//  element in sorted order which is less than the
			//  corresponding element in the parameter set), and
			//  0 otherwise.

	int		operator>(const set<Item>& x) const
 			// greater than:  returns 1 if this set is greater than
			//  the parameter set (contains more elements or an
			//  element in sorted order which is greater than the
			//  corresponding element in the parameter set), and 
			//  0 otherwise.
			{ return x < *this ? 1 : 0; } 

 	set<Item>&	operator+=(const set<Item>&);
			// set union:  performs the union of this set and
   			//  the parameter set.  The element in the parameter
			//  set are checked for membership in this set, so
			//  uniqueness is maintained.   Returns a reference
			//  to this set.

	set<Item>&	operator+=(const Item&);
			// adds an item to this set.  The set is checked to
			//  make sure the item is not already a member.  The
			//  function returns a reference to this set.

 	set<Item>&	operator-=(const set<Item>&);
			// deletes any elements found in both this set and
			//  the parameter set from this set.  If this set was
			//  sorted, it will remain sorted if it contains less
			//  than 64 elements.  Returns a reference to this set.

	set<Item>&	operator-=(const Item&);
 			// removes the parameter element from this set if it
			//  is a member.  If this set was sorted, it will
			//  remain sorted if it contains less than 64 elements.
			//  Returns a reference to this set.

	set<Item>&	from_list(list<Item>&);
			// makes the parameter list into a set and then swaps
			//  the contents of this set with the contents of the
			//  parameter list.  Returns a reference to this set.

	int		member(const Item&) const;
			// membership:  returns the index of the parameter
			//  element in the set if it was found, -1 otherwise.

};

#define		SET_CLASS_DEFINITION
#endif



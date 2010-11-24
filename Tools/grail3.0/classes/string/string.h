//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	January 1994


/***************************************************************************

  File:  classes/string/string.h
  -----

  Description:
  ------------
    This file contains the type definition for a string template
    class.  The template class has one parameter, Item, which specifies
    the type of the characters in the string.  A string consists of an
    ordered sequence of characters.  The space to store the string is 
    dynamically allocated and thus will grow as need to allow strings of    
    arbritrary length to be represented.  Strings can be compared,
    assigned, trucated and catenated.

    It is important to note that a string in Grail is not simply char*.
    Even a variable of type string<char> is not the same as char* since it
    is not null terminated.  In order to use functions such as printf and
    strcmp with string<char>, the programmer must ensure that a null character
    is appended to the string's content.

    string has a member function, ptr() which returns a pointer to the
    first Item in the string.  This could cause many problems since the
    contents of the string can be modified without the string object 
    adjusting its size and maximum value.  ptr() should only be used if
    the returned pointer will be used with operations that do not perform
    updates to the array.

    The string comparison operators are defined so that strings will be 
    ordered first by size, then lexicographically within equal sizes.

    An example of a declaration using the string template is the following:
      string<char> char_str;      // create a string of characters

    string is a subclass of array and it inherits the following functions
    from the template class array (see classes/array/array.h):

	array<Item>&	clear()
                        // clear the contents of this string
			{ sz = 0; sorted = 1; return *this; }

 	int		contain(const array<Item>&) const;
                        // returns 1 if all the characters in the parameter
                        //  string are contained in this string, 0 otherwise

	int		is_sorted() const
                        // returns 1 if this string is sorted, 0 otherwise
			{ return sorted; }

	static	int	compare(const Item*, const Item*);
                        // returns -1 if the first item is less than the
                        //  second item, 0 if they are equal, and 1
                        //  otherwise

	int		member(const Item&) const;
                        // returns the index of the parameter item if it is
                        //  found in the string, or -1 otherwise

	int		allocated() const
                        // returns the maximum number of characters which this
                        //  string can currently hold
			{ return max; }

	array<Item>&	operator+=(const array<Item>&);
                        // string catenation: appends the contents of the 
			//  parameter string to the contents of this string. 
			//  The maximum size of this string is increased if 
			//  necessary.

 	array<Item>&	operator+=(const Item&);
                        // appends the parameter character to the string.  If
                        //  the string already contains the maximum number of 
                        //  elements, its size is doubled before the new
                        //  item is added.

 	array<Item>&	operator-=(const array<Item>&);
                        // removes any characters which are in the parameter 
			//  string and this string from this string.  After 
			//  this operation, the strings will contain disjoint
			//  sets of characters.

 	array<Item>&	operator-=(const Item&);
                        // removes one instance of the parameter character 
			//  from the string.  If the parameter item was not 
			//  in the string, the string contents are unchanged
 
 	array<Item>&	remove(int i);
                        // removes the ith character from this string.

 	void		merge(const array<Item>&);
                        // merges this string with the parameter string.  Both
                        //  strings must be sorted before calling this
                        //  procedure.

	Item&		operator[](int i) const
                        //  returns the ith character of this string.
			{ return p[i]; }

	int		size() const
                        // returns the number of characters currently contained
                        //  in this string
			{ return sz; }

	void		sort() const;
                        // sorts the characters of this string according to the
                        //  compare function for items.  qsort is used to
                        //  perform the sort.

	void		swap(array<Item>& a)
                        // swaps the contents of this string with the contents
                        //  of the parameter string.
			{ int tmp = a.max; a.max = max; max = tmp;
			  tmp = a.sorted; a.sorted = sorted; sorted = tmp;
			  tmp = a.sz; a.sz = sz; sz = tmp;
			  Item* ptr = a.p; a.p = p; p =ptr; }

	array<Item>&	unique();
                        // sort the string, then remove any duplicates which 
                        //  are in this string and compact the remaining 
                        //  elements.

	void		unsorted()
                        // sets the status of this string to unsorted 
			//  regardless of the order of the elements.
			{ sorted = 0; }


    Template functions for inputting and outputting strings are also 
    defined (see classes/string/ostream.src and classes/string/istream.src).
    The function headers are the following:

	template <class S>
	ostream&
	operator<<(ostream& os, const string<S>& s)

	template <class Item>
	istream&
	operator>>(istream& is, string<Item>& s)


    The class specified for Item must have the following operators
    defined (most are necessary because string is a subclass of array):

    Operator:                 Used In Function (in array.h):
    ---------                 ------------------------------
    <                         compare, contain, <, member, -+, (sort)
    >                         merge, +=
    ==                        compare, member, -=, unique, (sort)
    !=                        ==, <
    =                         assign, -=, +=, remove, unique
    <<                        operator<<
    >>                        operator>>



  Revision History:
  -----------------
    The Grail Project               Initial version of source code
    M.Hoeberechts  98/06/02         Added header and comments (some comments
                                    are adapted from Programmer's Guide to
                                    Grail Version 2.5 by Darrell Raymond).

 ***************************************************************************/


#ifndef		STRING_CLASS_DEFINITION

#include	<iostream.h>
#include	<string.h>
#include	<ctype.h>

const	int	STR_SIZE	= 8;

#ifdef	ATT
template <class Item> class array;
template <class Item> class string;
#endif

template <class Item>
class string : public array<Item>
{
// public variables
public:
	static char	separator;

// public functions
public:

	//  constructor/destructor not necessary (array class
	//  takes care of creating and destroying the string
	//		string() { ; }
	// 		string(const string<Item>&) { ; }
	// 		~string() { ; }

	string<Item>&	operator=(const string<Item>& s)
			// assignment:  set the contents of this string to be
			//  the same as the contents of the parameter string.
			//  If the parameter string is longer than this string,
			//  this string's length is increased to the match
			//  that of the parameter string.
			{ array<Item>::operator=(s); return *this; }

	string<Item>&	operator=(const Item& s)
			// assign this string to contain only the parameter
			//  character.
			{ array<Item>::operator=(s); return *this; }

	int		operator==(const string<Item>& s) const 
			// test for equality:  returns 1 if this string
			//  contains the same number of characters in the
			//  same order as the parameter string and 0
			//  otherwise.
			{ return stringcmp(s) == 0 ? 1 : 0; }

	int		operator!=(const string<Item>& s) const 
			// test for inequality:  returns 1 if this string
			//  contains a different number of characters or 
			//  different sequence of characters than the 
			//  parameter string and 0 otherwise.
			{ return stringcmp(s) == 0 ? 0 : 1 ; }

	int		operator<(const string<Item>& s) const;
			// less than:  returns 1 if this string is less than
			//  the parameter string, 0 otherwise.  A string is 
			//  less than another string if it contains less 
			//  characters or an equal number of characters but 
			//  comes first according to lexicographical ordering.

	int		operator>(const string<Item>& s) const
			// greater than:  returns 1 if this string is greater 
			//  than the parameter string, 0 otherwise.  A string 
			//  is greater than another string if it contains more 
			//  characters or an equal number of characters but 
			//  comes later according to lexicographical ordering.
			{ return s < *this ? 1 : 0; }

	string<Item>&	assign(const Item*, const int);
			// This function converts an array of Items to a 
 			//  string.  The first parameter is a pointer to the
			//  start of the array and the second parameter 
			//  specifies the number of items in the array. 
			//  Conversion does not change the order.  A reference
			//  to this string is returned.

	inline int	get(istream&, Item&);
			// This function attempts to get one item from the
			//  parameter input stream and store it in the 
			//  parameter Item.  Returns 1 if successful, 0
			//  otherwise.

	int		is_null() const
			// test for null (empty) string.  Returns 1 if this
			//  string is null and 0 otherwise.
			{ return sz == 0 ? 1 : 0; }

	Item*		ptr()
			// Returns a pointer to the first Item in this 
			//  string.  See remarks in header.
			{ return p; }

	string<Item>&	reverse();
			// reverse the order of the characters in this string.
			//  Returns a reference to this string.

	int		stringcmp(const string<Item>&) const;
			// string comparison:  returns -1 if this string is
  			//  less than the parameter string, 1 if this string
			//  is greater and 0 if the strings are equal.

	int		truncate(int x)
			// truncate:  removes x characters from the end of
			//  the string.  Returns the new size of this string.
			{ x > sz ? sz = 0 : sz -= x; return sz; }
};

#define		STRING_CLASS_DEFINITION
#endif

//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	December 1995


/***************************************************************************

  File:  classes/fl/fl.h
  -----

  Description:
  ------------
    This file contains the declaration for the template class fl.  fl
    describes a finite language:  that is, a language composed of a finite
    number of finite-length words.  Internally, the language is stored as
    set<string<Item> > - a set of strings which represents the enumeration of
    the language.  The input and output functions for fl assumes newlines
    are used to delimit words.

    fl includes functions to add words to the language, remove words,
    reverse the language, find the cross product, compare languages, 
    find left and rights quotients and determine membership of words in
    the language.

    The template parameter item is used to determine the type for the 
    characters which make up the words in the language.  For example,
    fl<char> might contain "abc", "a1" and "ccc", whereas fl<int> could
    contain "12 25 192", "78 1 -19" and "14".

    fl has two friend functions:
      friend ostream& operator<<(ostream&, const fl<Item>&);
      friend istream& operator>>(istream&, fl<Item>&);
    These are used for stream output and input of finite languages.  For
    more information, see classes/fl/ostream.src and classes/fl/istream.src.


    The class specified for Item must have the following operators
    defined:

    Operator:                 Used In Function:
    ---------                 -----------------
    !=                        left_quotient, right_quotient
    +=                        left_quotient, right_quotient

    Since a set<string<Item> > is declared, Item must meet all the
    requirements for the string class.  Thus the following operators
    are also required:

    Operator:                 Used In string Function:
    ---------                 ------------------------
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
    M.Hoeberechts  98/06/05         Added header and comments (adapted from
                                    Release Notes for Grail Version 2.5 by
                                    Darrell Raymond and Roger Robson).

 ***************************************************************************/


#ifndef			FL_CLASS_DEFINITION

template <class Item>
class fl
{

	friend	ostream& operator<<(ostream&, const fl<Item>&);
	friend	istream& operator>>(istream&, fl<Item>&);

private:
	set<string<Item> >	words;	// store enumeration of language
	
public:
			fl() { }
			// default constructor

			~fl() { }
			// destructor

			fl(const fl<Item>& f)
			// copy constructor: initializes this fl to contain
			//  the same words as the parameter fl.
			{ words = f.words; }

	fl<Item>&	append(const string<Item>& s);
			// appends the parameter string to every word in the
			//  language.  Returns a reference to this fl.

	void		clear() 
			// clear all existing words in this fl.  After the 
 			//  clear operation, this fl represents the empty
			//  language.
			{ words.clear(); }

	void		get_words(set<string<Item> >& s) const
			// retrieves the set of words which make up this
			//  language.  After execution, s will contain a 
			//  copy of the set of words.
			{ s = words; } 

	fl<Item>&	intersect(const fl<Item>& f1, const fl<Item>& f2)
			// sets this fl to be the intersection of the two 
			//  parameter languages.  This function sets this fl
			//  to contain only those words which are members of
			//  both f1 and f2.  Returns a reference to this fl.
			{ words.intersect(f1.words, f2.words); return *this; }

	int		member(const string<Item>& s) const 
			// determine the index of the parameter word if it is 
			//  a member of this fl.  Returns the index of the 
			//  word in the set of words if it was found, -1 
			//  otherwise.
			{ return words.member(s); }

	int		member_of_language(const string<Item>& s) const 
			// determine whether the parameter word is a member
			//  of this fl.  Returns true if it is a member,
			//  false otherwise.
			{ return words.member(s) != -1; }

	fl<Item>&	left_quotient(const string<Item>&);
			// computes the left quotient of this fl and the
			//  parameter string and stores the resulting language
			//  in this fl.  Returns a reference to this fl.

	fl<Item>&	prepend(const string<Item>& s);
			// prepends the parameter string to every word in
			//  the language.   Returns a reference to this fl. 

	fl<Item>&	reverse(); 		
			// This function reverses the language represented
 			//  by this fl.  Returns a reference to this fl.

	fl<Item>&	right_quotient(const string<Item>&);
			// computes the right quotient of this fl and the
			//  parameter string and stores the resulting language
			//  in this fl.  Returns a reference to this fl.

	int		size() const
			// returns the number of words in this fl.
			{ return words.size(); }

	fl<Item>&	operator=(const fl<Item>& l)
			// assignment:  assigns this fl to be a copy
			//  (represent the same language) of the parameter fl.
			//  Returns a reference to this fl.
			{ words = l.words; return *this; }

	fl<Item>&	operator=(const set<string<Item> >& s)
			// assigns this fl to contain the language represented
			//  by the parameter set of words.  Returns a
			//  reference to this fl.
			{ words = s; return *this; }

	fl<Item>&	operator=(const string<Item>& s)
			// assigns this fl to contain only the parameter word.
			//  Returns a reference to this fl.
			{ words = s; return *this; }

	int		operator==(const fl<Item>& l) const 
			// test for equality:  Returns 1 if this fl
			//  represents the same language as the parameter
			//  language and 0 otherwise.
			{ return words == l.words; }

	int		operator!=(const fl<Item>& l) const 
			// test for inequality:  Returns 1 if this fl
			//  represents a different language than the parameter
			//  language and 0 otherwise.
			{ return words != l.words; }

	int		operator>(const fl<Item>& x) const
			// greater than: returns 1 if this language is 
			//  greater than the parameter language (contains
			//  more words, or if when the words are sorted
			//  lexicographically, it contains a word which is 
			//  greater than the corresponding word in the 
			//  parameter language).  Otherwise returns 0.
			{ return x.words > words ? 1 : 0; }

	int		operator<(const fl<Item>& x) const
			// less than:  returns 1 if this language is less than
			//  the parameter language (contains less words, or
			//  if when the words are sorted lexicographically, it
			//  contains a word which is less than the 
			//  corresponding word in the parameter language).
			//  Otherwise returns 0.
			{ return x.words < words ? 1 : 0; }

	fl<Item>&	operator*=(const fl<Item>& l);
			// computes the cross product of this language and the
			//  parameter language and stores the result in this
			//  fl.  Returns a reference to this fl.

	fl<Item>&	operator+=(fl<Item>& l) 
			// adds all the words in the parameter language to
			//  this language.  Returns a reference to this fl.
			{ words += l.words; return *this; }

	fl<Item>&	operator+=(string<Item> str) 
			// adds the parameter string to the language.  Returns
			//  a reference to this fl.
			{ words += str; return *this; }

	fl<Item>&	operator-=(const fl<Item>& l) 
			// difference: remove any words which are members
			//  of this language and the parameter language from
			//  this language.  After execution the set of words
			//  in the languages will be disjoint.  Returns
			//  a reference to this language.
			{ words -= l.words; return *this; }

	fl<Item>&	operator-=(const string<Item>& s)
			// removes the parameter word from this language if
			//  it is found.  If the parameter word is not found,
			//  no change is made.  Returns a referenct to this
			//  fl. 
			{ words -= s; return *this; }
};

#define		FL_CLASS_DEFINITION
#endif

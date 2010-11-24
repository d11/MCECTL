/******************************************************************************************
 File:  classes/afa/dnf/function.h
 -----
 Description: 
 ------------ 
        This file contains the type definition of the "funct" class.  This class implements
        Boolean functions in disjunctive normal form.  Terms are implemented by the "term"
        class declared in classes/afa/dnf/term.h.  

	The i/o functions for terms are defined in /classes/afa/dnf/functios.src.
	The headers are:

	istream& operator>>(istream& in, funct& f)
	ostream& operator<<(ostream& out, const funct& f)
	void funct::print(ostream& out) const

 Revision History:
 -----------------
 X. Wu                   Initial version of source code
 S. Huerter 08/11/98     simplify2(), header and comments 
 
******************************************************************************************/
#include "term.h"
#include <iostream.h>
#ifndef _FUNCT
#define _FUNCT
/******************************************************************************************/
class funct{

   friend istream& operator>>(istream&, funct&);
   friend ostream& operator<<(ostream&, const funct&);

   private:

                 term* head;

   public:

	         funct()  
	         // default constructor
	         { head = 0;}

                 funct(const funct& f);
                 // copy constructor; copies f to this function.

	         funct(const term& t);
                 // copy constructor; assigns this function to be term t.

	         ~funct();
                 // destructor

          void   print(ostream&) const;
                 // writes the function to the output stream symbolically, 
                 // eg. q1q2~q3q4~q5~q6+q1~q2~q3q4~q5q6+~q1q2q3~q4~q5q6
                 // (term variables are written as qi to denote afa states)
                 // terms are separted by "+", which denotes OR.

	  void   set_head (term* h)
                 // assigns h to this head member.
                 {head = h;}

          term*  get_head () const 
                 // returns this head member.
                 {return head;}

          int    numterms();
                 // returns the number of terms this function contains.

          void   append   (const term& t);
                 //if t is not contained in this funct,
                 //this funct becomes (this OR t);
  
          void   append   (const funct& f);
                 //for the terms t in f which are not contained in this funct,
                 //this funct becomes (this OR t)

	  void   addtoend (const term& t);
                 //this funct becomes (this OR t);

	  term*  search   (const term& t) const;
                 // returns the address of t if t is on this funct, or 0
                 // if term t is not found on this funct

          void   remove   (const term& t); 
                 // removes term t from this funct.
    
	  void   remove   (term* tp); 
                 // removes the term pointed to by tp from this funct.
          
          funct& operator=(const funct& f); 
                 //copies function f to this funct.

          int    evaluate (unsigned n) const;
                 // computes the Boolean value of this funct on truth assignment n,
                 // ie. f(n).

	  void   simplify();
                 // simplifies the expression of the function. 

	  void   simplify2(); 
                 // same method as above, but MUCH (ie.95%) faster.
	  
	  void   leftshift(int k); 
                 // leftshift the bit vectors alpha and beta in each term of this funct
                 // by k bits
         
	  funct  and(const term& t)const;
                 // returns (*this AND t)
  
	  funct  and(const funct& f)const; 
                 // returns (*this AND f)
 
	  funct  not()const;
                 // returns the negation of this funct.

	  funct  substitute(int* X, funct* F, int n) const;
                 // returns the funct resulting from substituting the function
                 // F[X[i-1]] for the ith variable in this function, i=1..n. 
	};
#endif



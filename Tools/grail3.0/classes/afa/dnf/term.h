/******************************************************************************
 
  File:  classes/afa/dnf/term.h
  -----

  Description: 
  ------------ 
    This file contains the type definition for the "term" class.
    We are representing a Boolean term using two unsigned integers, read
    as bit-vectors, according to Theorem 5 in X. Wu's paper "Efficient 
    Implementation of Regular Languages using r-afa".  This theorem states:
 
        For every Boolean function f (defined on a set S of n Boolean 
        variables) that can be expressed as a single term, there exist two 
        n-bit vectors a and b s.t.
        f(u)=1 iff (a&u)^b = 0 , for all u in B**n.  
	Here, & and ^ are the bitwise AND and OR operators, respectively.

   In other words, vector a flags which variables of the n possible variables 
   the term includes, and vector b flags which of the variables flagged by a 
   are non-negated.  A term pointer is included as a member of the term 
   construct so that terms may be linked together to form a Boolean function 
   in disjunctive normal form.

   The i/o functions for terms are defined in /classes/afa/dnf/functios.src  
   The headers are:

      istream& operator>>(istream& in, term& t)
      ostream& operator<<(ostream& out, const term& t)
      void term::print(ostream& out)const


 Revision History:
 -----------------
 X. Wu                   Initial version of source code
 S. Huerter 08/11/98     Faster "combine(.,.)", header and comments 
 
******************************************************************************/
#ifndef _TERM
#define _TERM
#include<math.h>
#include <iostream.h>


class term{

   friend istream& operator>>(istream&, term&);
   friend ostream& operator<<(ostream&, const term&);

   private:
          int      dimension; //how wide is the term, 0<=dim<=32, ie. max number of var.
          unsigned alpha;     //what variables of the bit vector are in the term
          unsigned beta;      //of the included variables, which ones are non-negated
	  term*    next;      //link to next term
   public:
	            term()
	            // default constructor  
		    {next = 0;}                                  
                           
                    term(unsigned a, unsigned b)
                    // assignment constructor
                    {alpha = a; beta = b;next =0;}

	            term(int d, unsigned a, unsigned b) 
                    // assignment constructor using the dim member
	            {dimension =d; alpha = a; beta = b;next =0;}

                    term(const term& t)                                
                    // copy constructor
                    {dimension=t.dimension;alpha =t.alpha; beta = t.beta;next = 0;}

	            ~term()
                    // destructor 
                    {next = 0; }                               
          
          void      print(ostream&) const;
                    // writes this term to the parameter output stream in symbolic form, 
                    // eg. q1~q2~q3q4~q5q6
                    // (term variables are denoted by qi to denote afa states) 

          void      set_alpha(unsigned a)
	            // assign private member alpha
                    {alpha = a;}

          void      set_beta(unsigned b)
	            // assign private member beta
                    {beta = b;}

          void      set_dim(int d)
                    // assign private member dimension
                    {dimension = d;}

	  void      set_next  (term* p)
                    // assign private member next
                    {next = p;}

          unsigned  get_alpha( ) const
                    // retrieve private member alpha
                    {return alpha;}

          unsigned  get_beta( ) const
                    // returns private member beta
                    {return beta;}

          int       get_dim( ) const 
                    // returns private member dimension
                    {return dimension;}

	  term*     get_next( ) const 
                    // returns private member next
                    {return next;}

          term&     operator=(const term& t)
	            // copies term t to this term
                    {
               	      if (this !=&t)
	                {dimension=t.dimension; alpha = t.alpha; beta = t.beta; next = 0;}
	              return *this;
                    }

          int       operator == (const term& t)const
	            // returns 1 if this term equals term t, 
                    // returns 0 otherwise.
                    { return (alpha == t.alpha)&&(beta == t.beta);}

          int       operator != (const term& t) const
                    // returns 0 if this term equals term t,
                    // returns 0 otherwise.
                    { return *this == t?0:1;}
	  
          int       evaluate(unsigned vector) const
                    // returns Boolean value of this term under truth assignment "vector"
                    {
                      return ((alpha & vector)^ beta) == 0?1:0;
	            }

	  void      leftshift(int k)  
                    // shifts alpha and beta of this term k bits to the left
	            {
	              alpha = (alpha<<k);
	              beta  = (beta << k);
	            }

          int       combine(const term& t);
                    // returns 1 if (this term OR term t) can be simplified;
                    // the simplified term is stored in this term;
                    // returns 0 otherwise.

          int       combine(const term& t, unsigned *ord_inv);
                    // exactly the same as the above function, except
                    // MUCH faster; ord_inv is the array of unsigned
                    // described in classes/afa/manage_bitvec.src
	  
 };

#endif
                  
              

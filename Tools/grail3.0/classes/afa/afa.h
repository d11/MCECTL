/******************************************************************************************
 File:  classes/afa/afa.h
 -----
 Description: 
 ------------ 
 This file contains the type definition for the alternating finite automata template class (h-afa).  
 In keeping with the Grail implementation of other finite automata, the set of input symbols 
 for afa is parameterized (see classes/array/array.h).    

 The default constructor builds an afa which accepts nothing (no states, empty transition table, and the 
 head function is set to "false").  A copy constructor and a constructor which builds an
 h-afa accepting the same language as a given dfa are also provided.     
 
 See the /classes/afa/dnf directory for descriptions of the Boolean functions making up the head
 function and transition table.

 See the i/o template functions defined in /classes/afa/afaios.src.  The headers are:

      template <class T>
      istream& 
      operator>>(istream& in, afa<T>& autom)

      template <class T>
      ostream&
      operator<<(ostream& out, const afa<T>& autom)


 *****************************************************************************************/
/*****************************************************************************************
 Revision History:
 -----------------
 X. Wu                   Initial version of source code
 S. Huerter 08/11/98     Header and comments 
 ******************************************************************************************/

#ifndef _AFA
#define _AFA

/*
#include "dnf/term.h"
#include "dnf/function.h"
#include <fstream.h>
#include "../bits/include.h"
#include "../array/include.h"
#include "../set/include.h"
#include "../list/include.h"
#include "../state/include.h"
#include "../string/include.h"
#include "../inst/include.h"
#include "../fm/include.h"
*/

#include <iostream.h>

template <class T>
class afa {

           friend istream& operator>>(istream&, afa<T>&);

           friend ostream& operator<<(ostream&, const afa<T>&);

    private:

           array<T>      symbol;  // ordered array of input symbols; 
	   unsigned      states;  // number of states in the afa;
	   unsigned      final;   // the characteristic bit vector (final states);
	   funct         head;    // the head function; 
	   funct*        table;   // the transition function;

           afa<T>        same_input_union(const afa<T>& a)const;
                         // Assuming "this" afa and afa a have the same set of input symbols, returns
			 // an h-afa accepting the union of the languages accepted by "this" and a;  
                         // Result is valid (under our bit-vector implementation of afa) only if
                         // the cardinality of the union of the two state sets does not exceed 
			 // 32 -- no error messages are provided;
                         // Called by the public "afaunion" function (below);

           afa<T>        expand(const array<T>& )const;
                         // Expands the set of input symbols of "this" afa without altering
                         // the language it accepts; 
	                 // The given set of symbols (which must properly include "this" input-symbol set)
                         // forms the new set of input symbols;
	                 // Result is valid (under our bit-vector implementation of afa) only if originally
                         // the number of states of "this" afa does not exceed 31 -- no error messages are provided;
                         // Called by "afacat()" and "afaunion()" (below);
			  
			   
           afa<T>        same_input_cat(const afa<T>& a) const;
                         // Assuming "this" afa and afa a have the same set of input symbols, returns
			 // an h-afa accepting the concatenation of the languages accepted by "this" and a;
	                 // An error message is returned if this.number_of_states() + 
			 // (the number of bit-vectors reachable from a.final under the transition function of a)
	  		 // exceeds 32;
                         // Called by the public "afacat" function (below);
                         

           unsigned      transition(unsigned n, int col) const 
	                 // Given a bit-vector n and an input symbol which
			 // is the (col+1)-st element in the symbol array of
			 // "this" afa,  calculate the bit-vector returned by
			 // this afa transition;
			 {
			    int num= symbol.size();
			    unsigned vector=0;
			    for (int index = 0; index<states; index++)
				if (table[index*num +col].evaluate(n) ==1)
				   vector += (unsigned) pow(2, index);
			    return vector;
			 }
    
    public:
                         afa();                 
                         // default constructor
   
			 afa(const afa<T>& a);  
                         // copy constructor
			   
			 afa(const fm<T>& d);   
                         //convert a dfa to an h-afa

			 ~afa();                
                         // destructor

           int           number_of_states()const
	                 // returns the number of states of the afa
	                 {return states;}

           array<T>      get_symbol()const
	                 // returns the set of input symbols
			 {return symbol;}

           void          print(ostream&) const;
                         // prints "this" afa in its token form to the terminal

	   fm<T>         afatofm() const;
                         // Returns a dfa which accepts the same language as "this" afa. 

	   void          fmtoafa(const fm<T>& d);
                         // Rebuilds "this" afa to accept the same language as dfa d; 
			 // Requirements on the input dfa: (i)the size of the dfa state set
                         // must not exceed pow(2,31); (ii)the dfa states must be numbered contiguously
			 // starting at 0 (ie. 0,1,2,..without skipping any integers)
	                 // If any of these requirements are not met, result is invalid -- no error
			 // messages are provided;

	   int           accept(string<T>& input)const;
                         // Returns nonzero value if "input" is accepted;
                         // Zero means "input" is not accepted.

	   afa<T>        afaunion(const afa<T>& a)const; 
                         // Builds an afa which accepts both the language accepted by "this" afa and
                         // the language accepted by afa a;
                         // Result is valid (under our bit-vector implementation of afa) only if
                         // the cardinality of the union of the two state sets does not exceed 
			 // 32 -- no error messages are provided;         
                         
	   afa<T>        complement() const; 
                         // Returns the afa accepting the complement of the language accepted by 
                         // "this" afa;
 
	   afa<T>        intersection(const afa<T>& a)const; 
                         // Returns the afa accepting the intersection of the language accepted
                         // by "this" afa and the language accepted by afa a;
                         // Result is valid (under our bit-vector implementation of afa) only if
                         // the cardinality of the union of the two state sets does not exceed 
			 // 32 -- no error messages are provided; 

	   afa<T>&       operator=(const afa<T>& a); 
                         // Copies afa a to "this" afa;

	   afa<T>        reverse() const; 
                         // Returns an afa accepting the reverse of the language accepted by
                         // "this" afa;
	                 // If the number of bit-vectors reachable from the characteristic vector under
			 // the transition function exceeds 32, the reverse machine cannot be represented
			 // in our bit-vector implementation, and an error message is returned 
                         // instead;
	                                                 
	   afa<T>        opstar() const;
                         // Returns an afa accepting the star of the language accepted by "this" afa;
                         // If the number of bit-vectors reachable from the characteristic vector under
			 // the transition function exceeds 32, the star machine cannot be represented
			 // in our bit-vector implementation, and an error message is returned 
                         // instead;  
	                                                 
	   afa<T>        afacat(const afa<T>& a) const;
                         // Returns an afa accepting the concatenation of the language accepted by 
                         // "this" afa with the language accepted by afa a;
                         // If this.number_of_states() + (the number of bit-vectors reachable
			 // from a.final under the transition function of a) exceeds		
			 // 32, the new machine cannot be represented in our bit-vector format,
			 // and an error message is returned instead;
	                                               
	 };

#endif



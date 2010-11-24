//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	December 1993

/***************************************************************************

  File:  classes/inst/inst.h
  -----

  Description:
  ------------
    This file contains the declaration of the inst template class.  inst 
    represents an instruction on an arc in a finite automaton.  There
    is one template parameter, Label which specifies the type for the 
    Label on the arc.

    Suppose the following transition must be represented:

            ___                  ___
           /   \       a        /   \       Change from state 1 to state
           | 1 |--------------> | 2 |       2 on input a.
           \___/                \___/

    
    A possible declaration using the inst template is the following:
      inst<char> instruction;       // instruction with character label

    The inst class has three private variables, sink, source and label. In
    this case, sink would be state 2, source would be state 1 and label 
    would be 'a'.

    There are three types of instructions.  The first represents a
    transition from one state to another (as shown above).  The second
    identifies a state as a start state.  For example if state 1 is a
    start state then there would be an instruction with START as the source,
    and state 1 as the sink (and an arbitrary label).  The third type of
    instruction identifies a final state.  In this case, the source is the
    state and the sink is FINAL (again with an arbitrary label).  States can 
    be designated as either START or FINAL for this purpose (see 
    classes/state/state.h).  A state designated START or FINAL is not a 
    state in the normal sense (there are no transitions to it - it is not
    a member of the set of states in any finite automaton), but it is used to 
    identify other states as start states or final states.

    There are member functions to get and set the sink, source and label,
    compare instructions, copy an instruction, reverse the direction and
    determine and set state the status of states as final or start states.

    inst has one friend function:
      friend istream& operator>>(istream&,inst<Label>&);    // stream input

    There is a template function outputting an instruction (see 
    classes/inst/ostream.src).  The function header is the following:

	template <class Label>
	ostream&
	operator<<(ostream& os, inst<Label>& t)


    The class specified for Item must have the following operators
    defined:

    Operator:                 Used In Function:
    ---------                 -----------------
    =                         operator=, assign
    !=                        operator==, labelis
    <                         operator<
    >                         operator<
    <<                        operator<<
    >>                        operator>>



  Revision History:
  -----------------
    The Grail Project               Initial version of source code
    M.Hoeberechts  98/06/03         Added header and comments

 ***************************************************************************/


#ifndef		INST_CLASS_DEFINITION

#include	<strstream.h>


template <class Label>
class inst
{

        // friend functions
        friend  istream&
        operator>>(istream&,inst<Label>&);              // stream input

private:

	state		source;				// source state
	state		sink;				// sink state
	Label		label;				// instruction label

public:
// public variables
	static	char	left_delimiter;			// used for output
	static	char	right_delimiter;		// used for output

// public functions
public:
			inst() { ; }
			// default constructor

			// inst(const state& s1,const Label& r,const state& s2)
			// { source = s1; label = r; sink = s2; }

			inst(const inst<Label>& t) :
			source(t.source), label(t.label), sink(t.sink) { }
			// copy constructor:  initializes this instruction
			//  to be the same as the parameter instruction

			// ~inst() { ; }	 		

	inst<Label>&	operator=(const inst<Label>& t)
			// assignment:  assigns this instruction to have the
			//  same values as the parameter instruction.  The
			//  function returns a reference to this instruction.
			{ if (this != &t) { source = t.source; 
			  label = t.label; sink = t.sink; } return *this; }

	int		operator==(const inst<Label>& s) const
			// test for equality:  returns 1 if this instruction
			//  has the same sink, source and label values as the
			//  parameter instruction, 0 otherwise.
			{ return source != s.source || sink != s.sink 
				|| label != s.label ? 0 : 1; }

	int		operator!=(const inst<Label>& t) const
			// test for inequality:  returns 1 if this instruction
			//  has a different sink, source or label than the
			//  parameter instruction and 0 otherwise.
			{ return *this == t ? 0 : 1; }

	int		operator<(const inst<Label>&) const;
                        // less than:  returns 1 if this instruction is less
                        //   than the parameter instruction, 0 otherwise.

	int		operator>(const inst<Label>& t) const
                        // greater than: returns 1 if this instruction is
                        //  greater than the parameter instruction, 0 otherwise
			{ return *this < t ? 0 : 1; }

	inst<Label>&	assign(const state& s1,const Label& r, const state& s2)
			// assigns this instruction to have the source state
			//  s1, label r and sink state s2.  Returns a
			//  reference to this instruction.
			{ source = s1; label = r; sink = s2; return *this; }

	Label&		get_label() 
			// returns the label on this instruction
			{ return label; }

	state&		get_sink() 
			// returns the sink state for this instruction
			{ return sink; }

	state&		get_source()
			// returns the source state for this instruction
			{ return source; }

	int		is_start() const
			// returns 1 if the source state is a start state
			//  and 0 otherwise.
			{ return source.is_start() ? 1 : 0; }

	int		is_final() const
			// returns 1 if the sink state is a final state and
			// 0 otherwise
			{ return sink.is_final() ? 1 : 0; }

	int		is_null() const
			// returns 1 if the source state is null and 0 
			//  otherwise
			{ return source.is_null() ? 1 : 0; }

	int		labelis(const Label& r) const
			// returns 1 if the source state is not a start state,
			//  the sink state is not a final state and the label
			//  is the same as the parameter label, 0 otherwise.
			{ return is_start() || 
                                 is_final() || label != r ? 0 : 1; } 

	void		make_final(const state& s)
			// make this instruction designate s as a final state.
			//  Sets the source to s, sink to FINAL and leaves
			//  an arbitrary label.
			{ source = s; sink.final(); }

	void		make_start(const state& s)
			// make this instruction designate s as a start state.
			//  Sets the source to START, sink to s and leaves
			//  an arbitrary label.
			{ sink = s; source.start(); }

	void		renumber(int bottom)
			{
				if (!source.is_start()) source += bottom;
				if (!sink.is_final()) sink += bottom;
			}

	void		reverse()
			// reverse the direction of this instruction.  The
			//  source and sink are switched, but label remains
			//  the same.  This function should not be called
			//  with instructions which contain START or FINAL
			//  states (no checking is done).
			{ state st = source; source = sink; sink = st; }

	void		null()
			// make this represent a transition from a NULL state.
			//  Sets source to NULL and leaves sink and label.
			{ source.null(); }

	int		sinkis(const state& s) const
			// returns 1 if the sink state is the same as the
			//  parameter state, 0 otherwise.
			{ return sink == s ? 1 : 0; }

	int		sourceis(const state& s) const
			// returns 1 if the source state is the same as the
			//  parameter state, 0 otherwise.
			{ return source == s ? 1 : 0; }
};

#define		INST_CLASS_DEFINITION
#endif

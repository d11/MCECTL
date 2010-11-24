//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	December 1993


/***************************************************************************

  File:  classes/state/state.h
  -----

  Description:
  ------------
    This file contains the type definition for a state class.  Variables of
    type state can represents states in finite state machines (see
    classes/fm/fm.h).  There are three types of states in addition to a
    null state.  Here is the purpose of each type:

      null state:    A state which is not in use can be designated as a null
                     state by calling the member function null().  A null
                     state is a placeholder for a state; it is not a part of
                     a finite state machine but is used internally by Grail.

      start state:   A state can be designated a start state by calling the
                     member function start().  Such a state is not itself a
                     member of a finite state machine, but is used internally
                     by Grail to designate another state as a start state.
                     An instruction which has a start state as the source and
                     state x as the sink indicates that x is a start state.
                     (see classes/inst/inst.h).

      final state:   A state can be designated a final state by calling the
                     member function final().  A final state is not itself a
                     member of a finte state machine, but is used internally
                     by Grail to designate another state as a final state.
                     An instruction which has state y as the source state and
                     a final state as the sink state indicates that y is a
                     final state.  (see classes/inst/inst.h).

      normal state:  A state which not one of the above types is a normal
                     state.  These states directly represent the states in 
                     a finite state machine.  Each normal state can be 
                     identified by an integer which can be set using the
                     member functions state(const int&) or operator= and
                     retrieved using value().


    Internally, a state is represented as an integer.  NULL, FINAL and START
    have set values which identify the type of the state.  Otherwise, the
    state's number is stored.

    A state can be declared in the following ways:
     state new_state;               // initially a null state
     state new_state(old_state);    // new state is a copy of old_state
     state new_state(4);            // create a new state with value 4

    Functions for inputting and outputting normal states are also defined
    (they do not work for final, start or null states). (see 
    classes/state/ostream.src and classes/state/istream.src). The function 
    headers are the following:

	ostream& operator<<(ostream& os, const state& s)
	istream& operator>>(istream& is, state& s)


  Revision History:
  -----------------
    The Grail Project               Initial version of source code
    M.Hoeberechts  98/06/04         Added header and comments

 ***************************************************************************/

#ifndef			STATE_CLASS_DEFINITION
#include	<iostream.h>

class state
{
private:
	// enumerate state types
	enum { NULL_STATE = -3, START = 0, FINAL = 1, EITHER = 2 };

	int		number;				// state number

// public functions
public:

			state():number(NULL_STATE) { }
			// default constructor:  initializes this state to be
			//  a null state.

			state(const state& s):number(s.number) { }
			// copy constructor:  initializes this state to be the
			//  same type and have the same state number as the
			//  parameter state.
			
			state(const int& i):number(i + 2) { }
			// constructor:  initializes this state to be a normal
			//  state with state number i.

	int		is_null() const
			// returns 1 if this state is null, 0 otherwise
			{ return number == NULL_STATE ? 1 : 0; }

	int		is_final() const
			// returns 1 if this state is final, 0 otherwise
			{ return number == FINAL ? 1 : 0; } 

	int		is_start() const
			// returns 1 if this state is a start state, 0 
			//  otherwise
			{ return number == START ? 1 : 0; } 

	state&		operator=(const state& s)
			// assignment operator:  assigns this state to be a
			//  copy of the parameter state (it will have the
			//  same type and state number).   Returns a
			//  reference to this state.
			{ number = s.number; return *this; }

	state&		operator=(const int& i)
			// assignment operator:  assigns this state to be a
			//  normal state with the state number i.  Returns
			//  a reference to this state.
			{ number = i + 2; return *this; }

	int		operator==(const state& s) const
			// test for equality:  returns 1 if this state is 
			//  equal to the parameter state (has the same type
			//  and state number) and 0 otherwise.
			{ return number == s.number ? 1 : 0; } 

	int		operator!=(const state& s) const
			// test for inequality:  returns 1 if this state is
			//  not equal to the parameter state (has a different
			//  type or state number) and 0 otherwise.
			{ return number != s.number ? 1 : 0; } 

	void		operator+=(const state& s)
			// number increment:  increments the state number for
			//  this state by the state number of the parameter
			//  state.
			{ number += s.number; }

	void		operator+=(int i)	
			// number increment:  increments the state number for
			//  this state by the value of the parameter.
			{ number += i; }

	void		operator-=(const state& s);
			// decrements this state's number by the value of the
			//  parameter state's number.  If the parameter state
			//  has a larger number than this state, an error
			//  message is printed on the standard error stream and
			//  no change is made.

	void		operator-=(int i);
			// decrement the state's number by the parameter value.
			//  If the parameter value is larger than this state's
			//  number, an error message is printed on the 
			//  standard error stream and no change is made.  (This
			//  function should only be called for normal states).

	int		operator<(const state& s) const
			// less than:  returns 1 if this state's number is 
			//  less than the parameter state's number and 0
			//  otherwise.
			{ return number < s.number ? 1 : 0; }

	int		operator<(int i) const
			// less than:  returns 1 if this state's number is less
			//  than the parameter value and 0 otherwise. (This
			//  function should only be called for normal states).
			{ return number < (i + 2) ? 1 : 0; }

	int		operator>(const state& s) const
			// greater than:  returns 1 if this state's number is
			//  greater than the parameter state's number and 0
			//  otherwise.
			{ return number > s.number ? 1 : 0; }

	int		operator>(int i) const
			// greater than:  returns 1 if this state's number is
			//  greater than the parameter value and 0 otherwise.
			//  (This function should only be called for normal
			//  states).
			{ return number > (i + 2) ? 1 : 0; }

	void		null()
			// make this state a null state
			{ number = NULL_STATE; }

	void		final()
			// make this state a final state
			{ number = FINAL; }

	void		start()
			// make this state a start state
			{ number = START; }

	int		value() const
			// return the value (state number) of this state.
			{ return number-2; }
};

#define STATE_CLASS_DEFINITION
#endif


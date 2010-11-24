//	This code copyright (c) by the Grail project.
//	No commercial use permitted without written consent. 
//	January 1994


/***************************************************************************

  File:  classes/inst/std.h
  -----

  Description:
  ------------
    This file contains code which initializes the public members
    inst<Label>::left_delimiter and inst<Label>::right_delimiter.  These
    delimiters are used when inputting and outputting instructions.
    They are both set to ' ' by this file.

  Revision History:
  -----------------
    The Grail Project               Initial version of source code
    M.Hoeberechts  98/06/04         Added header and comments


 ***************************************************************************/


//	user-defined delimiters

template <class T>
char	inst<T>::left_delimiter = ' ';

template <class T>
char	inst<T>::right_delimiter = ' ';

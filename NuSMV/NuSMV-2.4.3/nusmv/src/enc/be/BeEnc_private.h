
/**CHeaderFile*****************************************************************

  FileName    [eEnc_private.h]

  PackageName [enc.be]

  Synopsis    [Private and protected interface of class 'BeEnc']

  Description [This file can be included only by derived and friend classes]

  SeeAlso     [BeEnc.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``enc.be'' package of NuSMV version 2. 
  Copyright (C) 2004 by ITC-irst. 

  NuSMV version 2 is free software; you can redistribute it and/or 
  modify it under the terms of the GNU Lesser General Public 
  License as published by the Free Software Foundation; either 
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information on NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

  Revision    [$Id: BeEnc_private.h,v 1.1.2.8.6.1 2007/04/20 13:05:54 nusmv Exp $]

******************************************************************************/


#ifndef __BE_ENC_PRIVATE_H__
#define __BE_ENC_PRIVATE_H__


#include "BeEnc.h" 
#include "enc/base/BoolEncClient.h"
#include "enc/base/BoolEncClient_private.h" 

#include "be/be.h"
#include "utils/NodeList.h"
#include "utils/assoc.h"
#include "utils/utils.h" 


/**Struct**********************************************************************

  Synopsis    [BeEnc class definition derived from class BoolEncClient. 
  This is the variables encoder for BMC. This class implements the
  boolean encoding for BMC, by providing variables groups and
  time-related functions. If the Boolean Expression (BE) layer
  provides actual access to variables, such as creation, shifting,
  substitution, expressions, etc., the BeEnc class provides the
  right semantics for those variables.]

  Description [The idea is to provide:
  1. A structured layer for variables (encoding)
  2. A set of functionalities to manage the variables. 

  The encoding is internally organized to provide efficient operations
  on generic BEs, like variables shifting and substitutions, and
  conversions between symbolic variables to BE variables, BE indices,
  etc.

  The class BeEnc keeps and manages a set of encoded input and state
  variables that are committed as bunches within SymbLayers. 
  The encoder keeps the relation between:
  - The variables occurring within the committed layers and the untimed 
    variables in terms of BE variables. 
  - The untimed variables and the corresponding timed variables
    instantitated at determinated times. 

  The management of the untimed and timed variables is allowed by the
  use of two separated levels, called the Physical Level and the
  Logical Level.


  * Physical Level 

    This level holds the actual BE variables, that are instantitated
    to represent both untimed and timed variables. The order the
    variables are organized must be considered as random and not
    directly controllable from outside the class BeEnc, but it is the
    physical level that will be seen when the indices of BE variables
    are manipulated. Since there is not visible structure which BE
    variables wihin the physical level are organized with, the user
    cannot assume anything about the indices. For this reason, special
    iterators are provided to iterate over a set of variables (for
    example, the set of state untimed vars).

  * Logical Level
   
    This level provides the necessary structure to the physical level,
    and it is visible only from within the class BeEnc. Every
    operation that operates on BE variables will pass through the
    physical level at first, and then through the logical level. The
    class BeEnc provides the necessary support to pass from one level
    to the other, but it is necessary to remember that the logical
    level is NOT visible and accessible from outside the class.

  The logical level is splitted into two distinct parts: 
  1.1 Untimed variables block.
  1.2 Timed variables block.

  1.1 Untimed variables block 

      Keeps the set of untimed (logical) BE variables, such that any
      (untimed) expression in term of BE will contain references to
      variables located in this logical block. This block is splitted
      into three sub-blocks, that group respectively untimed current
      state variables, input variables, and next state variables.

                 ---- -- ----      * : current state variable
                |****|==|####|     = : input variable
                 ---- -- ----      # : next state variable
                 0123 45 6789      n : BE variable logical index
                 1435 28 7690      n : BE variable physical index

      The picture shows an example of a logical untimed variables
      block. The model contains 6 boolean variables (logical indices
      from 0..5), where logical indices 0..3 refer the state
      variables, and logical indices 4 and 5 refer the input
      variables. The set of state variables is than replicated to
      represent the next state variables, referred by logical indices
      6..9 that constitutes the third sub-block in the picture.

      Notice that logical indices 0..9 refer a boolean variable
      allocated by the BE layer, that does not distinguish between
      state, input, or next variables.  
      
      Also notice that BE variables are physically allocated at
      indices that can be completely different from the corresponding
      indices within the logical level. The class BeEnc keeps the
      relation between the logical and the physical level, and
      viceversa.

           state        input       next         UNTIMED BLOCK
        |-----------|   |---|   |-----------|
        0   1   2   3   4   5   6   7   8   9    LOGICAL INDEX
       _______________________________________
      | 1 | 4 | 3 | 5 | 2 | 8 | 7 | 6 | 9 | 0 |  LOG->PHY LEVEL MAP
       ---------------------------------------
      | 9 | 0 | 4 | 2 | 1 | 3 | 7 | 6 | 5 | 8 |  PHY->LOG LEVEL MAP
       ---------------------------------------


  1.2 Timed variables block
      
      Following the untimed logical variables block, the timed logical
      variables block holds the set of state and input variables that
      are instantiated at a given time. A BE expression instantiated
      at time t, will contain BE logical variables that belong to this
      block.

      Timed vars block is logically splitted into separate frames,
      each of one corresponding to a given time t.  The structure of
      each frame depends on the specific time t and on the number of
      transisitions the model has. When the problem length k is 0,
      only frame from time 0 is allocated, and this frame is
      constituted by only state variables. In this condition the
      encoding structure (with untimed and timed blocks) would be
      this:

                    Untimed block          Timed block
        |-------------------------------| |-----------|
           current    input      next        state 0
         -- -- -- --  -- --  -- -- -- --   -- -- -- --
        |  |  |  |  ||  |  ||  |  |  |  | |  |  |  |  | 
         -- -- -- --  -- --  -- -- -- --   -- -- -- --
         00 01 02 03  04 05  06 07 08 09   10 11 12 13
      
      In the example BE logical variables 10..13 are allocated to keep
      current state variables at the initial state (time 0). Since
      there are no transitions, input variables are not allocated for
      this value of problem length k.

      When k=1, the encoding becomes:

                    Untimed block                     Timed block
        |-------------------------------| |-------------------------------|
           current    input      next        state 0   input 0   state 1
         -- -- -- --  -- --  -- -- -- --   -- -- -- --  -- --  -- -- -- --
        |  |  |  |  ||  |  ||  |  |  |  | |  |  |  |  ||  |  ||  |  |  |  |
         -- -- -- --  -- --  -- -- -- --   -- -- -- --  -- --  -- -- -- --
         00 01 02 03  04 05  06 07 08 09   10 11 12 13  14 15  16 17 18 19

      Here 00..19 are of course logical indices, and the corresponding
      physical indices are not shown.
 
      When k>0 the timed block is a sequence of state (input state)+
      timed sub-blocks. In the example above BE logical variables 14
      and 15 have been added to encode input variables at time 0
      (since there is a transition leading to time 1), and logical
      variables 16..19 have been added to encode state variables at
      time 1.
      
      When a generic, untimed BE expressions E is instantiated at time
      t (i.e. is shifted to time t), each current state variable
      occurring in E will be replaced by the corresponding BE logical
      variable in the timed subblock t, as well any occurring input
      variable. Each next state variable will be replaced by the
      corresponding state variable located in the timed subblock
      (t+1).
      
      It is important to notice that the problem length k must be
      strictly taken into account when shifting operations are
      performed. In particular, as there are not transitions outgoing
      the last state at time t=k, neither inputs nor next states can
      be shifted at time t=k.
      
      From the logics point of view, when t=k inputs variables at time
      t do not exist at all (and the encoding directly maps this
      idea).  Because of this, the logic value of input variables at
      time t=k is false.
      
      See the class interface for further details on the provided
      features.]

  SeeAlso     [Base class BoolEncClient]   
  
******************************************************************************/
typedef struct BeEnc_TAG
{
  /* this MUST stay on the top */
  INHERITS_FROM(BoolEncClient); 

  /* -------------------------------------------------- */
  /*                  Private members                   */
  /* -------------------------------------------------- */

  /* the boolean expr manager, that is in charge of actually allocate
     be variables: */
  Be_Manager_ptr be_mgr;   

  /* the currently allocated space for variables, that is not the
   number of variables actually allocated. */
  int capacity; 
   
  /* the amount of memory should be requested as an excess to optimize 
   memory handling */
  int grow_excess; 

  /* max time reached until now for the variables environment: */
  int max_allocated_time;          
  
  /* the maximum allocated index of the be manager */
  int max_allocated_phy_idx; 

  /* number of variables occurring in the untimed logical block */
  int state_vars_num;
  int input_vars_num;

  /* queue of physical indices that can be reused after a layer is
     removed: */
  NodeList_ptr avail_phy_idx_queue; 

  /* var name to corresponding be: */
  hash_ptr name2be;     

  /* var logical index to name: */
  node_ptr* index2name; 
  int index2name_size;
  
  /* logical to physical indices map and viceversa: */
  int* log2phy;
  int* phy2log;

  int* subst_array; /* Used by substitution operations */
  int subst_array_size; 

  st_table* shift_hash; /* used to memoize shifting operations */

  /* -------------------------------------------------- */
  /*                  Virtual methods                   */
  /* -------------------------------------------------- */

} BeEnc;



/* ---------------------------------------------------------------------- */
/* Private methods to be used by derivate and friend classes only         */
/* ---------------------------------------------------------------------- */
void be_enc_init ARGS((BeEnc_ptr self, 
                       SymbTable_ptr symb_table, 
                       BoolEnc_ptr bool_enc));

void be_enc_deinit ARGS((BeEnc_ptr self));

void be_enc_commit_layer ARGS((BaseEnc_ptr enc_base, const char* layer_name));
void be_enc_remove_layer ARGS((BaseEnc_ptr enc_base, const char* layer_name));


#endif /* __BE_ENC_PRIVATE_H__ */

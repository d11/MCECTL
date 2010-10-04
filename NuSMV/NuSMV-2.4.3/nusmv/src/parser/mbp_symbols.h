/**CHeaderFile*****************************************************************

  FileName    [mbp_symbols.h]

  PackageName [parser]

  Synopsis    [Parse-tree symbols set required by MBP parkage]

  Description [This file defines an enum containing all the MBP parse
  tree symbols set]

  Author      []

  Copyright   [
  This file is part of the ``parser'' package of NuSMV version 2. 
  Copyright (C) 2006 by ITC-irst. 

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

  Revision    [$Id: mbp_symbols.h,v 1.1.2.1 2006/07/24 16:57:31 nusmv Exp $]

******************************************************************************/


#ifndef __PARSER_MBP_SYMBOLS_H__
#define __PARSER_MBP_SYMBOLS_H__

enum {
  NUSMV_MBP_SYMBOL_FIRST = 2000, /* Do not touch this */
  /* ---------------------------------------------------------------------- */

  GAME,
  PLAYER,

  REACHTARGET,
  AVOIDTARGET,
  REACHDEADLOCK,
  AVOIDDEADLOCK,
  BUCHIGAME,
  GENREACTIVITY,

  NUSMV_MBP_STATEMENTS_SYMBOL_LAST, 
  /* ---------------------------------------------------------------------- */


  /* ---------------------------------------------------------------------- */
  NUSMV_MBP_EXPR_SYMBOL_FIRST, 
  
  MBP_SPEC_WRAPPER, /* game specs are wrapped in this node by mbp flattener */
  MBP_EXP_LIST, /* one exp list */
  MBP_TWO_EXP_LISTS, /* two exp lists */
  
  
  NUSMV_MBP_EXPR_SYMBOL_LAST, 
  /* ---------------------------------------------------------------------- */

  NUSMV_MBP_SYMBOL_LAST
};



#endif /* __PARSER_MBP_SYMBOLS_H__ */

/**CHeaderFile*****************************************************************

  FileName    [symbols.h]

  PackageName [parser]

  Synopsis    [Parse-tree symbols set]

  Description [This file defines an enum containing all the core parse
  tree symbols set]

  Author      [Nusmv Team]

  Copyright   [
  This file is part of the ``parser'' package of NuSMV version 2. 
  Copyright (C) 2005 by ITC-irst. 

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

  Revision    [$Id: symbols.h,v 1.1.2.1.4.20.4.4 2007/05/11 08:37:48 nusmv Exp $]

******************************************************************************/


#ifndef __PARSER_SYMBOLS_H__
#define __PARSER_SYMBOLS_H__

enum {
  NUSMV_CORE_SYMBOL_FIRST = 100, 

  /* ---------------------------------------------------------------------- */
  NUSMV_STATEMENTS_SYMBOL_FIRST = NUSMV_CORE_SYMBOL_FIRST, 

  TRANS,
  INIT, 
  INVAR,
  ASSIGN,
  FAIRNESS,
  JUSTICE,
  COMPASSION,
  SPEC,
  LTLSPEC,
  PSLSPEC,
  INVARSPEC,
  COMPUTE, 
  DEFINE,
  ISA,
  GOTO, 
  CONSTRAINT,
  MODULE,
  PROCESS,
  MODTYPE,
  LAMBDA,
  CONSTANTS,  

  PRED, 
  PREDS_LIST, 
  MIRROR, 

  NUSMV_STATEMENTS_SYMBOL_LAST, 
  /* ---------------------------------------------------------------------- */


  /* ---------------------------------------------------------------------- */
  NUSMV_EXPR_SYMBOL_FIRST, 

  FAILURE,
  CONTEXT,
  EU,
  AU,
  EBU,
  ABU,
  MINU,
  MAXU,
  VAR,
  IVAR,
  BOOLEAN,
  ARRAY,
  OF,
  SCALAR,
  CONS,
  BDD,
  SEMI,
  LP,
  RP,
  LB,
  RB,
  LCB,
  RCB,
  EQDEF,
  TWODOTS,
  FALSEEXP,
  TRUEEXP,
  SELF,
  CASE,
  ESAC,
  COLON,
  IFTHENELSE,
  INCONTEXT,
  SIMPWFF,
  LTLWFF,
  CTLWFF,
  COMPWFF,
  ATOM,
  NUMBER,
  COMMA,
  IMPLIES,
  IFF,
  OR,
  XOR,
  XNOR,
  AND,
  NOT,
  EX,
  AX,
  EF,
  AF,
  EG,
  AG,
  EE,
  AA,
  SINCE,
  UNTIL,
  TRIGGERED,
  RELEASES,
  EBF,
  EBG,
  ABF,
  ABG,
  BUNTIL,
  MMIN,
  MMAX,
  OP_NEXT,
  OP_GLOBAL,
  OP_FUTURE,
  OP_PREC,
  OP_NOTPRECNOT,
  OP_HISTORICAL,
  OP_ONCE,
  EQUAL,
  NOTEQUAL,
  LT,
  GT,
  LE,
  GE,
  UNION,
  SETIN,
  MOD,
  PLUS,
  MINUS,
  TIMES,
  DIVIDE,
  UMINUS,
  NEXT,
  SMALLINIT,
  DOT,
  BIT,
  RANGE,
  WORD,
  INTEGER,
  REAL,
  NUMBER_WORD,
  NUMBER_FRAC,
  NUMBER_REAL,
  NUMBER_EXP,
  LSHIFT,
  RSHIFT,
  LROTATE,
  RROTATE,
  BIT_SELECTION,
  CONCATENATION,
  CAST_BOOL,
  CAST_WORD1,
  WORDARRAY,
  WAREAD,
  WAWRITE,

  /* Mathsat symbols */
  SLT, /* signed less then */
  SLE, /* signed less or equal */
  SGT, /* signed greater then */
  SGE, /* signed greater or equal */
  SIGN_EXTEND, /* sign extend */

  NUSMV_EXPR_SYMBOL_LAST, 
  /* ---------------------------------------------------------------------- */

  NUSMV_CORE_SYMBOL_LAST
};



#endif /* __PARSER_SYMBOLS_H__ */

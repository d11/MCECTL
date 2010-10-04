%{
/**CFile***********************************************************************

  FileName    [idlist_input.l]

  PackageName [parser.idlist]

  Synopsis    [Lexical analyzer for the list of IDs grammar]

  SeeAlso     [idlist_grammar.y]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``parser.idlist'' package of NuSMV version 2. 
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

******************************************************************************/
 
#include "utils/utils.h"
#include "utils/ustring.h"
#include "utils/error.h"

#include "idlist_grammar.h"
#include "ParserIdList.h"
#include "ParserIdList_private.h"

static char rcsid[] UTIL_UNUSED = "$Id: idlist_input.l,v 1.1.2.2 2006/12/05 14:14:52 nusmv Exp $";


void parser_idlist_error(char *s);


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/* The parser instance used by flex/bison generated code */
static ParserIdList_ptr _parser = PARSER_ID_LIST(NULL);

void parser_idlist_set_global_parser(ParserIdList_ptr parser)
{
  nusmv_assert(_parser == PARSER_ID_LIST(NULL));
  _parser = parser;
}

void parser_idlist_reset_global_parser(ParserIdList_ptr parser)
{
  PARSER_ID_LIST_CHECK_INSTANCE(_parser);
  nusmv_assert(parser == _parser);
  _parser = PARSER_ID_LIST(NULL);
}

ParserIdList_ptr parser_idlist_get_global_parser(void)
{
   PARSER_ID_LIST_CHECK_INSTANCE(_parser);
   return _parser;
}


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static int nusmv_skip_comment ARGS((void));


/* The following are needed in order to allow parsing of long
   comments, that otherwise will cause errors since flex thinks that
   it is using REJECT even if it isn't.  The "%option noreject" seems
   to be ignored in the latest (and previous) versions of flex.
*/
#ifdef YY_USES_REJECT
#undef YY_USES_REJECT
#endif

%}
%a	15000
%o	25000
%option noreject
%option prefix="parser_idlist_"
%option outfile="lex.yy.c"


%%
[ \n\t\r\f]             ;
"--"                    nusmv_skip_comment();
"."                     return DOT;
"["                     return LB;
"]"                     return RB;
[A-Za-z_][A-Za-z0-9_\$#-]*  { 
	  parser_idlist_lval.node = 
	  parser_id_list_mk_atom(parser_idlist_get_global_parser(), 
 				 parser_idlist_text);
	  return ATOM;
	  }
[0-9]+                  { 
  int i;
  sscanf(parser_idlist_text, "%d", &i);
  parser_idlist_lval.node = 
  parser_id_list_mk_num(parser_idlist_get_global_parser(),i);
  return NUMBER;
  }
.			{ 
                           parser_idlist_error("illegal character");
			   exit(1);
                        }
%%

static int nusmv_skip_comment() 
{
  register int c;

  do {
    c = input();
  } while ( c != '\n' && c != EOF );

  return 0;
}
 

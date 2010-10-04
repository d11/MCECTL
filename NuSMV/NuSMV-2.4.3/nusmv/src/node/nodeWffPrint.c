/**CFile***********************************************************************

  FileName    [nodeWffPrint.c]

  PackageName [node]

  Synopsis    [Pretty printing of formulas represented using node struct.]

  Description [This file conatins the code to perform pretty printing
  of a formula represented with a node struct.]

  SeeAlso     [node.c]

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``node'' package of NuSMV version 2.
  Copyright (C) 1998-2001 by CMU and ITC-irst.

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

#include "nodeInt.h"


static char rcsid[] UTIL_UNUSED = "$Id: nodeWffPrint.c,v 1.13.2.4.4.17 2006/03/13 09:58:35 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Pretty print a formula on a file]

  Description        [Pretty print a formula on a file]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int print_node(FILE *stream, node_ptr n)
{ 
  MasterPrinter_ptr mp = node_pkg_get_global_master_wff_printer();
  MasterPrinter_set_stream_type(mp, STREAM_TYPE_FILE, (StreamTypeArg) stream);
  return MasterPrinter_print_node(mp, n);
}



/**Function********************************************************************

  Synopsis           [Pretty print a formula into a string]

  Description        [Pretty print a formula into a string. The returned 
  string must be freed after using it.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
char* sprint_node(node_ptr n)
{
  MasterPrinter_ptr mp = node_pkg_get_global_master_wff_printer();
  MasterPrinter_set_stream_type(mp, 
				STREAM_TYPE_STRING, STREAM_TYPE_ARG_UNUSED);

  MasterPrinter_reset_string_stream(mp);
  MasterPrinter_print_node(mp, n);
  return strdup(MasterPrinter_get_streamed_string(mp));
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

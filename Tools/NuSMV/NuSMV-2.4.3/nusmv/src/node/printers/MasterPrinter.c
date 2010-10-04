
/**CFile***********************************************************************

  FileName    [MasterPrinter.c]

  PackageName [node.printers]

  Synopsis    [Implementaion of class 'MasterPrinter', derived from 
  MasterNodeWalker]

  Description []

  SeeAlso     [MasterPrinter.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``node.printers'' package of NuSMV version 2. 
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

  Revision    [$Id: MasterPrinter.c,v 1.1.2.6 2006/07/12 12:39:10 nusmv Exp $]

******************************************************************************/

#include "MasterPrinter.h" 
#include "MasterPrinter_private.h" 

#include "node/MasterNodeWalker_private.h"

#include "PrinterBase_private.h"
#include "printersInt.h"

#include "opt/opt.h"
#include "utils/utils.h" 
#include "utils/NodeList.h"
#include "utils/error.h"


static char rcsid[] UTIL_UNUSED = "$Id: MasterPrinter.c,v 1.1.2.6 2006/07/12 12:39:10 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [MasterPrinter class definition]

  Description []

  SeeAlso     []   
  
******************************************************************************/
typedef struct MasterPrinter_TAG
{
  /* this MUST stay on the top */
  INHERITS_FROM(MasterNodeWalker); 


  /* -------------------------------------------------- */
  /*                  Private members                   */
  /* -------------------------------------------------- */
  StreamType stream_type;
  StreamTypeArg stream_arg;

  /* string stream is handled by using these varaibles: */
  char* sstream;       /* string stream */
  size_t sstream_cap;  /* sstream allocation capacity */
  size_t sstream_len;  /* sstream current usage */
  size_t sstream_grow_sum; /* sstream grow sum */
  int sstream_grow_num; /* number of resizes */

  int (*inner_stream_print)(MasterPrinter_ptr self, const char* str);
  int (*external_stream_print)(void* stream, const char* str);

} MasterPrinter;



/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void master_printer_init ARGS((MasterPrinter_ptr self));
static void master_printer_deinit ARGS((MasterPrinter_ptr self));

static void master_printer_finalize ARGS((Object_ptr object, void* dummy));

static int 
master_printer_fprint ARGS((MasterPrinter_ptr self, const char* str));

static int 
master_printer_sprint ARGS((MasterPrinter_ptr self, const char* str));




/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The MasterPrinter class constructor]

  Description        [The MasterPrinter class constructor]

  SideEffects        []

  SeeAlso            [MasterPrinter_destroy]   
  
******************************************************************************/
MasterPrinter_ptr MasterPrinter_create()
{
  MasterPrinter_ptr self = ALLOC(MasterPrinter, 1);
  MASTER_PRINTER_CHECK_INSTANCE(self);

  master_printer_init(self);
  return self;
}


/**Function********************************************************************

  Synopsis           [Prints the given node on the stream currently set]

  Description        [If the stream is a string stream, the result can be 
  obtained be calling MasterPrinter_get_streamed_string. Returns 
  0 if an error occurred for some reason.]

  SideEffects        []

  SeeAlso            []   
  
******************************************************************************/
int MasterPrinter_print_node(MasterPrinter_ptr self, node_ptr n)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);
  return master_printer_print_node(self, n, 0);
}


/**Function********************************************************************

  Synopsis           [Prints the given string on the stream currently set]

  Description        [If the stream is a string stream, the result can be 
  obtained be calling MasterPrinter_get_streamed_string. Returns 
  0 if an error occurred for some reason.]

  SideEffects        []

  SeeAlso            []   
  
******************************************************************************/
int MasterPrinter_print_string(MasterPrinter_ptr self, const char* str)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);

  switch (self->stream_type) {
  case STREAM_TYPE_DEFAULT:
  case STREAM_TYPE_STDOUT:
  case STREAM_TYPE_STDERR:
  case STREAM_TYPE_FILE:
    return master_printer_fprint(self, str);

  case STREAM_TYPE_STRING:
      return master_printer_sprint(self, str);

  case STREAM_TYPE_FUNCTION:
    return self->stream_arg.func(str);

  default:
    internal_error("MasterPrinter::print_string: Invalid stream type\n");
  }

  return 0;  
}



/**Function********************************************************************

  Synopsis           [Returns the string that has been streamed]

  Description        [Returned string belongs to self, DO NOT free it. 
  
  Warning: this method can be called only when the current
  stream type is STREAM_TYPE_STRING.]

  SideEffects        []

  SeeAlso            [reset_string_stream]   
  
******************************************************************************/
const char* MasterPrinter_get_streamed_string(const MasterPrinter_ptr self)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);  
  nusmv_assert(self->stream_type == STREAM_TYPE_STRING);
  
  return (const char*) (self->sstream);
}


/**Function********************************************************************

  Synopsis           [Cleans up the stream that have been read so far. 
  Any previoulsy read stream will be lost]

  Description        []

  SideEffects        []

  SeeAlso            [get_streamed_string]   
  
******************************************************************************/
void MasterPrinter_reset_string_stream(MasterPrinter_ptr self)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);

  self->sstream[0] = '\0';
  self->sstream_len = 0;  
}



/**Function********************************************************************

  Synopsis [Sets the stream type to be used to produce a printing
  result]

  Description [When the given type requires an argument (for example,
  STREAM_TYPE_FILE requires a file), the argument must be passed by
  using the 'arg' parameter. When not required (for example
  STREAM_TYPE_STRING), the caller can pass STREAM_TYPE_ARG_UNUSED 
  as argument. 

  When STREAM_TYPE_FILE is used, the argument must be the handler of an open 
  writable file.
  ]
  

  SideEffects        []

  SeeAlso            []
  
******************************************************************************/
void MasterPrinter_set_stream_type(MasterPrinter_ptr self, 
				   StreamType type, StreamTypeArg arg)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);
  
  switch (type) {
  case STREAM_TYPE_DEFAULT:
  case STREAM_TYPE_STDOUT: 
    self->stream_arg.file = nusmv_stdout;
    break;

  case STREAM_TYPE_STDERR:
    self->stream_arg.file = nusmv_stderr;
    break;

  case STREAM_TYPE_STRING: 
    if (self->stream_type != STREAM_TYPE_STRING) {
      /* we start with a new string stream, but only if it is not
	 currently set a string stream */
      MasterPrinter_reset_string_stream(self);
    }
    break;

  case STREAM_TYPE_FILE:
    self->stream_arg.file = arg.file;
    break;

  case STREAM_TYPE_FUNCTION:
    self->stream_arg.func = arg.func;
    break;

  default:
    internal_error("MasterPrinter::set_stream_type: Invalid stream type\n");
  }
      
  self->stream_type = type;
}


/**Function********************************************************************

  Synopsis           [Returns the currently set stream type]

  Description        [Returns the currently set stream type]

  SideEffects        []

  SeeAlso            []
  
******************************************************************************/
StreamType MasterPrinter_get_stream_type(const MasterPrinter_ptr self)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);
  return self->stream_type;
}


/**Function********************************************************************

  Synopsis           [Closes the current stream, if possible or applicable]

  Description [The currently set stream is closed (file) or reset
  (string) and the stream type is set to be STREAM_TYPE_DEFAULT.
  IMPORTANT: If the current stream is nusmv_std{out,err} the stream is
  not closed. 
  
  This function is provided to allow the called to forget the set
  stream after setting it into the master printer.]

  SideEffects        []

  SeeAlso            []
  
******************************************************************************/
void MasterPrinter_close_stream(MasterPrinter_ptr self)
{
  MASTER_PRINTER_CHECK_INSTANCE(self);

  switch (self->stream_type) {
  case STREAM_TYPE_DEFAULT:
  case STREAM_TYPE_STDOUT:
  case STREAM_TYPE_STDERR:
    break;

  case STREAM_TYPE_STRING:
    /* resets the string stream */
    MasterPrinter_reset_string_stream(self);
    break;

  case STREAM_TYPE_FILE:
    /* closes the file stream if not nusmv_std{out,err} */
    if ((self->stream_arg.file != nusmv_stdout) && 
	(self->stream_arg.file != nusmv_stderr)) {
      fclose(self->stream_arg.file);
      break;      
    }

  case STREAM_TYPE_FUNCTION:
    break;

  default:
    nusmv_assert(false); /* no other possible cases */
  }

  /* sets the default stream type */
  MasterPrinter_set_stream_type(self, STREAM_TYPE_DEFAULT, 
				STREAM_TYPE_ARG_UNUSED);

}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Internal version of the method print_node, callable 
  internally and by printers]

  Description        []

  SideEffects        []

  SeeAlso            []   
  
******************************************************************************/
int master_printer_print_node(MasterPrinter_ptr self, 
			      node_ptr n, int priority)
{
  ListIter_ptr iter;
  iter = NodeList_get_first_iter(MASTER_NODE_WALKER(self)->walkers);
  while (!ListIter_is_end(iter)) {
    PrinterBase_ptr pr = 
      PRINTER_BASE(NodeList_get_elem_at(MASTER_NODE_WALKER(self)->walkers, 
					iter)); 
  
    if (NodeWalker_can_handle(NODE_WALKER(pr), n)) {
      return PrinterBase_print_node(pr, n, priority);
    }

    iter = ListIter_get_next(iter);
  }
  
  return 0;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The MasterPrinter class private initializer]

  Description        [The MasterPrinter class private initializer]

  SideEffects        []

  SeeAlso            [MasterPrinter_create]   
  
******************************************************************************/
static void master_printer_init(MasterPrinter_ptr self)
{
  /* base class initialization */
  master_node_walker_init(MASTER_NODE_WALKER(self));

  /* allocates a minimal string stream */
  self->sstream_cap = 1;
  self->sstream_len = 0;
  self->sstream_grow_sum = 0;
  self->sstream_grow_num = 0;
  self->sstream = ALLOC(char, self->sstream_cap);
  self->sstream[0] = '\0';

  /* default stream */
  MasterPrinter_set_stream_type(self, STREAM_TYPE_DEFAULT, 
				STREAM_TYPE_ARG_UNUSED);

  /* virtual methods settings */  
  OVERRIDE(Object, finalize) = master_printer_finalize;
}


/**Function********************************************************************

  Synopsis           [The MasterPrinter class private deinitializer]

  Description        [The MasterPrinter class private deinitializer]

  SideEffects        []

  SeeAlso            [MasterPrinter_destroy]   
  
******************************************************************************/
static void master_printer_deinit(MasterPrinter_ptr self)
{
  FREE(self->sstream);
  self->sstream = (char*) NULL;

  /* base class deinitialization */
  master_node_walker_deinit(MASTER_NODE_WALKER(self));
}


/**Function********************************************************************

  Synopsis    [The PrinterBase class virtual finalizer]

  Description [Called by the class destructor]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void master_printer_finalize(Object_ptr object, void* dummy) 
{
  MasterPrinter_ptr self = MASTER_PRINTER(object);

  master_printer_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis           [Appends a string to the internal string stream]

  Description        [Warning: current stream type must be STREAM_TYPE_STRING]

  SideEffects        []

  SeeAlso            []
  
******************************************************************************/
static int master_printer_sprint(MasterPrinter_ptr self, const char* str)
{
  int len;

  nusmv_assert(self->stream_type == STREAM_TYPE_STRING);

  len = strlen(str);
  self->sstream_len += len;

  if (self->sstream_len >= self->sstream_cap) {
    /* resizes the sstream */
    self->sstream_grow_sum += self->sstream_len;
    self->sstream_grow_num += 1;
    self->sstream_cap = self->sstream_len + 1 + 
      (self->sstream_grow_sum / self->sstream_grow_num);
    
    self->sstream = (char*) REALLOC(char, self->sstream, self->sstream_cap); 
    nusmv_assert(self->sstream != (char*) NULL);      
  }
    
  strcat(self->sstream, str);
  return 1;
}


/**Function********************************************************************

  Synopsis           [Stream the given string to the internally set file 
  stream]

  Description        [Warning: current stream type must be compatible]

  SideEffects        []

  SeeAlso            []
  
******************************************************************************/
static int master_printer_fprint(MasterPrinter_ptr self, const char* str)
{
  nusmv_assert( (self->stream_type == STREAM_TYPE_DEFAULT) ||
		(self->stream_type == STREAM_TYPE_STDOUT) ||
		(self->stream_type == STREAM_TYPE_STDERR) ||
		(self->stream_type == STREAM_TYPE_FILE));
  
  if (str[0] != '\0') return fprintf(self->stream_arg.file, str);
  else return 1;
}


/**AutomaticEnd***************************************************************/


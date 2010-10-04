/**CFile***********************************************************************

  FileName    [TraceManager.c]

  PackageName [trace]

  Synopsis    [Routines related to TraceManager's functionality.]

  Description [Primitives to create, query and manipulate TraceManager are
  provided.]
		
  SeeAlso     []

  Author      [Ashutosh Trivedi]

  Copyright   [
  This file is part of the ``trace'' package of NuSMV version 2. 
  Copyright (C) 2003 by ITC-irst.

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

#if HAVE_CONFIG
# include "config.h"
#endif

#include "pkg_traceInt.h"
#include "Trace.h"
#include "TraceLabel.h"
#include "TraceManager.h"
#include "Trace_private.h" /* To access special methods of Trace class */
#include "trace/plugins/TraceExplainer.h" /* the default plugin */
#include "trace/plugins/TraceTable.h" /* table plugin */
#include "trace/plugins/TraceXml.h" /* the XML plugins */

#if HAVE_SA
#include "sa/plugins/TraceVIS.h" /* the STSA plugin */
#endif

static char rcsid[] UTIL_UNUSED = "$Id: TraceManager.c,v 1.1.2.29.4.4.4.4 2007/05/17 11:46:54 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [TraceManager Class]

  Description [ This class contains informations about TraceManager:<br>
  	<dl> 
        <dt><code>trace_list</code>
            <dd>  List of Traces registered with TraceManager. 
        <dt><code>plugin_list</code>
            <dd>  List of plugins registered with TraceManager.
        <dt><code>current_trace_number</code>
            <dd>  Index of the current trace. 
        <dt><code>default_plugin</code>
            <dd>  default plugin to print traces. 
        <dt><code>internal_plugins_num</code>
            <dd> The number of plugins registered within NuSMV. All
                 the possibly existing other external plugins will be
                 assigned to indices greater or equal to this value.
	</dl>
	<br>
	]

  SeeAlso     []   
  
******************************************************************************/
typedef struct TraceManager_TAG 
{
  array_t* trace_list;
  array_t* plugin_list;
  array_t* layer_names;

  int current_trace_number;
  int default_plugin;

  int internal_plugins_num;
} TraceManager;


/*---------------------------------------------------------------------------*/
/* Definition of external functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Initializes the TraceManager.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
TraceManager_ptr TraceManager_create(void)
{
  TraceManager_ptr self = ALLOC(TraceManager, 1);

  TRACE_MANAGER_CHECK_INSTANCE(self);

  self->trace_list = array_alloc(Trace_ptr, 1);
  nusmv_assert(self->trace_list != (array_t *) ARRAY_OUT_OF_MEM);

  self->plugin_list = array_alloc(TracePlugin_ptr, 1);
  nusmv_assert(self->plugin_list != (array_t *) ARRAY_OUT_OF_MEM);

  self->layer_names = array_alloc(const char*, 1);
  nusmv_assert(self->layer_names != (array_t *) ARRAY_OUT_OF_MEM);

  self->current_trace_number = -1 ;  /* Not yet assigned */
  self->default_plugin = 0 ; /* Default plugin */

  self->internal_plugins_num = 0; /* number of plugins within NuSMV */
  return self;
}

/**Function********************************************************************

  Synopsis    [Destroys the TraceManager with all the registered traces and
  plugins]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
void TraceManager_destroy(TraceManager_ptr self)
{
  int i, num;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  num = TraceManager_get_size(self);
  for (i = 0; i < num; ++i) {
    Trace_ptr trace = TraceManager_get_trace_at_index(self, i);
    Trace_unregister(trace);
    Trace_destroy(trace);
  }
  array_free(self->trace_list);

  num = array_n(self->plugin_list);
  for (i = 0; i < num; ++i) {
    Object_destroy(OBJECT(TraceManager_get_plugin_at_index(self, i)), NULL);
  }
  array_free(self->plugin_list);

  num = array_n(self->layer_names);
  for (i = 0; i < num; ++i) {
    const char* name = array_fetch(const char*, self->layer_names, i);
    if (name != (const char*) NULL) { FREE(name); }
  }
  array_free(self->layer_names);

  FREE(self);
}


/**Function********************************************************************

  Synopsis    [Returns the size of the TraceManager.]

  Description [This function returns the number of traces registered with
  traceManager]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_get_size(const TraceManager_ptr self)
{
  TRACE_MANAGER_CHECK_INSTANCE(self);

  return array_n(self->trace_list);
}

/**Function********************************************************************

  Synopsis [Returns the total number of plugins registered with
  TraceManager.]

  Description [This function returns the total number of plugins
  registered with traceManager]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_get_plugin_size(const TraceManager_ptr self)
{
  TRACE_MANAGER_CHECK_INSTANCE(self);

  return array_n(self->plugin_list);
}

/**Function********************************************************************

  Synopsis [Returns the number of internal plugins registered with
  TraceManager.]

  Description [This function returns the number of internal plugins
  registered with traceManager]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_get_internal_plugin_size(const TraceManager_ptr self)
{
  TRACE_MANAGER_CHECK_INSTANCE(self);

  return self->internal_plugins_num;
}

/**Function********************************************************************

  Synopsis    [Returns the trace stored at given index]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
Trace_ptr TraceManager_get_trace_at_index(const TraceManager_ptr self,
					  int index)
{
  Trace_ptr trace;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  nusmv_assert(index < array_n(self->trace_list));
  nusmv_assert(index >= 0);

  trace = array_fetch(Trace_ptr, self->trace_list, index);

  return trace;
}

/**Function********************************************************************

  Synopsis    [Returns the plugin stored at given index]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
TracePlugin_ptr 
TraceManager_get_plugin_at_index(const TraceManager_ptr self, int index)
{
  TracePlugin_ptr plugin;

  TRACE_MANAGER_CHECK_INSTANCE(self);
  nusmv_assert(index >= 0);
  nusmv_assert(index < array_n(self->plugin_list) );

  plugin = array_fetch(TracePlugin_ptr, self->plugin_list, index);

  return plugin;
}

/**Function********************************************************************

  Synopsis    [Registers a trace with TraceManager.]

  Description [It registers a trace with the TraceManager and returns
  the corresponding index. The given trace can not be previously
  registered with any Trace Manager.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_register_trace(TraceManager_ptr self, Trace_ptr trace)
{
  int index;
  boolean status;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  nusmv_assert(!Trace_is_registered(trace));

  status = array_insert_last(Trace_ptr, self->trace_list, trace);
  nusmv_assert(status != ARRAY_OUT_OF_MEM);

  index = array_n(self->trace_list) - 1;
  Trace_set_ID(trace, index+1);  /* Friend function of Trace Class */

  return index;
}

/**Function********************************************************************

  Synopsis    [Registers a plugin with TraceManager.]

  Description [It registers a plugin with the TraceManager and returns the
  corresponding index.] 

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_register_plugin(TraceManager_ptr self, TracePlugin_ptr plugin)
{
  int res;
  boolean status;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  status = array_insert_last(TracePlugin_ptr, self->plugin_list, plugin); 
  nusmv_assert(status != ARRAY_OUT_OF_MEM);

  res = array_n(self->plugin_list);

  /* returns the index of the plugin for further use as handle */
  return (res - 1);
}


/**Function********************************************************************

  Synopsis [Registers a new layer name to be used later by the
  explainers when printing symbols. Only the symbols into registered
  layers will be shown.]

  Description [Use this method to control which symbols will be shown
  when a trace is shown. Only symbols occurring inside registered
  layers will be presented by plugins. Warning: before renaming or
  deleting a previoulsy registered layer, the layer should be
  unregistered. If not unregistered, the behaviour is unpredictable.]

  SideEffects []

  SeeAlso     [unregister_layer]

******************************************************************************/
void TraceManager_register_layer(TraceManager_ptr self, const char* layer_name)
{
  const char* name;
  int idx;
  boolean found = false;

  TRACE_MANAGER_CHECK_INSTANCE(self);
  
  /* does not registers if already registered */
  if (TraceManager_is_layer_registered(self, layer_name)) return;

  /* first search for a hole */
  arrayForEachItem(const char*, self->layer_names, idx, name) {
    if (name == (const char*) NULL) {
      array_insert(const char*, self->layer_names, idx, 
                   util_strsav((char*) layer_name));
      found = true;
      break;
    }
  }

  if (!found) { 
    /* if not inserted in a hole, push at the end */
    array_insert_last(const char*, self->layer_names, 
                      util_strsav((char*) layer_name));
  }

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stderr, "TraceManager: registered layer '%s'\n", layer_name);
  }

}


/**Function********************************************************************

  Synopsis    [Unregisters a previoulsy registered layer]

  Description [The given layer must be registered before calling this method, 
  otherwise an internal error occurs]

  SideEffects []

  SeeAlso     [register_layer]

******************************************************************************/
void TraceManager_unregister_layer(TraceManager_ptr self, 
				   const char* layer_name)
{
  int idx;
  const char* name;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stderr, "TraceManager: unregistering layer '%s'...\n", 
	    layer_name);
  }

  arrayForEachItem(const char*, self->layer_names, idx, name) {    
    if ((name != (const char*) NULL) && strcmp(name, layer_name) == 0) {
      FREE(name);
      array_insert(const char*, self->layer_names, idx, NULL);
      return;
    }
  }

  internal_error("TraceManager_unregister_layer: "\
		 "given layer had not been registered\n");
}


/**Function********************************************************************

  Synopsis    [Returns true if the given layer names was previously 
  registered]

  Description []

  SideEffects []

  SeeAlso     [unregister_layer]

******************************************************************************/
boolean TraceManager_is_layer_registered(const TraceManager_ptr self, 
					 const char* layer_name)
{
  int idx;
  const char* name;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  arrayForEachItem(const char*, self->layer_names, idx, name) {
    if ((name != (const char*) NULL) && 
        strcmp(name, layer_name) == 0) return true;
  }

  return false;
}


/**Function********************************************************************

  Synopsis    [Returns an array of names (strings) of the registered layers]

  Description [Returned array belongs to self, do not change or delete it]

  SideEffects []

  SeeAlso     []

******************************************************************************/
const array_t* TraceManager_get_registered_layers(const TraceManager_ptr self)
{
  TRACE_MANAGER_CHECK_INSTANCE(self);
  return self->layer_names;
}


/**Function********************************************************************

  Synopsis    [Execute the given trace using specified plugin. ]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_execute_plugin(const TraceManager_ptr self, int plugin_index,
                                int trace_index)
{
  Trace_ptr trace;
  TracePlugin_ptr plugin;
  int result = 0;

  TRACE_MANAGER_CHECK_INSTANCE(self);

  trace = TraceManager_get_trace_at_index(self, trace_index);
  plugin = TraceManager_get_plugin_at_index(self, plugin_index);

  switch (plugin_index) {
  case 0:  /* BASIC TRACE EXPLAINER */
  case 1:  /* BASIC TRACE EXPLAINER */
    result = TracePlugin_action(plugin, trace, self->layer_names, 
				Trace_begin(trace));
    break;
  
  case 2:  /* TRACE TABLE PLUGIN -- column format */
  case 3:  /* TRACE TABLE PLUGIN -- row format */
  case 4:  /* TRACE XML DUMP PLUGIN */
    result = TracePlugin_action(plugin, trace, self->layer_names, 
				nusmv_stdout);
    break;
    
#if HAVE_LIBEXPAT
  case 5:   /* TRACE XML LOAD PLUGIN */
    result = TracePlugin_action(plugin, trace, self->layer_names, 
				stdin);
    break;
#endif

  default:
    if (plugin_index < TraceManager_get_plugin_size(self))
      result = TracePlugin_action(plugin, trace, self->layer_names, 
				  Trace_begin(trace));
    else
      {
	/* Requested plugin is not loaded */
	internal_error("Warning: Requested plugin %d is not available. \n", 
		       plugin_index);
	nusmv_assert(false);
      }
  }

  return result;
}

/**Function********************************************************************

  Synopsis    [Sets trace_id as ths current trace of the TraceManager. ]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
void TraceManager_set_current_trace_number(TraceManager_ptr self, int trace_id)
{
  self->current_trace_number = trace_id;
}

/**Function********************************************************************

  Synopsis    [Returns the trace_id of the current trace of the TraceManager.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_get_current_trace_number(TraceManager_ptr self)
{
  return self->current_trace_number;
}

/**Function********************************************************************

  Synopsis    [Sets plugin_id as ths default_plugin of the TraceManager. ]

  Description [ Default plugin is the plugin to be used to print a trace by
  default.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void TraceManager_set_default_plugin(TraceManager_ptr self, int plugin_id)
{
  self->default_plugin = plugin_id;
}

/**Function********************************************************************

  Synopsis    [Returns the default plugin of the TraceManager.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
int TraceManager_get_default_plugin(TraceManager_ptr self)
{
  return self->default_plugin;
}


/**Function********************************************************************

  Synopsis    [Returns true if the plugin whose index is provided is
  internal to NuSMV. It returns false if the given plugin has been
  externally registered.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
boolean TraceManager_is_plugin_internal(const TraceManager_ptr self, int index)
{
  return (index >= 0 && index < self->internal_plugins_num);
}


/**Function********************************************************************

  Synopsis    [Returns the traceIterator pointing to the label.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
TraceIterator_ptr TraceManager_get_iterator_from_label(TraceManager_ptr self, 
                                                       TraceLabel label)
{
  TraceIterator_ptr result;
  Trace_ptr trace;
  int trace_no = TraceLabel_get_trace(label);
  int state_no = TraceLabel_get_state(label);

  nusmv_assert(trace_no >= 0 && trace_no < TraceManager_get_size(self));

  trace = TraceManager_get_trace_at_index(self, trace_no);

  nusmv_assert(state_no >= 0 && state_no <= Trace_get_length(trace));

  result = Trace_get_iterator(trace, Trace_get_ith_node(trace, state_no));

  return result;
}

/**Function********************************************************************

  Synopsis    [Checks if the label is valid label in a registered trace.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
boolean TraceManager_is_label_valid(TraceManager_ptr self, TraceLabel label)
{
  int trace_no, state_no;
  Trace_ptr trace;
  boolean result = true;

  trace_no = TraceLabel_get_trace(label);
  state_no = TraceLabel_get_state(label);

  if (trace_no < 0 || trace_no >= TraceManager_get_size(self)) result = false;
  else {
    trace = TraceManager_get_trace_at_index(self, trace_no);
    if (state_no < 0 || state_no > Trace_get_length(trace)) result = false;
  }

  return result;
}

/**Function********************************************************************

  Synopsis    [Registers default plugins.]

  Description [Statically registers available plugins]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void TraceManager_init_plugins(TraceManager_ptr self)
{
  TracePlugin_ptr plugin;

  /* Not previously registered: */
  nusmv_assert(self->internal_plugins_num == 0); 

  /* 0. BASIC TRACE EXPLAINER - changes only */
  plugin = TRACE_PLUGIN(TraceExplainer_create(true));
  TraceManager_register_plugin(self, plugin);

  /* 1. BASIC TRACE EXPLAINER  */
  plugin = TRACE_PLUGIN(TraceExplainer_create(false));
  TraceManager_register_plugin(self, plugin);

  /* 2. TRACE TABLE PLUGIN -- column format */
  plugin = TRACE_PLUGIN(TraceTable_create(TRACE_TABLE_TYPE_COLUMN));  
  TraceManager_register_plugin(self, plugin);
                  
  /* 3. TRACE TABLE PLUGIN -- row format */
  plugin = TRACE_PLUGIN(TraceTable_create(TRACE_TABLE_TYPE_ROW));  
  TraceManager_register_plugin(self, plugin);

  /* 4. TRACE XML DUMP PLUGIN */
  plugin = TRACE_PLUGIN(TraceXmlDumper_create());
  TraceManager_register_plugin(self, plugin);

#if HAVE_LIBEXPAT
  /* in current implementation loading plugin MUST be the last internal one */
  /* 5. TRACE XML LOAD PLUGIN */
  plugin = TRACE_PLUGIN(TraceXmlLoader_create());
  TraceManager_register_plugin(self, plugin);
#endif

  self->internal_plugins_num = TraceManager_get_plugin_size(self);

#if HAVE_SA
  /* in current implementation, SA plugin MUST be the last */
  /* 6. TRACE STSA PLUGIN */
  plugin = TRACE_PLUGIN(TraceVIS_create(true));
  TraceManager_register_plugin(self, plugin);
#endif

}


/*---------------------------------------------------------------------------*/
/* Static functions definitions                                              */
/*---------------------------------------------------------------------------*/

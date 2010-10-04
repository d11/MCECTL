/**CFile***********************************************************************

  FileName    [assoc.c]

  PackageName [util]

  Synopsis    [A simple associative list]

  Description [This file provides the user with a data structure that
  implemnts an associative list. If there is already an entry with
  the same ky in the table, than the value associated is replaced with
  the new one.]

  SeeAlso     []

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``utils'' package of NuSMV version 2. 
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

#include <stdlib.h>
#include "util.h"
#include "node/node.h"
#include "parser/symbols.h"
#include "utils/utils.h" /* for nusmv_assert */
#include "utils/assoc.h"

static char rcsid[] UTIL_UNUSED = "$Id: assoc.c,v 1.5.6.2.4.5.6.2 2007/01/15 15:40:29 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
/*
  Initial size of the associative table.
*/
#define ASSOC_HASH_SIZE 127
/* 
   Largest everage number of entries per hash element before the table
  is grown.
*/
#define ASSOC_MAX_DENSITY  ST_DEFAULT_MAX_DENSITY

/*
  The factor the table is grown when it becames too full. 
*/
#define ASSOC_GROW_FACTOR  ST_DEFAULT_GROW_FACTOR

/*
  If is non-zero, then every time an entry is found, it is moved on 
  top of the chain. 
*/
#define ASSOC_REORDER_FLAG ST_DEFAULT_REORDER_FLAG

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static unsigned long assoc_hash_fun ARGS((node_ptr, int));
static int assoc_string_key_hash_fun ARGS((node_ptr key, int size));

static int assoc_eq_fun ARGS((node_ptr, node_ptr));
static int assoc_neq_fun ARGS((node_ptr a1, node_ptr a2));
static int assoc_string_key_eq_fun ARGS((node_ptr, node_ptr));
static int assoc_string_key_neq_fun ARGS((node_ptr, node_ptr));

static enum st_retval 
assoc_get_key_aux ARGS((char *key, char *data, char * arg));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

hash_ptr new_assoc()
{
  st_table * new_table = st_init_table_with_params((ST_PFICPCP) assoc_neq_fun,
                                                   (ST_PFICPI) assoc_hash_fun,
                                                   ASSOC_HASH_SIZE,
                                                   ASSOC_MAX_DENSITY,
                                                   ASSOC_GROW_FACTOR,
                                                   ASSOC_REORDER_FLAG);
  if (new_table == (st_table *)NULL) {
    fprintf(stderr, "new_assoc: Out of Memory\n");
    exit(1);
  }
  return ((hash_ptr)new_table);
}


hash_ptr new_assoc_string_key()
{
  st_table * new_table = 
    st_init_table_with_params((ST_PFICPCP) assoc_string_key_neq_fun,
			      (ST_PFICPI) assoc_string_key_hash_fun,
			      ASSOC_HASH_SIZE,
			      ASSOC_MAX_DENSITY,
			      ASSOC_GROW_FACTOR,
			      ASSOC_REORDER_FLAG);

  if (new_table == (st_table *)NULL) {
    fprintf(stderr, "new_assoc_string_key: Out of Memory\n");
    exit(1);
  }
  return ((hash_ptr)new_table);
}


hash_ptr copy_assoc(hash_ptr hash)
{
  return (hash_ptr) st_copy((st_table *) hash);
}

node_ptr find_assoc(hash_ptr hash, node_ptr key)
{
  node_ptr data;

  if (st_lookup((st_table *)hash, (char *)key, (char **)&data)) return(data);
  else return(Nil);
}

/* Returns the list of inserted keys. If parameter ignore_nils is true,
   the those keys whose associated values are Nil (typically, removed
   associations) will not be added to the returned list. Entries in
   the returned list are presented in an arbitrary order. */
node_ptr assoc_get_keys(hash_ptr hash, boolean ignore_nils)
{
  node_ptr data = new_node(CONS, Nil, (node_ptr) ignore_nils);
  st_foreach(hash, assoc_get_key_aux, (char*) data);
  return car(data);
}

void insert_assoc(hash_ptr hash, node_ptr key, node_ptr data)
{
  (void)st_insert((st_table *)hash, (char *)key, (char *)data);
}

void remove_assoc(hash_ptr hash, node_ptr key, node_ptr data)
{
  (void)st_delete((st_table *)hash, (char **)&key, (char **)&data);
}

/*
  Frees any internal storage associated with the hash table.
  It's user responsibility to free any storage associated with the
  pointers in the table.
*/
void free_assoc(hash_ptr hash)
{
  (void)st_free_table((st_table *)hash);
}

static enum st_retval delete_entry(char *key, char *data, char * arg)
{
  return(ST_DELETE);
}

void clear_assoc(hash_ptr hash)
{ st_foreach(hash, delete_entry, NULL); }

void clear_assoc_and_free_entries(hash_ptr hash, ST_PFSR fn)
{ clear_assoc_and_free_entries_arg(hash, fn, NULL); }

void clear_assoc_and_free_entries_arg(hash_ptr hash, ST_PFSR fn, char* arg)
{
  nusmv_assert(hash != NULL);
  st_foreach(hash, fn, arg);
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

static unsigned long assoc_hash_fun(node_ptr key, int size)
{ return((unsigned long)(key) % size); }


/**Function********************************************************************

  Synopsis           [One-at-a-Time Hash function]

  Description        [Used to hash string keys.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int assoc_string_key_hash_fun(node_ptr key, int size)
{
  int hash, i;
  int len;
  
  hash = 0;
  len = strlen((const char*) key);

  for (i=0; i<len; ++i) {
    hash += ((const char*) key)[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return (hash % size);
}

static int assoc_eq_fun(node_ptr a1, node_ptr a2)
{ return((a1) == (a2)); }

static int assoc_neq_fun(node_ptr a1, node_ptr a2)
{ return((a1) != (a2)); }

static int assoc_string_key_eq_fun(node_ptr a1, node_ptr a2)
{ 
  return (assoc_eq_fun(a1, a2) || 
	  (strcmp((const char*) a1, (const char*) a2) == 0));
}

static int assoc_string_key_neq_fun(node_ptr a1, node_ptr a2)
{ 
  return (assoc_neq_fun(a1, a2) && 
	  (strcmp((const char*) a1, (const char*) a2) != 0));
}

/* A private service for assoc_get_keys */
static enum st_retval assoc_get_key_aux(char *key, char *data, char * arg)
{
  node_ptr res = car(NODE_PTR(arg));
  boolean ignore_nils = (boolean) cdr(NODE_PTR(arg));

  if (!ignore_nils || data != (char*) NULL) {
    res = cons((node_ptr) key, res);
    setcar(NODE_PTR(arg), res);
  }
  
  return ST_CONTINUE;
}


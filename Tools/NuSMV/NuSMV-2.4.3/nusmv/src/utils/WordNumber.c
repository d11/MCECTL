/**CSourceFile*****************************************************************

  FileName    [WordNumber.c]

  PackageName [utils]

  Synopsis    []

  Description []

  SeeAlso     [WordNumber.h]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``enc.utils'' package of NuSMV version 2. 
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

******************************************************************************/


#include "WordNumber.h"

#include "utils/assoc.h"
#include "node/node.h"
#include "utils/ustring.h"
#include "parser/symbols.h"
#include "opt/opt.h"

#include <stdlib.h> /* for stdtol */
#include <limits.h> /* for */
#include <errno.h> /* for errno and ERANGE */

static char rcsid[] UTIL_UNUSED = "$Id: WordNumber.c,v 1.1.2.1.4.5 2007/05/16 15:14:45 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Types definition                                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [WordNumber struct.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
typedef struct WordNumber_TAG 
{
  unsigned long long value; /* Words are unsigned */
  int width;
  string_ptr parsedString;
} WordNumber;


/**Struct**********************************************************************

  Synopsis           [The memory manager of the WordNumber class.]

  Description        [the manager looks after all the created WordNumbers and 
  destroys them during deinitialisation.
  There are two kinds of numbers: one kind contains just value and width,
  and the other one contains also a string obtained obtained during parsing.

  The former kind of numbers are stored in a hash table is used (and
  the memory is shared by the numbers). The later kind is stored in
  a list (and memory is not shared).
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
typedef struct MemoryManager_TAG 
{
  hash_ptr hashTable; /*node_ptr(value(half_1), value(half_2), width) ->
		        -> WordNumber_ptr */
  node_ptr numberList; /*list of numbers created during
				parsing -- a list of elements
				connected by CONS and Nil at the end */
} MemoryManager;
 
/*---------------------------------------------------------------------------*/
/* Macro definition                                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable definition                                                       */
/*---------------------------------------------------------------------------*/
EXTERN options_ptr options; /* for print function */


/**Variable*********************************************************************

  Synopsis           [The instance of a global memory manager of the WordNumber
  class.]

  Description        []

  SideEffects        []

  SeeAlso            [WordNumberManager_TAG ]

******************************************************************************/
static MemoryManager memoryManager;


/**Variable*********************************************************************

  Synopsis           [The maximal width of a WordConstant, i.e. 
  a maximal number of bits supported by the class ]

  Description        [This value is implementation-dependent.
  The constant is initialised in the WordNumber_init]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int wordNumberMaxWidth;

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static WordNumber_ptr word_number_create ARGS((unsigned long long value,
					       int width,
					       char* parsedString));
static void word_number_destroy ARGS((WordNumber_ptr));



/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [initialiser of the class]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void WordNumber_init(void)
{
  /* compute the maximal size of Word constants we can handle */
  if (0 == wordNumberMaxWidth) {/* the constant has not been initialized*/
    int i = 1;
    unsigned long long ull = 1;
    while ((ull << 1) > ull) {
      ull = ull << 1;
      ++i;
    }
    wordNumberMaxWidth = i; /* max number of bits in "unsigned long long" */
    
    /* two int must be enough to hold one Word constant (this is
       used in caching, see word_number_create). So, if required,
       decrease the number of bits to make it possible.
    */
    if (sizeof(int) * 2 * CHAR_BIT < wordNumberMaxWidth) {
      wordNumberMaxWidth = sizeof(int) * 2 * CHAR_BIT;
    }
  }
  
  /* the manager should not be initialized yet */
  nusmv_assert((hash_ptr)NULL == memoryManager.hashTable);
  memoryManager.hashTable = new_assoc();
  memoryManager.numberList = Nil;
}


/* hash table cleaner for the  WordNumber_quit */
static enum st_retval word_number_hashTableCleaner(char* key,
						   char* record, char* arg)
{
  WordNumber_ptr number = WORD_NUMBER(record);
  nusmv_assert(WORD_NUMBER(NULL) != number);
  word_number_destroy(number);
  return ST_DELETE;
}

/**Function********************************************************************

  Synopsis           [deinitialiser of the class]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void WordNumber_quit(void)
{
  /* the manager should have been initialized already */
  nusmv_assert((hash_ptr) NULL != memoryManager.hashTable);
  clear_assoc_and_free_entries(memoryManager.hashTable,
			       word_number_hashTableCleaner);
  free_assoc(memoryManager.hashTable);
  memoryManager.hashTable = (hash_ptr) NULL;

  while (Nil != memoryManager.numberList) {
    /* internal contrains */
    nusmv_assert(CONS == node_get_type(memoryManager.numberList));

    word_number_destroy(WORD_NUMBER(car(memoryManager.numberList)));
    memoryManager.numberList = cdr(memoryManager.numberList);
  }
  /* by the above code => memoryManager.numberList = Nil; */
}


/**Function********************************************************************

  Synopsis           [The functions returns the maximal width a Word constant
  can have. This is implemenatation-dependent limit]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int WordNumber_max_width()
{
  return wordNumberMaxWidth;
}

/**Function********************************************************************

  Synopsis           [Constructs a Word number WordNumber_ptr from the string
  representation]

  Description        [The string and base should be proper for standard
  "strtoull" function.  The base can be 2, 8 or 16.  In the case of
  any problem NULL is returned.

  Note: base 10 is not allowed, because it does not provide enough info
  about the width of the Word number.

  NOTE: Memory sharing is used, i.e. given a string with the same
  value of WordNumber this constructor will return the same pointer
  (this is important for node_ptr hashing)]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_from_string(char* str, int base)
{
  unsigned long long value;
  int width;
  char* tmpStr;
  
  value = strtoull(str, &tmpStr, base);

  /* an error happened */
  if (NULL == str || '\0' != *tmpStr || ERANGE == errno) {
    return WORD_NUMBER(NULL);
  }

  /* calculate the width */
  width = tmpStr - str;
  switch (base) {
  case 2:  /* nothing */ break;
  case 8:  width *= 3; break;
  case 16: width *= 4; break;
  default: nusmv_assert(false); /* only 2,8 and 16 bits base are allowed */
  }

  return word_number_create(value, width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [Constructs a Word number WordNumber_ptr from the string
  representation]

  Description        [The string and base should be proper for standard
  "strtoull" function. The base can be 2, 8, 10 or 16. The number
  should be in the range supposed by the width. The provided width of
  the constant should be enough to hold the obtained number. In the
  case of any problem NULL is returned.

  NOTE: Memory sharing is used, i.e. given a string with the same
  value of WordNumber this constructor will return the same pointer
  (this is important for node_ptr hashing)]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_from_sized_string(char* str, int base,
					    int width)
{
  unsigned long long value;
  char* tmpStr;
  
  /* only this bases are allowed here */
  nusmv_assert(2 == base || 8 == base || 10 == base || 16 == base);

  value = strtoull(str, &tmpStr, base);

  /* an error happened */
  if (NULL == str || '\0' != *tmpStr || ERANGE == errno) {
    return WORD_NUMBER(NULL);
  }

  /* the width is the number of digit multiplied by the base */
  return word_number_create(value, width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [Constructs a Word number WordNumber_ptr from the string
  representation obtained during parsing]

  Description        [The string is the string obtained during parsing. The
  string should correspond to the NuSMV lexer token "word constants",
  i.e. "0" character followed by the base, optional width (decimal
  number), "_" character and the value (binary, octal, decimal or
  hexadecimal number).  The base and the digits should correspond each
  other.

  The limit for width is implementation dependant.
  In the case of any problem NULL is returned, and if errorString is not NULL,
  it is set to a text string explaining the cause of the error.
  The returned error-string belongs to this function.

  NOTE: this constructor is NOT memory shared, i.e. given the same string
  twice different pointer will be returned.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_from_parsed_string(char* str,
					     char** errorString)
{
  unsigned long long value;
  long width;
  int base;

  char* currentStr;
  char* tmpStr;

  /* the buffer is used to get rid of '_' from the string */
  static char* buffer = (char*)NULL;
  static int bufferSize = 0;

  nusmv_assert((char*)NULL != str);
  currentStr = str;
  
  /* 1. remove the first "0" and get the base specifier */
  nusmv_assert('0' == *currentStr);/*first character of Word constant is "0"*/
  ++currentStr;
  
  switch (toupper(*currentStr)) {
  case 'B': base = 2;  break;
  case 'O': base = 8;  break;
  case 'D': base = 10; break;
  case 'H': base = 16; break;
  default: nusmv_assert(false);/* something wrong with the base specifier */
    /* this error should be impossible => assertion */
  } /* switch */
  ++currentStr; /* get past the base specifier */


  /* 2. calculate the explicit width of the Word constant*/
  if ('_' != *currentStr) { /* the width is explicitly specified */
    nusmv_assert(isdigit(*currentStr)); /* only a digit can go here */

    width = strtol(currentStr, &tmpStr, 10);
    
    /* error : width specifier overflow or underflow*/
    if (ERANGE == errno || width != (int)width) {
      if (NULL != errorString) { 
	*errorString = "overflow or underflow in the width specifier of a Word constant";
      }
      return WORD_NUMBER(NULL);
    }

    /* error in the width specifier */
    if ('_' != *tmpStr) { /* 'underscore must go after the width specifier */
      nusmv_assert(false);/* in current implementation this code 
			     is impossible */
      if (NULL != errorString) { 
	*errorString = "erroneous character in the width specifier of a Word constant";
      }
      return WORD_NUMBER(NULL);
    }
    
    /* 2.1 move past the optional width specifier */
    for ( ; isdigit(*currentStr); ++currentStr) {}
  }
  else width = -1; /* for sure incorrect value */

  
  /* 3. copy the value string into a buffer and remove '_' from the string */
  nusmv_assert('_' == *currentStr); /* underscore before the value part */

  if (bufferSize < strlen(currentStr)) { /* reallocate the buffer */
    bufferSize = strlen(currentStr);
    buffer = REALLOC(char, buffer, bufferSize); 
  }
  
  for (tmpStr = buffer; '\0' != *currentStr; ++currentStr) {
    if ('_' != *currentStr) {
      *tmpStr = *currentStr; /* = toupper(*str); not needed at the moment */
      ++tmpStr;
    } 
  } /* for */
  *tmpStr = '\0';

  /* 4. calculate the implicit width of the Word constant */
  if (-1 == width) {/* there was no width specifier => calculate it */
    /* calculate the number of digits */
    width = strlen(buffer);
      
    switch (base) {
    case 2: /* nothing */ break; 
    case 8: width *= 3; break;
    case 10:  /* error */
      if (NULL != errorString) { 
	*errorString = "decimal Word constant without width specifier";
      }
      return WORD_NUMBER(NULL);
    case 16:width *= 4; break;
    default: nusmv_assert(false); /* impossible error */
    } /* switch */
  }
  /* check the wellformedness of the width of the Word */
  if (width <= 0 || width > wordNumberMaxWidth) {
    if (NULL != errorString) { 
      /* construct an error message. 200 chars is enough for sure to
	 reprsent any number
      */
      static char buf[200] = "\0";
      if ('\0' == *buf) { /* the buffer has not been initialized yeet */
	int i = snprintf(buf, 200,
			 "width of a Word constant is out of range 1..%i",
			 wordNumberMaxWidth);
	nusmv_assert(i > 0 && i < 200); /* above print was successful */
      }
      *errorString = buf;
    }
    return WORD_NUMBER(NULL);
  }

  /* 5. calculate the value */
  value = strtoull(buffer, &tmpStr, base);

  /* error : value overflow or underflow*/
  if (ERANGE == errno) {
    if (NULL != errorString) { 
      *errorString = "overflow or underflow in the value of a Word constant";
    }
    return WORD_NUMBER(NULL);
  }

  /* error in the value */
  if (*tmpStr != '\0') {
      if (NULL != errorString) { 
	*errorString = "erroneous character in a Word constant value";
      }
      return WORD_NUMBER(NULL);
    }
    
  /* here two shifts are performed because shift by the width of 
     the type in C is illegal
  */
  if ( ((value >> (width-1)) >> 1) != 0) {
    if (NULL != errorString) { 
      *errorString = "value of a Word constant is outside of its width";
    }
    return WORD_NUMBER(NULL);
  }

  if (NULL != errorString) { /* potential error in the constructor */
    *errorString = "undefined error with a Word constant";
  }
  
  return word_number_create(value, width, str);
}


/**Function********************************************************************

  Synopsis           [returns a WordNumber]

  Description        [value and width should be correct, i.e. in a proper
  range.

  NOTE: Memory sharing is used, i.e. given the same parameter this
  constructor will return the same pointer (this is important for
  node_ptr hashing)]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_from_integer(unsigned long long value, int width)
{
  return word_number_create(value, width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns a memory shared WordNumber]

  Description [If a word number was created with a constructor
  WordNumber_from_parsed_string then such a WordNumber is NOT memory
  shared.
  This function takes such WordNumber and returns its memory shared
  analog, i.e. with the same width and value but without string
  information.
  If WordNumber was created with memory-shared constructor (for
  example, WordNumber_from_integer), there is no need to use this
  function since the returned value will be the same as input]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_normalize(const WordNumber_ptr number)
{
  return word_number_create(WordNumber_get_value(number),
                            WordNumber_get_width(number),
                            (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns the value of a WordNumber]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
unsigned long long WordNumber_get_value(WordNumber_ptr self)
{
  WORD_NUMBER_CHECK_INSTANCE(self);
  return self->value;
}


/**Function********************************************************************

  Synopsis           [returns the width of a WordNumber]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int WordNumber_get_width(WordNumber_ptr self)
{
  WORD_NUMBER_CHECK_INSTANCE(self);
  return self->width;
}

/**Function********************************************************************

  Synopsis           [returns the status (true or false) of a particular bit]

  Description        [the bit number should be in the range \[0, width-1\].]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_get_bit(WordNumber_ptr self, int n)
{
  WORD_NUMBER_CHECK_INSTANCE(self);
  nusmv_assert(n >= 0 && n < self->width);
  return (self->value >> n) & 1;
}


/**Function********************************************************************

  Synopsis           [returns a string which was given 
  to WordNumber_from_parsed_string constructor. If the number was created
  by any other constructor, NULL is returned.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
char* WordNumber_get_parsed_string(WordNumber_ptr self)
{
  WORD_NUMBER_CHECK_INSTANCE(self);
  if ((string_ptr)NULL != self->parsedString) {
    return get_text(self->parsedString);
  } 
  else return (char*)NULL;
}


/**Function********************************************************************

  Synopsis           [prints a Word constant to a stream in a generic format.]

  Description        [Generic format means that the number is printed
  in the base specified by the system variable "output_word_format"
  with one exception -- if the number was created with
  WordNumber_from_parsed_string, then that string provided during
  construction will be printed.]

  SideEffects        []

  SeeAlso            [WordNumber_based_print]

******************************************************************************/
int WordNumber_print(FILE* output_stream, WordNumber_ptr self)
{
  char* result = WordNumber_to_string(self);
  if ((char*)NULL == result) return -1;
  return fprintf(output_stream, "%s", result);
}


/**Function********************************************************************

  Synopsis           [prints a Word constant in a provided base.]

  Description        [returns negative value in a case of error.
  Only 2, 8, 10, 16 bits bases are allowed.]

  SideEffects        []

  SeeAlso            [WordNumber_print]

******************************************************************************/
int WordNumber_based_print(FILE* output_stream, WordNumber_ptr self, int base)
{
  char* result = WordNumber_to_based_string(self, base);
  if ((char*)NULL == result) return -1;
  return fprintf(output_stream, "%s", result);
}
 

/**Function********************************************************************

  Synopsis           [prints a Word constant to a char string.]

  Description        [It is the same as WordNumber_print
  but outputs to a string instead of a stream, i.e.:
    the number is printed in the base specified by the system variable
    "output_word_format" with one exception -- if the number was
    created with WordNumber_from_parsed_string, then that string
    provided during construction will be printed.
  In case of any problem, NULL is returned.

  Note: The returned string belongs to the funcion. Do not modify this
  string.  
  Note: The next invocation of this function or WordNumber_to_based_string
  makes the previously returned string unusable]

  SideEffects        []

  SeeAlso            [WordNumber_print, WordNumber_to_based_string]

******************************************************************************/
char* WordNumber_to_string (WordNumber_ptr self)
{
  WORD_NUMBER_CHECK_INSTANCE(self);

  if ((string_ptr)NULL != self->parsedString) {/*this is a string from parser*/
    return get_text(self->parsedString);
  }
  
  return WordNumber_to_based_string (self, get_output_word_format(options));
}

 
/**Function********************************************************************

  Synopsis           [prints a Word constant in a provided base to a string.]

  Description        [This function is the same as WordNumber_based_print,
  except this function outputs to a string, not a stream.
  Only 2, 8, 10, 16 bits bases are allowed.
  In case of any problem, NULL is returned.

  Note: The returned string belongs to the funcion. Do not modify this
  string.  
  Note: The next invocation of this function or WordNumber_to_string
  makes the previously returned string unusable]

  SideEffects        []

  SeeAlso            [WordNumber_based_print, WordNumber_to_string]

******************************************************************************/
char* WordNumber_to_based_string(WordNumber_ptr self, int base)
{
  static char* buffer = (char*)NULL;
  static int buffer_size = 0;

  int printed;

  WORD_NUMBER_CHECK_INSTANCE(self);

  /* allocate a buffer. The requires string is very likely less than width+20*/
  if (buffer_size < (self->width + 20)) {
    buffer_size = self->width + 20;
    buffer = REALLOC(char, buffer, buffer_size);
  }


  /* print the "0", the base, the width and the value */
  switch (base) {

  case 2: {
    int n = self->width - 1;
    unsigned long long v = self->value;
    char* str = buffer;
    nusmv_assert(v >= 0);

    printed = sprintf(str, "0b%i_", self->width);
    if (printed <= 0) return (char*)NULL; /* error in printing */
    else str += printed;

    /* while (((v>>n)&1) == 0 && n > 0) --n; */ /* skip initial zeros */

    while (n >= 0) {
      printed = sprintf(str, "%u", (int)((v>>n)&1));
      if ( printed <= 0) return (char*)NULL; /* error in printing */
      else str += printed;
      --n;
    }
    nusmv_assert((str - buffer) < buffer_size);
    return buffer;
  }

  case 8:  printed = sprintf(buffer, "0o%d_%llo", self->width, self->value);
    break;
  case 10: printed = sprintf(buffer, "0d%d_%llu", self->width, self->value);
    break;
  case 16: printed = sprintf(buffer, "0h%d_%llX", self->width, self->value);
    break;
  default: nusmv_assert(false); /* unknown base */
  }/* switch */

  if ( printed <= 0) return (char*)NULL; /* error in printing */
  nusmv_assert(printed < buffer_size);
  return buffer;
}


/**Function********************************************************************

  Synopsis           [perform the negation operation]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_unary_minus(WordNumber_ptr v)
{
  unsigned long long l;
  WORD_NUMBER_CHECK_INSTANCE(v); 
  
  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v->width - 1)) << 1);

  return word_number_create((- v->value)&l, v->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform summation operation]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_plus(WordNumber_ptr v1, WordNumber_ptr v2)
{
  unsigned long long l;
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);
  
  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v1->width - 1)) << 1);

  return word_number_create((v1->value + v2->value)&l, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform subtraction operation on Words]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_minus(WordNumber_ptr v1, WordNumber_ptr v2)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v1->width - 1)) << 1);
  
  return word_number_create((v1->value - v2->value)&l, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform multiplidation operation on Words]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_times(WordNumber_ptr v1, WordNumber_ptr v2)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 

  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v1->width - 1)) << 1);

  nusmv_assert(v1->width == v2->width);
  return word_number_create((v1->value * v2->value)&l, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform division operation on Words]

  Description        [the width of operands should be equal. The 
  right operand should not be 0.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_divide(WordNumber_ptr v1,  WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  nusmv_assert(0 != v2->value);
  /* division is wounded DOWN (as in evaluation procedure).
   Since 'value' is unsigned, there is no need to worry about it.
  */
  return word_number_create(v1->value / v2->value, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform remainder operation on Words]

  Description        [the width of operands should be equal. The right
  operand should not be 0.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_mod(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  nusmv_assert(0 != v2->value);
  /* C's division is rounded towards 0, but NuSMV rounds down.
   Since 'value' is unsigned, there is no need to worry about it.
  */
  return word_number_create(v1->value % v2->value, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns TRUE if operands are equal]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_equal(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return v1->value == v2->value;
}


/**Function********************************************************************

  Synopsis           [returns TRUE if operands are NOT equal]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_not_equal(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return v1->value != v2->value;
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is less than
  the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_less(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return v1->value < v2->value;
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is less than, or equal to,
  the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_less_or_equal(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return v1->value <= v2->value;
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is greater than
  the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_greater(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return v1->value > v2->value;
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is greate than, or eqaul to,
  the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_greater_or_equal(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return v1->value >= v2->value;
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is signed less than
  the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_signed_less(WordNumber_ptr v1, WordNumber_ptr v2)
{
  boolean bit1, bit2;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  bit1 = WordNumber_get_bit(v1, v1->width - 1);
  bit2 = WordNumber_get_bit(v2, v1->width - 1);

  return bit1 > bit2 || (bit1 == bit2 && v1->value < v2->value);
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is signed less than,
  or equal to, the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_signed_less_or_equal(WordNumber_ptr v1, WordNumber_ptr v2)
{
  boolean bit1, bit2;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  bit1 = WordNumber_get_bit(v1, v1->width - 1);
  bit2 = WordNumber_get_bit(v2, v1->width - 1);

  return bit1 > bit2 || (bit1 == bit2 && v1->value <= v2->value);
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is signed greater than
  the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_signed_greater(WordNumber_ptr v1, WordNumber_ptr v2)
{
  boolean bit1, bit2;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  bit1 = WordNumber_get_bit(v1, v1->width - 1);
  bit2 = WordNumber_get_bit(v2, v1->width - 1);

  return bit1 < bit2 || (bit1 == bit2 && v1->value > v2->value);
}


/**Function********************************************************************

  Synopsis           [returns TRUE if left operand is signed greate than,
  or eqaul to, the right one]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean WordNumber_signed_greater_or_equal(WordNumber_ptr v1, WordNumber_ptr v2)
{
  boolean bit1, bit2;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  bit1 = WordNumber_get_bit(v1, v1->width - 1);
  bit2 = WordNumber_get_bit(v2, v1->width - 1);

  return bit1 < bit2 || (bit1 == bit2 && v1->value >= v2->value);
}


/**Function********************************************************************

  Synopsis           [returns bitwise NOT of a Word number]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_not(WordNumber_ptr v)
{
  unsigned long long l;
  WORD_NUMBER_CHECK_INSTANCE(v); 
  
  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v->width - 1)) << 1);

  return word_number_create((~ v->value) & l, v->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns bitwise AND of two Word numbers]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_and(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return word_number_create(v1->value & v2->value, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns bitwise OR of two Word numbers]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_or(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return word_number_create(v1->value | v2->value, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns bitwise XOR of two Word numbers]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_xor(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  return word_number_create(v1->value ^ v2->value, v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns bitwise XNOR(or IFF) of two Word numbers]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_xnor(WordNumber_ptr v1, WordNumber_ptr v2)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v1->width - 1)) << 1);

  return word_number_create((~ (v1->value ^ v2->value)) & l,
			    v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns bitwise IMPLIES of two Word numbers]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_implies(WordNumber_ptr v1, WordNumber_ptr v2)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width == v2->width);

  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v1->width - 1)) << 1);

  return word_number_create(((~ v1->value) | v2->value) & l,
			    v1->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns bitwise IFF(or XNOR) of two Word numbers]

  Description        [the width of operands should be equal]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_iff(WordNumber_ptr v1, WordNumber_ptr v2)
{
  return WordNumber_xnor(v1, v2);
}


/**Function********************************************************************

  Synopsis           [returns a concatenation of two Word numbers]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_concatenate(WordNumber_ptr v1, WordNumber_ptr v2)
{
  WORD_NUMBER_CHECK_INSTANCE(v1); 
  WORD_NUMBER_CHECK_INSTANCE(v2); 
  nusmv_assert(v1->width + v2->width <= wordNumberMaxWidth);

  return word_number_create((v1->value << v2->width) | v2->value,
			    v1->width + v2->width,  (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [returns a Word number consisting of the
  bits [highBit .. lowBit] from a given Word number]

  Description        [highBit should be less than the Word width and greater or
  equal to lowBit. lowBit should be greater or equal to 0.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_bit_select(WordNumber_ptr v, int highBit, int lowBit)
{
  unsigned long long l;
  int newWidth = highBit - lowBit + 1;

  WORD_NUMBER_CHECK_INSTANCE(v); 
  nusmv_assert(v->width > highBit && highBit >= lowBit && lowBit >= 0);

  /* create a constant of 'width' number of 1 bits.
     Two left shift are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (newWidth - 1)) << 1);
  
  return word_number_create((v->value >> lowBit) & l, newWidth, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform right shift on a Word numbers]

  Description        [the number of shifted bits should be in the range 
  \[0, width-1\]. The word is padded with zeros.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_right_shift(WordNumber_ptr v, int numberOfBits)
{
  WORD_NUMBER_CHECK_INSTANCE(v); 
  nusmv_assert(v->width > numberOfBits && numberOfBits >= 0);

  return word_number_create(v->value >> numberOfBits, v->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform left shift on a Word numbers]

  Description        [the number of shifted bits should be in the range 
  \[0, width-1\]. The word is padded with zeros.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_left_shift(WordNumber_ptr v, int numberOfBits)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v); 
  nusmv_assert(v->width > numberOfBits && numberOfBits >= 0);

  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v->width - 1)) << 1);

  return word_number_create((v->value << numberOfBits) & l,
			    v->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform right rotate on a Word numbers]

  Description        [the number of rotated bits should be in the range 
  \[0, width-1\].]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_right_rotate(WordNumber_ptr v, int numberOfBits)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v); 
  nusmv_assert(v->width > numberOfBits && numberOfBits >= 0);
  
  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v->width - 1)) << 1);

  return word_number_create(( (v->value >> numberOfBits) 
			    | (v->value << (v->width - numberOfBits))
			    ) & l, 
			    v->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [perform left rotate on a Word numbers]

  Description        [the number of rotated bits should be in the range 
  \[0, width-1\].]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_left_rotate(WordNumber_ptr v, int numberOfBits)
{
  unsigned long long l;

  WORD_NUMBER_CHECK_INSTANCE(v); 
  nusmv_assert(v->width > numberOfBits && numberOfBits >= 0);
  
  /* create a constant of 'width' number of 1 bits.
     The left shifts are used because in C shift by a full width is not allowed
  */
  l = ~ (((~ 0ULL) << (v->width - 1)) << 1);
  
  return word_number_create(( (v->value << numberOfBits) 
			    | (v->value >> (v->width - numberOfBits))
			    ) & l,
			    v->width, (char*)NULL);
}


/**Function********************************************************************

  Synopsis           [performs sign extend, i.e. concatenates 'numberOfTimes'
  number of times the highest bit of v with v.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr WordNumber_sign_extend(WordNumber_ptr v, int numberOfTimes)
{
  unsigned long long highestBit;
  unsigned long long value;
  int newWidth;

  WORD_NUMBER_CHECK_INSTANCE(v); 
  nusmv_assert(v->width + numberOfTimes < wordNumberMaxWidth);
  
  /* optimisation */
  if (0 == numberOfTimes) return v;

  highestBit = WordNumber_get_bit(v, v->width - 1);
  highestBit <<= v->width;

  newWidth = v->width + numberOfTimes;

  for (value = v->value; numberOfTimes > 0; --numberOfTimes) {
    value |= highestBit;
    highestBit <<= 1;
  }

  return word_number_create(value, newWidth, (char*)NULL);
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The constructor of WordNumber_ptr. Also adds the created
  number to the memory manager]

  Description        [In case of any errors, returns NULL.
  This function can be invoked only by constructor-converter functions.
  The function creates a copy of the parameter parsedString.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
WordNumber_ptr word_number_create(unsigned long long value, int width, 
				  char* parsedString)
{
  WordNumber_ptr word;
  node_ptr node;

  nusmv_assert(width > 0 && width <= wordNumberMaxWidth); /* implementation limit */

  /* the number is out of the range for a given width.
  NB: Two shifts are performed since shift by the full width isn't allowed in C
  */
  nusmv_assert(((value >> (width - 1)) >> 1) == 0); 
  
  /* there is a string from a parser => not-sharing-memory number */
  if (NULL != parsedString) {
    word = ALLOC(WordNumber, 1);
    if (WORD_NUMBER(NULL) == word) return WORD_NUMBER(NULL);
    
    word->value = value;
    word->width = width;
    word->parsedString = find_string(parsedString);
    
    memoryManager.numberList = cons((node_ptr)word, memoryManager.numberList);
    return word;
  }

  /* usual number => sharing-memory number */
  node = find_node((int)value,
		   NODE_FROM_INT((int)(value>>(wordNumberMaxWidth/2))),
		   NODE_FROM_INT(width));
  word = WORD_NUMBER(find_assoc(memoryManager.hashTable, node));
  
  if (WORD_NUMBER(NULL) != word) return word;
  
  /* there is no such WordNumber => create it */
  word = ALLOC(WordNumber, 1);
  if (WORD_NUMBER(NULL) == word) return WORD_NUMBER(NULL);
    
  word->value = value;
  word->width = width;
  word->parsedString = (string_ptr)NULL;
  
  insert_assoc(memoryManager.hashTable, node, (node_ptr)word);
  return word;
}
 

/**Function********************************************************************

  Synopsis           [Destructor of a WordNumber_ptr]

  Description        [Destructor can be invoked only by the class
  deinitializer, when all the WordNumber_ptr numbers are destroyed.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void word_number_destroy(WordNumber_ptr word)
{
  FREE(word);
}
 

/*  === THIS FUNCTION IF FROM PARSER. ===
    === IT CONVERTS PARSED STRING TO AN ARBITRARY-PRECISION WORD CONSTANT ===
*/
/* /\**Function******************************************************************** */

/*   Synopsis           [The function analyses a word constant, checkes for */
/*   errors and returns a normalised string.] */


/*   Description        [Normalised string is the  */
/*   binary representation of the original value with 0 at the beginning to */
/*   make the width of the string equal to the width of the constant. */
/*   In case of errors the function rpterr is used.] */

/*   SideEffects        [the input string is modified!] */

/*   SeeAlso            [] */

/* ******************************************************************************\/ */
/* static char* nusmv_normilise_word_constant(char* inputString) */
/* { */
/*   char* str = inputString; */
/*   char* tmp; */
/*   int width = 0;  */
/*   int base; */
/*   int baseWidth; */

/*   static int nusmv_word_buffer_size = 0; */
/*   static char* nusmv_word_buffer = (char*) Nil; */

/*   /\* 1. move past the optional width specifier *\/ */
/*   while (isdigit(*str)) ++str; */

/*   /\* 2. move past the the base specifier *\/ */
/*   switch (toupper(*str)) { */
/*   case 'B': base = 2;  baseWidth = 1; break; */
/*   case 'O': base = 8;  baseWidth = 3; break; */
/*   case 'D': base = 10; baseWidth = -1; break; */
/*   case 'H': base = 16; baseWidth = 4; break; */
/*   default: nusmv_assert(false); */
/*   } /\* switch *\/ */
/*   str += 2; */
 
/*   /\* 3. calculate the width (in the original string) *\/ */
/*   if (1 == sscanf(inputString,"%d",&width)) { */
/*     /\* there is a width specifier *\/ */
/*     if (0 == width) rpterr("specified zero width of a word constant"); */
/*   } */
/*   else { /\* there is no width specifier => calculate it *\/ */
/*     for (tmp = str; '\0' != *tmp; ++tmp) if (isxdigit(*tmp)) ++width; */

/*     if (2 == base) {/\* nothing *\/} */
/*     else if (8 == base) width *= 3; */
/*     else if (16 == base) width *= 4; */
/*     else { */
/*       nusmv_assert(10 == base); */
/*       rpterr("decimal word constant without width specifier"); */
/*     } */
/*   } /\* if else *\/ */

/*   /\* 4. remove '_' from the string and make characters upper case*\/ */
/*   for (tmp = inputString; '\0' != *str; ++str) { */
/*      if ('_' != *str) { */
/*        *tmp = toupper(*str); */
/*        ++tmp; */
/*      } */
/*    } /\* for *\/ */
/*    *tmp = '\0'; */

/*    /\* 4. normalise the value string:  convert it to the binary */
/*       representation and add '0's at the beginning to make  */
/*       the width correct. */
/*    *\/ */
/*    /\* Decimal base is a special case (because we do not have */
/*       arbitrary length arithmetic). So at first we convert it to hexadecimal */
/*       one with the standard library functions. */
/*    *\/ */
/*    if (10 == base) { */
/*      long l; */
/*      l = strtol(inputString, &str, 10); */
/*      if (LONG_MAX == l) { /\* overflow *\/ */
/*        rpterr("decimal word constant is too big " */
/* 	      "(at the moment, the implementation forces decimal" */
/* 	      " word constant width to be less than %i bits)", */
/* 	      sizeof(long) * 8);  */
/*        /\* The problem caused by the fact that we do not */
/*        have arbitrary length arithmetic library. Therefore we */
/*        cannot convert big decimal number to binary ones. */
/*        BTW: 'long long' does not work because LLONG_MAX is not defined. */
/*        NB: The max width limit is the implementation-dependent  */
/*        (and not accurate) information. */
/*        *\/ */
/*      } */
/*      if (str != tmp) { */
/*        /\* the most likely reason of the problem is  */
/*        non-digit character in the string. */
/*        *\/ */
/*        rpterr("problem in a decimal word constant " */
/* 	      "(maybe, incorrect digit character)"); */
/*      } */
/*      sprintf(inputString, "%lX", l); /\* hexadecimal long (upper case) *\/ */
/*      base = 16; */
/*      baseWidth = 4;  */
/*    } /\* if (10 == base) *\/ */

/*   /\* make the buffer *\/ */
/*   if (nusmv_word_buffer_size <= width) { /\* reallocate buffer*\/ */
/*     nusmv_word_buffer_size = width + 1; */
/*     nusmv_word_buffer = REALLOC(char, nusmv_word_buffer, nusmv_word_buffer_size); */
/*   } */

/*   /\* convert binary, octal and hexadecimal strings to the  */
/*      binary representation  */
/*   *\/ */
/*   nusmv_assert( (2 == base && 1 == baseWidth) || /\* remove this assert later *\/ */
/* 		(8 == base && 3 == baseWidth) || */
/* 		(16 == base && 4 == baseWidth)); */
/*   { */
/*     char* in; */
/*     char* out; */
    
/*     /\* in = end of input string *\/ */
/*     in = inputString; */
/*     while ('\0' != *in) ++in; */
/*     --in; */
/*     /\* out = end of output string *\/ */
/*     out = nusmv_word_buffer + width; */
/*     *out = '\0'; */
/*     --out; */
/*     while (in >= inputString && out >= nusmv_word_buffer) { */
/*       long int i = strtol(in, (char**)Nil, 16); */
/*       int steps = baseWidth;; */

/*       if (i >= base) rpterr("a digit does not corresond to the constant's base"); */
/*       while (steps > 0 && out >= nusmv_word_buffer) { */
/* 	*out = i & 1 ? '1' : '0'; */
/* 	i >>= 1; */
/* 	--steps; */
/* 	--out; */
/*      } */
/*       if (i!=0) rpterr("a word constant does not fit into the specified width"); */

/*       *in = '\0'; */
/*       --in; */
/*     } /\* while *\/ */
/*     /\* remove the remaining 0s *\/ */
/*     while (in >= inputString && '0' == *in) --in; */
/*     if (in >= inputString) { */
/*       rpterr("a word constant does not fit into the specified width."); */
/*     } */
/*     while (out >= nusmv_word_buffer) { */
/*       *out = '0'; */
/*       --out; */
/*     } */
/*   } */
/*   return  nusmv_word_buffer; */
/* } */

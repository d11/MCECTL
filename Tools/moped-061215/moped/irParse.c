/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         irparse
#define yylex           irlex
#define yyerror         irerror
#define yylval          irlval
#define yychar          irchar
#define yydebug         irdebug
#define yynerrs         irnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "irParse.y"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "alloc.h"
#include "irInt.h"

int irlinenumber = 1;
int int_bits = 0;
int sgnint_bits = 0;
IrModule current_module = NULL;
IrModule called_module = NULL;
wIdent last_var = 0;
int shared_stmt = 0;
int else_cnt = 0;

int irlex();
void irerror (char*);



/* Line 189 of yacc.c  */
#line 104 "irParse.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DEFINE = 258,
     IDENT = 259,
     BVAR = 260,
     NUMBER = 261,
     DEFAULT_INT_BITS = 262,
     DEFAULT_SIGNED_INT_BITS = 263,
     BOOL = 264,
     INT = 265,
     SGNINT = 266,
     ENUM = 267,
     STRUCT = 268,
     INIT = 269,
     ARROW = 270,
     IF = 271,
     FI = 272,
     ELSE = 273,
     DO = 274,
     OD = 275,
     DOUBLECOLON = 276,
     CONDARROW = 277,
     PDS_LGC_AND = 278,
     PDS_LGC_OR = 279,
     PDS_EQV = 280,
     PDS_LGC_XOR = 281,
     PDS_LGC_NOT = 282,
     PDS_FORALL = 283,
     PDS_EXISTS = 284,
     TOK_TRUE = 285,
     TOK_FALSE = 286,
     PDS_PLUS = 287,
     PDS_MINUS = 288,
     PDS_MULT = 289,
     PDS_DIV = 290,
     PDS_SHL = 291,
     PDS_SHR = 292,
     PDS_AND = 293,
     PDS_OR = 294,
     PDS_XOR = 295,
     QVAR = 296,
     PDS_LE = 297,
     PDS_CMPEQ = 298,
     PDS_NE = 299,
     PDS_GE = 300,
     PDS_EQ = 301,
     PRIME = 302,
     GOTO = 303,
     BREAK = 304,
     RETURN = 305,
     SKIP = 306,
     MODULE = 307,
     VOID = 308,
     LABEL = 309,
     UNDEF = 310,
     FORK = 311,
     SHARED = 312
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 25 "irParse.y"

	char *string;
	int integer;
	wIdent name;
	IrDim y_dim;
	IrQvar qvar;
	IrVar y_var;
	IrEnum y_enum;
	IrStruct y_struct;
	IrVarList y_varlist;
	IrName y_name;
	IrFnode y_fnode;
	IrStmt y_stmt;
	IrRule y_rule;
	IrIfdo y_ifdo;
	IrExprList y_exprlist;
	IrModule y_module;



/* Line 214 of yacc.c  */
#line 218 "irParse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 230 "irParse.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   710

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  177
/* YYNRULES -- Number of states.  */
#define YYNSTATES  360

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   312

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      58,    59,     2,     2,    63,     2,    69,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    66,    60,
      67,     2,    68,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    65,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    61,     2,    62,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,     9,    14,    19,    24,    25,    28,
      32,    35,    38,    46,    50,    54,    58,    63,    68,    73,
      75,    80,    82,    84,    88,    92,    93,    95,    96,    98,
     101,   105,   111,   112,   116,   123,   130,   132,   136,   138,
     141,   145,   147,   150,   151,   155,   156,   159,   162,   166,
     168,   172,   175,   178,   181,   184,   185,   187,   198,   208,
     217,   218,   220,   224,   228,   229,   232,   237,   242,   246,
     250,   254,   258,   262,   266,   269,   273,   277,   279,   281,
     283,   285,   292,   296,   300,   304,   308,   312,   316,   320,
     324,   328,   332,   336,   340,   344,   348,   352,   356,   358,
     360,   362,   364,   368,   370,   375,   383,   385,   388,   392,
     394,   398,   400,   405,   413,   415,   418,   422,   426,   430,
     434,   438,   442,   446,   450,   454,   458,   462,   464,   466,
     468,   471,   476,   481,   483,   487,   489,   492,   495,   500,
     501,   503,   505,   509,   511,   513,   515,   518,   521,   522,
     525,   527,   531,   535,   539,   543,   547,   551,   555,   559,
     565,   572,   573,   576,   578,   581,   585,   589,   593,   597,
     598,   600,   602,   606,   609,   612,   615,   619
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      71,     0,    -1,    72,    73,    87,    90,    -1,    -1,    72,
       3,     4,   110,    -1,    72,     3,     7,   110,    -1,    72,
       3,     8,   110,    -1,    -1,    73,    75,    -1,    73,    57,
      75,    -1,    73,    84,    -1,    73,    74,    -1,    52,   124,
       4,    58,   125,    59,    60,    -1,     9,    78,    60,    -1,
      10,    78,    60,    -1,    11,    78,    60,    -1,    12,    76,
      78,    60,    -1,    13,    77,    78,    60,    -1,    80,    61,
      85,    62,    -1,     4,    -1,    80,    61,    86,    62,    -1,
       4,    -1,    79,    -1,    78,    63,    79,    -1,     4,    81,
      83,    -1,    -1,     4,    -1,    -1,    82,    -1,    82,    82,
      -1,    64,   110,    65,    -1,    64,   110,    63,   110,    65,
      -1,    -1,    58,   110,    59,    -1,    12,    80,    61,    85,
      62,    60,    -1,    13,    80,    61,    86,    62,    60,    -1,
       4,    -1,     4,    63,    85,    -1,    75,    -1,    86,    75,
      -1,    14,    88,    60,    -1,     4,    -1,     4,    88,    -1,
      -1,    58,   100,    59,    -1,    -1,    91,    90,    -1,   121,
      90,    -1,     4,    66,    92,    -1,    92,    -1,    94,    95,
      89,    -1,    96,    93,    -1,    97,    93,    -1,   112,    60,
      -1,   119,    60,    -1,    -1,    60,    -1,     4,    67,     4,
      68,    15,     4,    67,     4,     4,    68,    -1,     4,    67,
       4,    68,    15,     4,    67,     4,    68,    -1,     4,    67,
       4,    68,    15,     4,    67,    68,    -1,    -1,    54,    -1,
      16,    98,    17,    -1,    19,    98,    20,    -1,    -1,    99,
      98,    -1,    21,   100,    22,    90,    -1,    21,    18,    22,
      90,    -1,    58,   100,    59,    -1,   100,    23,   100,    -1,
     100,    24,   100,    -1,   100,    26,   100,    -1,   100,    25,
     100,    -1,   100,    43,   100,    -1,    27,   100,    -1,    28,
     101,   100,    -1,    29,   101,   100,    -1,   107,    -1,   102,
      -1,    30,    -1,    31,    -1,     4,    58,   110,    63,   110,
      59,    -1,   103,    67,   103,    -1,   103,    42,   103,    -1,
     103,    43,   103,    -1,   103,    44,   103,    -1,   103,    45,
     103,    -1,   103,    68,   103,    -1,    58,   103,    59,    -1,
     103,    32,   103,    -1,   103,    33,   103,    -1,   103,    34,
     103,    -1,   103,    35,   103,    -1,   103,    36,   103,    -1,
     103,    37,   103,    -1,   103,    38,   103,    -1,   103,    39,
     103,    -1,   103,    40,   103,    -1,   104,    -1,    41,    -1,
       6,    -1,   105,    -1,   105,    69,   105,    -1,   106,    -1,
     106,    64,   103,    65,    -1,   106,    64,   103,    65,    64,
     103,    65,    -1,     4,    -1,     4,    47,    -1,     4,    47,
      47,    -1,   108,    -1,   105,    69,   108,    -1,   109,    -1,
     109,    64,   103,    65,    -1,   109,    64,   103,    65,    64,
     103,    65,    -1,     5,    -1,     5,    47,    -1,     5,    47,
      47,    -1,   110,    32,   110,    -1,   110,    33,   110,    -1,
     110,    34,   110,    -1,   110,    35,   110,    -1,   110,    36,
     110,    -1,   110,    37,   110,    -1,   110,    38,   110,    -1,
     110,    39,   110,    -1,   110,    40,   110,    -1,    58,   110,
      59,    -1,     6,    -1,   100,    -1,   103,    -1,   113,   118,
      -1,   104,    46,   113,   118,    -1,   107,    46,   113,   118,
      -1,   117,    -1,    48,     4,   118,    -1,    49,    -1,    51,
      89,    -1,    56,   113,    -1,     4,    58,   114,    59,    -1,
      -1,   115,    -1,   116,    -1,   116,    63,   115,    -1,   111,
      -1,    55,    -1,    50,    -1,    50,   100,    -1,    50,   103,
      -1,    -1,    63,   119,    -1,   120,    -1,   120,    63,   119,
      -1,   104,    46,   103,    -1,   107,    46,   100,    -1,   104,
      46,    55,    -1,   107,    46,    55,    -1,   104,    44,   103,
      -1,    28,   101,   120,    -1,    29,   101,   120,    -1,   122,
      61,   123,    90,    62,    -1,    52,   124,     4,    58,   125,
      59,    -1,    -1,   123,    75,    -1,    53,    -1,     9,    81,
      -1,    10,    81,    83,    -1,    11,    81,    83,    -1,    12,
       4,    81,    -1,    13,     4,    81,    -1,    -1,   126,    -1,
     127,    -1,   127,    63,   126,    -1,     9,    79,    -1,    10,
      79,    -1,    11,    79,    -1,    12,     4,    79,    -1,    13,
       4,    79,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   135,   135,   141,   142,   146,   149,   154,   155,   161,
     174,   175,   178,   185,   192,   202,   213,   222,   238,   244,
     253,   259,   268,   273,   280,   285,   286,   289,   290,   291,
     297,   301,   308,   309,   315,   319,   325,   328,   333,   338,
     361,   366,   369,   374,   375,   378,   379,   397,   419,   423,
     428,   434,   437,   440,   441,   451,   452,   455,   465,   468,
     473,   474,   477,   486,   495,   498,   504,   514,   528,   529,
     532,   535,   538,   541,   544,   547,   553,   559,   560,   561,
     564,   569,   581,   584,   587,   590,   593,   596,   601,   604,
     607,   610,   613,   616,   619,   622,   625,   628,   631,   634,
     637,   642,   662,   699,   702,   706,   713,   717,   721,   727,
     747,   783,   786,   790,   797,   800,   803,   808,   809,   810,
     811,   817,   818,   819,   820,   821,   822,   823,   826,   827,
     830,   839,   857,   875,   879,   886,   889,   892,   927,   941,
     942,   945,   948,   953,   956,   961,   966,   975,   986,   989,
     994,   997,  1006,  1092,  1113,  1166,  1171,  1176,  1183,  1192,
    1227,  1246,  1247,  1258,  1261,  1265,  1272,  1280,  1293,  1314,
    1315,  1318,  1321,  1326,  1332,  1344,  1357,  1371
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFINE", "IDENT", "BVAR", "NUMBER",
  "DEFAULT_INT_BITS", "DEFAULT_SIGNED_INT_BITS", "BOOL", "INT", "SGNINT",
  "ENUM", "STRUCT", "INIT", "ARROW", "IF", "FI", "ELSE", "DO", "OD",
  "DOUBLECOLON", "CONDARROW", "PDS_LGC_AND", "PDS_LGC_OR", "PDS_EQV",
  "PDS_LGC_XOR", "PDS_LGC_NOT", "PDS_FORALL", "PDS_EXISTS", "TOK_TRUE",
  "TOK_FALSE", "PDS_PLUS", "PDS_MINUS", "PDS_MULT", "PDS_DIV", "PDS_SHL",
  "PDS_SHR", "PDS_AND", "PDS_OR", "PDS_XOR", "QVAR", "PDS_LE", "PDS_CMPEQ",
  "PDS_NE", "PDS_GE", "PDS_EQ", "PRIME", "GOTO", "BREAK", "RETURN", "SKIP",
  "MODULE", "VOID", "LABEL", "UNDEF", "FORK", "SHARED", "'('", "')'",
  "';'", "'{'", "'}'", "','", "'['", "']'", "':'", "'<'", "'>'", "'.'",
  "$accept", "automaton", "defines", "gbldecls", "moduledecl", "vardecl",
  "enumname", "structname", "varlist", "varident", "optident", "optdim",
  "dim", "optbit", "typedecl", "enumlist", "structtypelist", "initconfig",
  "identlist", "optboolexpr", "stmtlist", "lblstmt", "stmt",
  "optsemicolon", "rawtrans", "label", "ifstmt", "dostmt", "clnstmtlist",
  "clnstmt", "boolexpr", "quant", "intcomp", "intexpr", "structvaroptidx",
  "varoptidx", "varoptprime", "structbvaroptidx", "bvaroptidx",
  "bvaroptprime", "cnstexpr", "expr", "action", "callstmt", "args",
  "argslist", "exproptundef", "retstmt", "optassgnlist", "assgnlist",
  "assgn", "module", "modulehead", "localdecls", "rettype", "params",
  "paramlist", "param", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,    40,    41,
      59,   123,   125,    44,    91,    93,    58,    60,    62,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    72,    72,    72,    72,    73,    73,    73,
      73,    73,    74,    75,    75,    75,    75,    75,    76,    76,
      77,    77,    78,    78,    79,    80,    80,    81,    81,    81,
      82,    82,    83,    83,    84,    84,    85,    85,    86,    86,
      87,    88,    88,    89,    89,    90,    90,    90,    91,    91,
      92,    92,    92,    92,    92,    93,    93,    94,    94,    94,
      95,    95,    96,    97,    98,    98,    99,    99,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   101,   102,   102,   102,   102,   102,   102,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   104,   104,   105,   105,   105,   106,   106,   106,   107,
     107,   108,   108,   108,   109,   109,   109,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
     112,   112,   112,   112,   112,   112,   112,   112,   113,   114,
     114,   115,   115,   116,   116,   117,   117,   117,   118,   118,
     119,   119,   120,   120,   120,   120,   120,   120,   120,   121,
     122,   123,   123,   124,   124,   124,   124,   124,   124,   125,
     125,   126,   126,   127,   127,   127,   127,   127
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     0,     4,     4,     4,     0,     2,     3,
       2,     2,     7,     3,     3,     3,     4,     4,     4,     1,
       4,     1,     1,     3,     3,     0,     1,     0,     1,     2,
       3,     5,     0,     3,     6,     6,     1,     3,     1,     2,
       3,     1,     2,     0,     3,     0,     2,     2,     3,     1,
       3,     2,     2,     2,     2,     0,     1,    10,     9,     8,
       0,     1,     3,     3,     0,     2,     4,     4,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     1,     1,     1,
       1,     6,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     1,     3,     1,     4,     7,     1,     2,     3,     1,
       3,     1,     4,     7,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       2,     4,     4,     1,     3,     1,     2,     2,     4,     0,
       1,     1,     3,     1,     1,     1,     2,     2,     0,     2,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     5,
       6,     0,     2,     1,     2,     3,     3,     3,     3,     0,
       1,     1,     3,     2,     2,     2,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     7,     1,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    25,     0,     0,     0,    11,     8,    10,
      45,   127,     0,     4,     5,     6,    27,     0,    22,     0,
       0,    19,     0,     0,    21,     0,     0,    41,     0,    27,
      27,    27,     0,     0,   163,     0,    25,    25,     9,   106,
     114,    64,    64,     0,     0,     0,   135,   145,    43,     0,
       0,     2,    45,    49,    60,    55,    55,     0,   101,   103,
       0,   109,   111,     0,   148,   133,     0,   150,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,    28,    13,     0,    14,    15,     0,     0,     0,
       0,    42,    40,   164,    32,    32,    27,    27,     0,     0,
       0,   107,   139,     0,     0,   115,     0,     0,    64,     0,
       0,     0,     0,   148,   106,   100,     0,     0,     0,    79,
      80,    99,     0,   146,    78,   147,    98,    77,     0,   136,
       0,     0,   137,    46,    61,    43,    56,    51,    52,     0,
       0,     0,     0,     0,     0,    53,     0,   130,    54,     0,
      47,   161,   126,   117,   118,   119,   120,   121,   122,   123,
     124,   125,     0,     0,    24,    29,    23,    16,    36,     0,
      17,    38,     0,   165,   166,   167,   168,   169,     0,     0,
     108,   144,   128,   129,   143,     0,   140,   141,   106,    48,
       0,   116,     0,     0,     0,    62,    65,    63,     0,     0,
       0,   157,   158,   134,    74,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    50,     0,   156,   101,   106,   154,   152,   148,   102,
     110,     0,   155,   153,   148,     0,   149,   151,    45,     0,
      30,     0,     0,    18,    20,    39,     0,     0,     0,     0,
       0,     0,   170,   171,     0,     0,   138,     0,     0,    45,
      45,     0,     0,     0,    75,    76,    68,    88,    69,    70,
      72,    71,    73,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    83,    84,    85,    86,    82,    87,    44,   169,
       0,     0,   131,   104,   132,   112,   162,     0,     0,    33,
      37,    34,    35,   173,   174,   175,     0,     0,     0,     0,
      18,    20,   142,     0,    67,    66,     0,     0,     0,     0,
     159,    31,   176,   177,    12,   172,     0,     0,   160,     0,
       0,     0,    81,   105,   113,     0,    59,     0,    58,    57
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,    17,   181,    32,    35,    27,    28,
      33,    91,    92,   174,    19,   179,   182,    20,    38,   139,
      61,    62,    63,   147,    64,   145,    65,    66,   117,   118,
     192,   121,   134,   204,   136,    68,    69,   137,    71,    72,
      23,   194,    73,    74,   195,   196,   197,    75,   157,    76,
      77,    78,    79,   258,    45,   271,   272,   273
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -178
static const yytype_int16 yypact[] =
{
    -178,    24,    53,  -178,    51,   352,     2,     2,     2,    59,
      59,    59,    63,    93,   103,   119,   276,  -178,  -178,  -178,
     398,  -178,     2,   661,   661,   661,    55,   -44,  -178,   -42,
      95,    83,    59,   166,    83,    59,   170,   103,    64,    55,
      55,    55,   161,   188,  -178,   229,    63,    93,  -178,   -19,
     200,   228,   228,   253,   253,   254,  -178,   347,   206,   119,
     273,  -178,   398,  -178,   224,   232,   232,   110,   259,   233,
     284,  -178,   268,   274,   270,  -178,   275,   278,   398,   281,
     587,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   285,    55,  -178,    59,  -178,  -178,   100,   340,   101,
     276,  -178,  -178,  -178,   285,   285,    55,    55,   288,   286,
     293,   302,   112,   424,   351,   310,   309,   341,   228,   339,
     321,    33,    33,   270,   333,  -178,   347,   253,   253,  -178,
    -178,  -178,   347,   250,  -178,   460,  -178,  -178,   347,  -178,
     356,   323,  -178,  -178,  -178,   206,  -178,  -178,  -178,    94,
     104,   133,    94,   210,    94,  -178,    33,  -178,  -178,    33,
    -178,  -178,  -178,   134,   134,  -178,  -178,   185,   185,   336,
     498,   386,   474,     2,  -178,  -178,  -178,  -178,   319,   322,
    -178,  -178,    32,  -178,  -178,  -178,  -178,   313,   340,   276,
    -178,  -178,   250,   460,  -178,   324,  -178,   326,    -1,  -178,
     328,  -178,   368,   220,   460,  -178,  -178,  -178,     2,   186,
     353,  -178,  -178,  -178,  -178,   347,   347,     6,   423,   347,
     347,   347,   347,   347,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    10,
     342,  -178,    94,   670,   337,    76,  -178,   670,   270,  -178,
    -178,   483,  -178,   250,   270,   517,  -178,  -178,   382,     2,
    -178,   615,   340,   348,   355,  -178,    59,    59,    59,   393,
     403,   357,  -178,   362,   373,   198,  -178,   112,   421,   398,
     398,   578,   116,   290,  -178,  -178,  -178,  -178,    46,   236,
    -178,   150,  -178,   178,   178,  -178,  -178,   248,   248,   508,
     552,   451,   670,   670,   670,   670,   670,   670,  -178,   313,
     624,   433,  -178,   375,  -178,   377,  -178,   380,   526,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,    59,    59,   384,   313,
    -178,  -178,  -178,   441,  -178,  -178,     2,   392,    94,    94,
    -178,  -178,  -178,  -178,  -178,  -178,   397,   633,  -178,   535,
     569,    22,  -178,  -178,  -178,    23,  -178,   401,  -178,  -178
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -178,  -178,  -178,  -178,  -178,     9,  -178,  -178,   115,   -71,
       4,   -34,   378,   131,  -178,  -177,   282,  -178,   439,   332,
     -58,  -178,   365,   413,  -178,  -178,  -178,  -178,   -30,  -178,
     -17,   -41,  -178,   -48,   -10,    74,  -178,    -8,   330,  -178,
      -7,  -178,  -178,   -57,  -178,   247,  -178,  -178,  -121,   -64,
     132,  -178,  -178,  -178,   442,   216,   197,  -178
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -27
static const yytype_int16 yytable[] =
{
      24,    25,   213,   142,   143,   103,   104,   105,    21,   135,
      67,   274,    70,   122,    18,    80,    93,    36,    95,    94,
     160,    94,   119,   176,     3,    48,   355,   357,   111,   219,
     220,   221,   222,   219,   220,   221,   222,   124,    50,   112,
     133,     9,    10,    11,    46,    47,   111,   113,   114,   223,
     109,   110,    67,   223,    70,     6,     4,   112,     7,     8,
      22,    53,    54,    26,   193,   286,   114,    31,    67,   308,
      70,   221,   185,   186,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   218,   320,   215,   216,   206,   223,
     356,   358,   256,   248,   264,   257,   254,    34,   124,   203,
     125,   243,   247,    67,   251,    70,   255,    37,   245,   214,
     125,   209,   209,   210,   210,   217,   124,    50,   125,    90,
     124,   239,   125,   111,   102,    29,    30,   312,    39,    40,
      41,    42,    43,   314,   112,   131,   253,   124,    50,   126,
     127,   128,   129,   130,   -26,   131,   209,    97,   210,   209,
      99,   210,   242,   131,   149,    96,   150,   131,    94,   246,
     177,   180,   242,    94,    94,   106,   261,   191,    83,    84,
     132,   246,    44,   219,   242,   221,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   265,   107,   223,   310,   323,   324,   325,   284,   285,
     317,   281,   288,   289,   290,   291,   292,     9,    10,    11,
      46,    47,   226,   227,   245,    50,   125,    81,    82,    83,
      84,   334,   335,   244,   244,   249,   244,    98,   244,   193,
     149,   100,   282,   108,   247,   183,   184,   126,   127,   128,
     129,   130,   280,   219,   220,   221,   222,   115,    67,   116,
      70,   131,   318,   211,   212,   342,   343,   120,   123,   219,
     331,   221,   222,   223,   138,   252,   253,   316,   132,    67,
      67,    70,    70,   219,   220,   221,   222,   141,   144,   223,
     224,   225,   226,   227,   265,     9,    10,    11,    46,    47,
     349,   350,   146,   223,   124,    50,   125,   152,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   124,    50,   125,   244,   126,   127,   128,
     129,   130,   266,   267,   268,   269,   270,   202,   151,   347,
     153,   131,   154,   156,   155,   158,   126,   127,   128,   129,
     130,   159,   161,   173,   178,   252,   187,   188,   132,   190,
     131,   124,    50,   125,   189,   200,   244,   201,   205,   207,
     240,     9,    10,    11,    12,    13,    14,   132,    81,    82,
      83,    84,    85,    86,   126,   127,   128,   129,   130,   208,
     111,   112,   262,   276,   263,   249,    49,    50,   131,   277,
     279,     9,    10,    11,    46,    47,   278,   326,    51,   283,
     309,    52,    49,    50,    15,   132,   311,   327,   321,    16,
      53,    54,   244,   244,    51,   322,   328,    52,    81,    82,
      83,    84,    85,    86,    87,   329,    53,    54,   198,    50,
      55,    56,    57,    58,    59,   330,   333,   124,    60,   338,
      51,   339,   340,    52,   344,   346,    55,    56,    57,    58,
      59,   348,    53,    54,    60,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   351,   233,   234,   235,   236,   359,
     175,   275,    55,    56,    57,    58,   101,   241,   199,   148,
      60,   250,   287,   224,   225,   226,   227,   228,   229,   230,
     237,   238,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   140,   233,   234,   235,   236,    81,    82,    83,    84,
      85,    86,    87,    88,    89,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   332,   337,   345,   237,   238,     0,
      81,    82,    83,    84,    85,    86,    87,   259,    89,   260,
     224,   225,   226,   227,   228,   229,     0,     0,   313,   224,
     225,   226,   227,   228,   229,   230,   231,   232,    81,    82,
      83,    84,    85,    86,    87,    88,    89,   224,   225,   226,
     227,   228,   229,   230,   231,   232,     0,     0,     0,     0,
       0,     0,   315,     0,   224,   225,   226,   227,   228,   229,
     230,   341,   232,     0,     0,     0,     0,     0,     0,     0,
     353,   224,   225,   226,   227,   228,   229,   230,   231,   232,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    81,
      82,    83,    84,    85,    86,    87,    88,    89,     0,     0,
       0,     0,     0,     0,   354,     0,     0,     0,     0,     0,
       0,   336,     0,     0,     0,     0,   162,    81,    82,    83,
      84,    85,    86,    87,    88,    89,   224,   225,   226,   227,
     228,   229,   230,   231,   232,    81,    82,    83,    84,    85,
      86,    87,    88,    89,   319,     0,     0,     0,     0,     0,
       0,     0,     0,   287,     0,     0,     0,     0,     0,     0,
       0,     0,   352,    81,    82,    83,    84,    85,    86,    87,
      88,    89,   224,   225,   226,   227,   228,   229,   230,   231,
     232
};

static const yytype_int16 yycheck[] =
{
       7,     8,   123,    60,    62,    39,    40,    41,     6,    57,
      20,   188,    20,    54,     5,    22,    60,    13,    60,    63,
      78,    63,    52,    94,     0,    16,     4,     4,    47,    23,
      24,    25,    26,    23,    24,    25,    26,     4,     5,    58,
      57,     9,    10,    11,    12,    13,    47,    66,    67,    43,
      46,    47,    62,    43,    62,     4,     3,    58,     7,     8,
      58,    28,    29,     4,   112,    59,    67,     4,    78,    59,
      78,    25,   106,   107,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,   132,   262,   127,   128,   118,    43,
      68,    68,   156,   150,    62,   159,   153,     4,     4,   116,
       6,   149,   150,   113,   152,   113,   154,     4,     4,   126,
       6,   121,   122,   121,   122,   132,     4,     5,     6,    64,
       4,   138,     6,    47,    60,    10,    11,   248,     9,    10,
      11,    12,    13,   254,    58,    41,   153,     4,     5,    27,
      28,    29,    30,    31,    61,    41,   156,    32,   156,   159,
      35,   159,    58,    41,    44,    60,    46,    41,    63,    55,
      60,    60,    58,    63,    63,     4,   173,    55,    34,    35,
      58,    55,    53,    23,    58,    25,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   182,     4,    43,   242,   266,   267,   268,   215,   216,
     258,   208,   219,   220,   221,   222,   223,     9,    10,    11,
      12,    13,    34,    35,     4,     5,     6,    32,    33,    34,
      35,   279,   280,   149,   150,   151,   152,    61,   154,   277,
      44,    61,    46,     4,   282,   104,   105,    27,    28,    29,
      30,    31,    22,    23,    24,    25,    26,    47,   258,    21,
     258,    41,   259,   121,   122,   326,   327,     4,     4,    23,
      62,    25,    26,    43,    58,    55,   283,   258,    58,   279,
     280,   279,   280,    23,    24,    25,    26,     4,    54,    43,
      32,    33,    34,    35,   275,     9,    10,    11,    12,    13,
     338,   339,    60,    43,     4,     5,     6,    64,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,     4,     5,     6,   242,    27,    28,    29,
      30,    31,     9,    10,    11,    12,    13,    18,    69,   336,
      46,    41,    64,    63,    60,    60,    27,    28,    29,    30,
      31,    63,    61,    58,     4,    55,    58,    61,    58,    47,
      41,     4,     5,     6,    61,     4,   282,    47,    17,    20,
       4,     9,    10,    11,    12,    13,    14,    58,    32,    33,
      34,    35,    36,    37,    27,    28,    29,    30,    31,    58,
      47,    58,    63,    59,    62,   311,     4,     5,    41,    63,
      22,     9,    10,    11,    12,    13,    68,     4,    16,    46,
      58,    19,     4,     5,    52,    58,    69,     4,    60,    57,
      28,    29,   338,   339,    16,    60,    59,    19,    32,    33,
      34,    35,    36,    37,    38,    63,    28,    29,     4,     5,
      48,    49,    50,    51,    52,    62,    15,     4,    56,    64,
      16,    64,    62,    19,    60,     4,    48,    49,    50,    51,
      52,    59,    28,    29,    56,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    67,    42,    43,    44,    45,    68,
      92,   189,    48,    49,    50,    51,    37,   145,   113,    66,
      56,   151,    59,    32,    33,    34,    35,    36,    37,    38,
      67,    68,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    59,    42,    43,    44,    45,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    32,    33,    34,    35,    36,
      37,    38,    39,    40,   277,   309,   329,    67,    68,    -1,
      32,    33,    34,    35,    36,    37,    38,    63,    40,    65,
      32,    33,    34,    35,    36,    37,    -1,    -1,    65,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    -1,    32,    33,    34,    35,    36,    37,
      38,    65,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    -1,    -1,    -1,    -1,    59,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    32,    33,    34,    35,    36,    37,    38,    39,
      40
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    71,    72,     0,     3,    73,     4,     7,     8,     9,
      10,    11,    12,    13,    14,    52,    57,    74,    75,    84,
      87,     6,    58,   110,   110,   110,     4,    78,    79,    78,
      78,     4,    76,    80,     4,    77,    80,     4,    88,     9,
      10,    11,    12,    13,    53,   124,    12,    13,    75,     4,
       5,    16,    19,    28,    29,    48,    49,    50,    51,    52,
      56,    90,    91,    92,    94,    96,    97,   104,   105,   106,
     107,   108,   109,   112,   113,   117,   119,   120,   121,   122,
     110,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      64,    81,    82,    60,    63,    60,    60,    78,    61,    78,
      61,    88,    60,    81,    81,    81,     4,     4,     4,    80,
      80,    47,    58,    66,    67,    47,    21,    98,    99,    98,
       4,   101,   101,     4,     4,     6,    27,    28,    29,    30,
      31,    41,    58,   100,   102,   103,   104,   107,    58,    89,
     124,     4,   113,    90,    54,    95,    60,    93,    93,    44,
      46,    69,    64,    46,    64,    60,    63,   118,    60,    63,
      90,    61,    59,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,    58,    83,    82,    79,    60,     4,    85,
      60,    75,    86,    83,    83,    81,    81,    58,    61,    61,
      47,    55,   100,   103,   111,   114,   115,   116,     4,    92,
       4,    47,    18,   100,   103,    17,    98,    20,    58,   104,
     107,   120,   120,   118,   100,   101,   101,   100,   103,    23,
      24,    25,    26,    43,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    42,    43,    44,    45,    67,    68,   100,
       4,    89,    58,   103,   105,     4,    55,   103,   113,   105,
     108,   103,    55,   100,   113,   103,   119,   119,   123,    63,
      65,   110,    63,    62,    62,    75,     9,    10,    11,    12,
      13,   125,   126,   127,    85,    86,    59,    63,    68,    22,
      22,   110,    46,    46,   100,   100,    59,    59,   100,   100,
     100,   100,   100,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,    59,    58,
     103,    69,   118,    65,   118,    65,    75,    90,   110,    59,
      85,    60,    60,    79,    79,    79,     4,     4,    59,    63,
      62,    62,   115,    15,    90,    90,    63,   125,    64,    64,
      62,    65,    79,    79,    60,   126,     4,   110,    59,   103,
     103,    67,    59,    65,    65,     4,    68,     4,    68,    68
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 135 "irParse.y"
    {
		ir_stmt = (yyvsp[(4) - (4)].y_stmt);
		irLabelUpdate(ir_stmt);
	;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 142 "irParse.y"
    {
		IrDef def = irDefCreate((yyvsp[(3) - (4)].name), (yyvsp[(4) - (4)].integer), ir_def);
		ir_def = def;
	;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 146 "irParse.y"
    {
		int_bits = (yyvsp[(4) - (4)].integer);
	;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 149 "irParse.y"
    {
		sgnint_bits = (yyvsp[(4) - (4)].integer);
	;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 155 "irParse.y"
    {
		IrVarList vl;
		for (vl = (yyvsp[(2) - (2)].y_varlist); vl->next; vl = vl->next) {}
		vl->next = ir_gbl;
		ir_gbl = (yyvsp[(2) - (2)].y_varlist);
	;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 161 "irParse.y"
    {
		IrGbl vl;
		for (vl = (yyvsp[(3) - (3)].y_varlist); vl->next; vl = vl->next) {}
		vl->next = ir_sgbl;
		ir_sgbl = (yyvsp[(3) - (3)].y_varlist);
		
		/* TODO: Insert list of shared global variables (ir_sgbl) 
		to the end of list of global variables (ir_gbl). */
		if (ir_gbl)
			for (vl = ir_gbl; vl->next; vl = vl->next) {}
		else
			ir_gbl = ir_sgbl;
	;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 178 "irParse.y"
    {
		IrModule module = irModuleCreate((yyvsp[(3) - (7)].name), (yyvsp[(2) - (7)].y_var), (yyvsp[(5) - (7)].y_varlist));
		module->next = ir_module;
		ir_module = module;
	;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 185 "irParse.y"
    {
		(yyval.y_varlist) = (yyvsp[(2) - (3)].y_varlist);
		IrVarList vl;
		for (vl = (yyval.y_varlist); vl; vl = vl->next) {
			vl->var->type = IR_BOOL;
		}
	;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 192 "irParse.y"
    {
		(yyval.y_varlist) = (yyvsp[(2) - (3)].y_varlist);
		IrVarList vl;
		for (vl = (yyval.y_varlist); vl; vl = vl->next) {
			vl->var->type = IR_INT;
			if (vl->var->bits) continue;
			if (!int_bits) irerror("unspecified integer range");
			vl->var->bits = int_bits;
		}
	;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 202 "irParse.y"
    {
		irerror("current implementation does not support signed int");
		(yyval.y_varlist) = (yyvsp[(2) - (3)].y_varlist);
		IrVarList vl;
		for (vl = (yyval.y_varlist); vl; vl = vl->next) {
			vl->var->type = IR_SGNINT;
			if (vl->var->bits) continue;
			if (!sgnint_bits) irerror("unspecified integer range");
			vl->var->bits = sgnint_bits;
		}
	;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 213 "irParse.y"
    {
		IrVarList vl;
		for (vl = (yyvsp[(3) - (4)].y_varlist); vl; vl = vl->next) {
			vl->var->type = IR_ENUM;
			vl->var->type_ptr = (yyvsp[(2) - (4)].y_enum);
			vl->var->bits = (yyvsp[(2) - (4)].y_enum)->bits;
		}
		(yyval.y_varlist) = (yyvsp[(3) - (4)].y_varlist);
	;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 222 "irParse.y"
    {
		IrVarList vl;
		IrVarList fields;
		for (vl = (yyvsp[(3) - (4)].y_varlist); vl; vl = vl->next) {
			for (fields = (yyvsp[(2) - (4)].y_struct)->fields; fields; fields = fields->next)
				if (vl->var->dim && fields->var->dim)
					irerror("array of struct of array not allowed");
			vl->var->type = IR_STRUCT;
			vl->var->type_ptr = (yyvsp[(2) - (4)].y_struct);

			irAddStructVar(vl->var);
		}
		(yyval.y_varlist) = (yyvsp[(3) - (4)].y_varlist);
	;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 238 "irParse.y"
    {
		if (!(yyvsp[(3) - (4)].y_name)) irerror("empty enum list");
		if (current_module) 
			irerror("enum declaration found inside a module");
		(yyval.y_enum) = irCreateTypeEnum((yyvsp[(1) - (4)].name), (yyvsp[(3) - (4)].y_name));
	;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 244 "irParse.y"
    {
		IrEnum en = NULL;
		for (en = ir_enum; en; en = en->next)
			if (en->name == (yyvsp[(1) - (1)].name)) break;
		if (!en) irerror("undefined enum type");
		(yyval.y_enum) = en;
	;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 253 "irParse.y"
    {
		if (current_module) 
			irerror("struct declaration found inside a module");
		(yyval.y_struct) = irStructCreate((yyvsp[(1) - (4)].name), (yyvsp[(3) - (4)].y_varlist), ir_struct);
		ir_struct = (yyval.y_struct);
	;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 259 "irParse.y"
    {
		IrStruct s = NULL;
		for (s = ir_struct; s; s = s->next)
			if (s->name == (yyvsp[(1) - (1)].name)) break;
		if (!s) irerror("undefined struct type");
		(yyval.y_struct) = s;
	;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 268 "irParse.y"
    {
		if (irVarGetUnformatted((yyvsp[(1) - (1)].y_var)->name, current_module, called_module))
			irerror("two variables have the same name");
		(yyval.y_varlist) = irVarListCreate((yyvsp[(1) - (1)].y_var), NULL);
	;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 273 "irParse.y"
    {
		if (irVarGetUnformatted((yyvsp[(3) - (3)].y_var)->name, current_module, called_module))
			irerror("two variables have the same name");
		(yyval.y_varlist) = irVarListCreate((yyvsp[(3) - (3)].y_var), (yyvsp[(1) - (3)].y_varlist));
	;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 280 "irParse.y"
    {
		(yyval.y_var) = irVarCreate((yyvsp[(1) - (3)].name), (yyvsp[(2) - (3)].y_dim), (yyvsp[(3) - (3)].integer));
	;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 285 "irParse.y"
    { (yyval.name) = 0; ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 286 "irParse.y"
    { (yyval.name) = (yyvsp[(1) - (1)].name); ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 289 "irParse.y"
    { (yyval.y_dim) = NULL; ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 290 "irParse.y"
    { (yyval.y_dim) = (yyvsp[(1) - (1)].y_dim); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 291 "irParse.y"
    {
		(yyvsp[(1) - (2)].y_dim)->next = (yyvsp[(2) - (2)].y_dim);
		(yyval.y_dim) = (yyvsp[(1) - (2)].y_dim);
	;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 297 "irParse.y"
    {
		if ((yyvsp[(2) - (3)].integer) <= 0) irerror("invalid array size");
		(yyval.y_dim) = irDimCreate((yyvsp[(2) - (3)].integer), 0, (yyvsp[(2) - (3)].integer)-1, NULL);
	;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 301 "irParse.y"
    {
		if ((yyvsp[(2) - (5)].integer) < 0 || (yyvsp[(2) - (5)].integer) > (yyvsp[(4) - (5)].integer))
			irerror("invalid array range");
		(yyval.y_dim) = irDimCreate((yyvsp[(4) - (5)].integer)-(yyvsp[(2) - (5)].integer)+1, (yyvsp[(2) - (5)].integer), (yyvsp[(4) - (5)].integer), NULL)
	;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 308 "irParse.y"
    { (yyval.integer) = 0; ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 309 "irParse.y"
    { 
		if ((yyvsp[(2) - (3)].integer) <= 0) irerror("positive integer expected");
		(yyval.integer) = (yyvsp[(2) - (3)].integer);
	;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 315 "irParse.y"
    {
		if (!(yyvsp[(4) - (6)].y_name)) irerror("empty enum list");
		irCreateTypeEnum((yyvsp[(2) - (6)].name), (yyvsp[(4) - (6)].y_name))
	;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 319 "irParse.y"
    {
		IrStruct st = irStructCreate((yyvsp[(2) - (6)].name), (yyvsp[(4) - (6)].y_varlist), ir_struct);
		ir_struct = st;
	;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 325 "irParse.y"
    {
		(yyval.y_name) = irNameCreate((yyvsp[(1) - (1)].name), NULL);
	;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 328 "irParse.y"
    {
		(yyval.y_name) = irNameCreate((yyvsp[(1) - (3)].name), (yyvsp[(3) - (3)].y_name));
	;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 333 "irParse.y"
    {
		if ((yyvsp[(1) - (1)].y_varlist)->var->type == IR_STRUCT)
			irerror("struct cannot be inside a struct");
		(yyval.y_varlist) = (yyvsp[(1) - (1)].y_varlist);
	;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 338 "irParse.y"
    {
		if ((yyvsp[(2) - (2)].y_varlist)->var->type == IR_STRUCT)
			irerror("struct cannot be inside a struct");
		(yyval.y_varlist) = (yyvsp[(2) - (2)].y_varlist);
		(yyval.y_varlist)->next = (yyvsp[(1) - (2)].y_varlist);
	;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 361 "irParse.y"
    {
		ir_init = irInitCreate(default_state_name, NULL, (yyvsp[(2) - (3)].y_name), NULL);
	;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 366 "irParse.y"
    {
		(yyval.y_name) = irNameCreate((yyvsp[(1) - (1)].name), NULL);
	;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 369 "irParse.y"
    {
		(yyval.y_name) = irNameCreate((yyvsp[(1) - (2)].name), (yyvsp[(2) - (2)].y_name));
	;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 374 "irParse.y"
    { (yyval.y_fnode) = NULL; ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 375 "irParse.y"
    { (yyval.y_fnode) = (yyvsp[(2) - (3)].y_fnode); ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 378 "irParse.y"
    { (yyval.y_stmt) = NULL; ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 379 "irParse.y"
    {
		if ((yyvsp[(1) - (2)].y_stmt)->next) {
			IrStmt stmt;
			for (stmt = (yyvsp[(1) - (2)].y_stmt); stmt->next; stmt = stmt->next)
				;
			stmt->next = (yyvsp[(2) - (2)].y_stmt);
		}
		else {
			(yyvsp[(1) - (2)].y_stmt)->next = (yyvsp[(2) - (2)].y_stmt);
		}

		// check if two labels have the same name
		if (((yyvsp[(1) - (2)].y_stmt)->label > 0) && (irLabelExist((yyvsp[(2) - (2)].y_stmt), (yyvsp[(1) - (2)].y_stmt)->label) > 0)) {
			irerror("two labels have the same name");
		}

		(yyval.y_stmt) = (yyvsp[(1) - (2)].y_stmt);
	;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 397 "irParse.y"
    {
		if ((yyvsp[(1) - (2)].y_stmt)->next) {
			IrStmt stmt;
			// FIXME: error will only be reported at the end of file.
			for (stmt = (yyvsp[(1) - (2)].y_stmt); stmt->next; stmt = stmt->next) {
				if (stmt->label > 0 && irLabelExist((yyvsp[(2) - (2)].y_stmt), stmt->label) > 0)
					irerror("two labels have the same name");
			}
			if (stmt->label > 0 && irLabelExist((yyvsp[(2) - (2)].y_stmt), stmt->label) > 0)
				irerror("two labels have the same name");
			stmt->next = (yyvsp[(2) - (2)].y_stmt);
		}
		else {
			if ((yyvsp[(1) - (2)].y_stmt)->label > 0 && irLabelExist((yyvsp[(2) - (2)].y_stmt), (yyvsp[(1) - (2)].y_stmt)->label) > 0)
				irerror("two labels have the same name");
			(yyvsp[(1) - (2)].y_stmt)->next = (yyvsp[(2) - (2)].y_stmt);
		}

		(yyval.y_stmt) = (yyvsp[(1) - (2)].y_stmt);	
	;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 419 "irParse.y"
    {
		(yyvsp[(3) - (3)].y_stmt)->label = (yyvsp[(1) - (3)].name);
		(yyval.y_stmt) = (yyvsp[(3) - (3)].y_stmt);
	;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 423 "irParse.y"
    {
		(yyval.y_stmt) = (yyvsp[(1) - (1)].y_stmt);
	;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 428 "irParse.y"
    {
		(yyvsp[(1) - (3)].y_rule)->expr = (yyvsp[(3) - (3)].y_fnode);
		(yyval.y_stmt) = irStmtCreate(IR_RAWTRANS, (yyvsp[(1) - (3)].y_rule)->from_stack, (yyvsp[(1) - (3)].y_rule), NULL);
		
		called_module = NULL;
	;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 434 "irParse.y"
    {
		(yyval.y_stmt) = irStmtCreate(IR_IF, 0, (yyvsp[(1) - (2)].y_ifdo), NULL);
	;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 437 "irParse.y"
    {
		(yyval.y_stmt) = irStmtCreate(IR_DO, 0, (yyvsp[(1) - (2)].y_ifdo), NULL);
	;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 440 "irParse.y"
    { (yyval.y_stmt) = (yyvsp[(1) - (2)].y_stmt); ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 441 "irParse.y"
    {
		(yyval.y_stmt) = irStmtCreate(IR_ASSGN, 0, (yyvsp[(1) - (2)].y_exprlist), NULL);
		if (shared_stmt) {
			(yyval.y_stmt)->shared = 1;
			shared_stmt = 0;
		}
	;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 455 "irParse.y"
    {
		(yyval.y_rule) = irRuleCreate((yyvsp[(1) - (10)].name), (yyvsp[(3) - (10)].name), (yyvsp[(6) - (10)].name), (yyvsp[(8) - (10)].name), (yyvsp[(9) - (10)].name));
		
		IrModule module;
		for (module = ir_module; module; module = module->next) {
			if (module->name != (yyvsp[(8) - (10)].name)) continue;
			called_module = module;
			break;
		}
	;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 465 "irParse.y"
    {
		(yyval.y_rule) = irRuleCreate((yyvsp[(1) - (9)].name), (yyvsp[(3) - (9)].name), (yyvsp[(6) - (9)].name), (yyvsp[(8) - (9)].name), 0);
	;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 468 "irParse.y"
    {
		(yyval.y_rule) = irRuleCreate((yyvsp[(1) - (8)].name), (yyvsp[(3) - (8)].name), (yyvsp[(6) - (8)].name), 0, 0);
	;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 473 "irParse.y"
    { (yyval.string) = NULL; ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 474 "irParse.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 477 "irParse.y"
    {
		if (!(yyvsp[(2) - (3)].y_ifdo)) irerror("empty if statement");
		//if (!$2->next && $2->cond->token == IR_ELSE)
		//	irerror("else without other statments");
		(yyval.y_ifdo) = (yyvsp[(2) - (3)].y_ifdo);
		else_cnt = 0;
	;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 486 "irParse.y"
    {
		if (!(yyvsp[(2) - (3)].y_ifdo)) irerror("empty do statement");
		//if (!$2->next && $2->cond->token == IR_ELSE)
		//	irerror("else without other statments");
		(yyval.y_ifdo) = (yyvsp[(2) - (3)].y_ifdo);
		else_cnt = 0;
	;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 495 "irParse.y"
    {
		(yyval.y_ifdo) = NULL;
	;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 498 "irParse.y"
    {
		(yyvsp[(1) - (2)].y_ifdo)->next = (yyvsp[(2) - (2)].y_ifdo);
		(yyval.y_ifdo) = (yyvsp[(1) - (2)].y_ifdo);
	;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 504 "irParse.y"
    {
		if (!(yyvsp[(4) - (4)].y_stmt)) irerror("invalid statement");
		// handles the case ":: cond -> x: ...;" by adding "skip"
		if ((yyvsp[(4) - (4)].y_stmt)->label) {
			IrStmt s = irStmtCreate(IR_SKIP, 0, NULL, NULL);
			s->next = (yyvsp[(4) - (4)].y_stmt);
			(yyvsp[(4) - (4)].y_stmt) = s;
		}
		(yyval.y_ifdo) = irIfdoCreate((yyvsp[(2) - (4)].y_fnode), (yyvsp[(4) - (4)].y_stmt), NULL);
	;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 514 "irParse.y"
    {
		if (!(yyvsp[(4) - (4)].y_stmt)) irerror("invalid statement");
		if (else_cnt) irerror("second else found");
		else_cnt++;
		// handles the case ":: cond -> x: ...;" by adding "skip"
		if ((yyvsp[(4) - (4)].y_stmt)->label) {
			IrStmt s = irStmtCreate(IR_SKIP, 0, NULL, NULL);
			s->next = (yyvsp[(4) - (4)].y_stmt);
			(yyvsp[(4) - (4)].y_stmt) = s;
		}
		(yyval.y_ifdo) = irIfdoCreate(irFnodeCreate(IR_ELSE, 0 ,NULL, NULL), (yyvsp[(4) - (4)].y_stmt), NULL);
	;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 528 "irParse.y"
    { (yyval.y_fnode) = (yyvsp[(2) - (3)].y_fnode); ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 529 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LGC_AND, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 532 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LGC_OR, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 535 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LGC_XOR, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 538 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LGC_EQV, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 541 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_CMPEQ, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 544 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LGC_NOT, 0, NULL, (yyvsp[(2) - (2)].y_fnode));
	;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 547 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_FORALL, 0, (yyvsp[(2) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 553 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_EXISTS, 0, (yyvsp[(2) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 559 "irParse.y"
    { (yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode); ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 560 "irParse.y"
    { (yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode); ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 561 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_TRUE, 0, NULL, NULL);
	;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 564 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_FALSE, 0, NULL, NULL);
	;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 569 "irParse.y"
    {
		if ((yyvsp[(3) - (6)].integer) > (yyvsp[(5) - (6)].integer)) irerror("invalid integer range");
		(yyval.y_fnode) = irNamedFnodeCreate(IR_QUANT, (yyvsp[(1) - (6)].name), 0
			, irFnodeCreate(IR_NUMBER, (yyvsp[(3) - (6)].integer), NULL, NULL)
			, irFnodeCreate(IR_NUMBER, (yyvsp[(5) - (6)].integer), NULL, NULL)
		);
		IrQvar qvar = irQvarCreate((yyvsp[(1) - (6)].name), (yyvsp[(3) - (6)].integer), (yyvsp[(5) - (6)].integer));
		qvar->next = ir_qvar;
		ir_qvar = qvar;
	;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 581 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LT, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 584 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_LE, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 587 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_EQ, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 590 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_NE, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 593 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_GE, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 596 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_GT, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 601 "irParse.y"
    {
		(yyval.y_fnode) = (yyvsp[(2) - (3)].y_fnode);
	;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 604 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_PLUS, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 607 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_MINUS, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 610 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_MULT, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 613 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_DIV, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 616 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_SHL, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 619 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_SHR, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 622 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_AND, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 625 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_OR, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 628 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_XOR, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 631 "irParse.y"
    {
		(yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode);
	;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 634 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_QVAR, (yyvsp[(1) - (1)].qvar)->name, 0, NULL, NULL);
	;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 637 "irParse.y"
    {
		(yyval.y_fnode) = irFnodeCreate(IR_NUMBER, (yyvsp[(1) - (1)].integer), NULL, NULL);
	;}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 642 "irParse.y"
    { 
		IrVar v = irVarGetUnformatted((yyvsp[(1) - (1)].y_fnode)->name, current_module, called_module);
		if (!v) irerror("unknown variable");

		if ((yyvsp[(1) - (1)].y_fnode)->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if ((yyvsp[(1) - (1)].y_fnode)->right && (!v->dim || (v->dim && v->dim->next)))
				irerror("array dimension mismatched");
		}

		if ((yyvsp[(1) - (1)].y_fnode)->left) 
			(yyvsp[(1) - (1)].y_fnode)->right = irTwoDimIndex2Fnode(v, (yyvsp[(1) - (1)].y_fnode)->left, (yyvsp[(1) - (1)].y_fnode)->right);
		else if ((yyvsp[(1) - (1)].y_fnode)->right)
			(yyvsp[(1) - (1)].y_fnode)->right = irFnodeCreate(IR_INDEX, 0, NULL, (yyvsp[(1) - (1)].y_fnode)->right);

		(yyvsp[(1) - (1)].y_fnode)->left = NULL;
		(yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode);
	;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 662 "irParse.y"
    {
		wIdent n = irNameStructCreate((yyvsp[(1) - (3)].y_fnode)->name, (yyvsp[(3) - (3)].y_fnode)->name);
		IrVar v = irVarGet(n, current_module, called_module);
		if (!v) irerror("unknown struct variable");

		if ((yyvsp[(1) - (3)].y_fnode)->right && (yyvsp[(3) - (3)].y_fnode)->right)
			irerror("invalid array index");

		if ((yyvsp[(1) - (3)].y_fnode)->left || (yyvsp[(3) - (3)].y_fnode)->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if (((yyvsp[(1) - (3)].y_fnode)->right || (yyvsp[(1) - (3)].y_fnode)->right) 
					&& (!v->dim || (v->dim && v->dim->next))) 
				irerror("array dimension mismatched");
		}

		IrFnode idx = NULL;
		if ((yyvsp[(1) - (3)].y_fnode)->right && (yyvsp[(3) - (3)].y_fnode)->right)
			irerror("invalid array index");
		if ((yyvsp[(1) - (3)].y_fnode)->left) 
			idx = irTwoDimIndex2Fnode(v, (yyvsp[(1) - (3)].y_fnode)->left, (yyvsp[(1) - (3)].y_fnode)->right);
		else if ((yyvsp[(1) - (3)].y_fnode)->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, (yyvsp[(1) - (3)].y_fnode)->right);
		else if ((yyvsp[(3) - (3)].y_fnode)->left) 
			idx = irTwoDimIndex2Fnode(v, (yyvsp[(3) - (3)].y_fnode)->left, (yyvsp[(3) - (3)].y_fnode)->right);
		else if ((yyvsp[(3) - (3)].y_fnode)->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, (yyvsp[(3) - (3)].y_fnode)->right);

		(yyval.y_fnode) = irNamedFnodeCreate(
			IR_IDENT, n, (yyvsp[(3) - (3)].y_fnode)->value, NULL, NULL);
		(yyval.y_fnode)->right = irFnodeCopy(idx);
		irFnodeFree((yyvsp[(1) - (3)].y_fnode));
		irFnodeFree((yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 699 "irParse.y"
    {
		(yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode);
	;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 702 "irParse.y"
    {
		(yyvsp[(1) - (4)].y_fnode)->right = (yyvsp[(3) - (4)].y_fnode);
		(yyval.y_fnode) = (yyvsp[(1) - (4)].y_fnode);
	;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 706 "irParse.y"
    {
		(yyvsp[(1) - (7)].y_fnode)->left = (yyvsp[(3) - (7)].y_fnode);
		(yyvsp[(1) - (7)].y_fnode)->right = (yyvsp[(6) - (7)].y_fnode);
		(yyval.y_fnode) = (yyvsp[(1) - (7)].y_fnode);
	;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 713 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_IDENT, (yyvsp[(1) - (1)].name), 0, NULL, NULL);
		last_var = (yyvsp[(1) - (1)].name);
	;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 717 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_IDENT, (yyvsp[(1) - (2)].name), 1, NULL, NULL);
		last_var = (yyvsp[(1) - (2)].name);
	;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 721 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_IDENT, (yyvsp[(1) - (3)].name), 2, NULL, NULL);
		last_var = (yyvsp[(1) - (3)].name);
	;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 727 "irParse.y"
    { 
		IrVar v = irVarGetUnformatted((yyvsp[(1) - (1)].y_fnode)->name, current_module, called_module);
		if (!v) irerror("unknown variable");

		if ((yyvsp[(1) - (1)].y_fnode)->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if ((yyvsp[(1) - (1)].y_fnode)->right && (!v->dim || (v->dim && v->dim->next)))
				irerror("array dimension mismatched");
		}	

		if ((yyvsp[(1) - (1)].y_fnode)->left) 
			(yyvsp[(1) - (1)].y_fnode)->right = irTwoDimIndex2Fnode(v, (yyvsp[(1) - (1)].y_fnode)->left, (yyvsp[(1) - (1)].y_fnode)->right);
		else if ((yyvsp[(1) - (1)].y_fnode)->right)
			(yyvsp[(1) - (1)].y_fnode)->right = irFnodeCreate(IR_INDEX, 0, NULL, (yyvsp[(1) - (1)].y_fnode)->right);

		(yyvsp[(1) - (1)].y_fnode)->left = NULL;
		(yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode);
	;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 747 "irParse.y"
    {
		wIdent n = irNameStructCreate((yyvsp[(1) - (3)].y_fnode)->name, (yyvsp[(3) - (3)].y_fnode)->name);
		IrVar v = irVarGet(n, current_module, called_module);
		if (!v) irerror("unknown struct variable");

		if ((yyvsp[(1) - (3)].y_fnode)->right && (yyvsp[(3) - (3)].y_fnode)->right)
			irerror("invalid array index");
		if ((yyvsp[(1) - (3)].y_fnode)->left || (yyvsp[(3) - (3)].y_fnode)->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if (((yyvsp[(1) - (3)].y_fnode)->right || (yyvsp[(1) - (3)].y_fnode)->right) 
					&& (!v->dim || (v->dim && v->dim->next))) 
				irerror("array dimension mismatched");
		}	

		IrFnode idx = NULL;
		if ((yyvsp[(1) - (3)].y_fnode)->right && (yyvsp[(3) - (3)].y_fnode)->right)
			irerror("invalid array index");
		if ((yyvsp[(1) - (3)].y_fnode)->left) 
			idx = irTwoDimIndex2Fnode(v, (yyvsp[(1) - (3)].y_fnode)->left, (yyvsp[(1) - (3)].y_fnode)->right);
		else if ((yyvsp[(1) - (3)].y_fnode)->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, (yyvsp[(1) - (3)].y_fnode)->right);
		else if ((yyvsp[(3) - (3)].y_fnode)->left) 
			idx = irTwoDimIndex2Fnode(v, (yyvsp[(3) - (3)].y_fnode)->left, (yyvsp[(3) - (3)].y_fnode)->right);
		else if ((yyvsp[(3) - (3)].y_fnode)->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, (yyvsp[(3) - (3)].y_fnode)->right);

		(yyval.y_fnode) = irNamedFnodeCreate(
			IR_BVAR, n, (yyvsp[(3) - (3)].y_fnode)->value, NULL, NULL);
		(yyval.y_fnode)->right = irFnodeCopy(idx);
		irFnodeFree((yyvsp[(1) - (3)].y_fnode));
		irFnodeFree((yyvsp[(3) - (3)].y_fnode));
	;}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 783 "irParse.y"
    {
		(yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode);
	;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 786 "irParse.y"
    {
		(yyvsp[(1) - (4)].y_fnode)->right = (yyvsp[(3) - (4)].y_fnode);
		(yyval.y_fnode) = (yyvsp[(1) - (4)].y_fnode);
	;}
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 790 "irParse.y"
    {
		(yyvsp[(1) - (7)].y_fnode)->left = (yyvsp[(3) - (7)].y_fnode);
		(yyvsp[(1) - (7)].y_fnode)->right = (yyvsp[(6) - (7)].y_fnode);
		(yyval.y_fnode) = (yyvsp[(1) - (7)].y_fnode);
	;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 797 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_BVAR, (yyvsp[(1) - (1)].name), 0, NULL, NULL);
	;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 800 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_BVAR, (yyvsp[(1) - (2)].name), 1, NULL, NULL);
	;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 803 "irParse.y"
    {
		(yyval.y_fnode) = irNamedFnodeCreate(IR_BVAR, (yyvsp[(1) - (3)].name), 2, NULL, NULL);
	;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 808 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) + (yyvsp[(3) - (3)].integer); ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 809 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) - (yyvsp[(3) - (3)].integer); ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 810 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) * (yyvsp[(3) - (3)].integer); ;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 811 "irParse.y"
    { 
		if ((yyvsp[(3) - (3)].integer) == 0) {
			irerror("division by zero");
		}
		(yyval.integer) = (yyvsp[(1) - (3)].integer) / (yyvsp[(3) - (3)].integer); 
	;}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 817 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) << (yyvsp[(3) - (3)].integer); ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 818 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) >> (yyvsp[(3) - (3)].integer); ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 819 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) & (yyvsp[(3) - (3)].integer); ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 820 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) | (yyvsp[(3) - (3)].integer); ;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 821 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) ^ (yyvsp[(3) - (3)].integer); ;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 822 "irParse.y"
    { (yyval.integer) = (yyvsp[(2) - (3)].integer); ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 823 "irParse.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].integer); ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 826 "irParse.y"
    { (yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode); ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 827 "irParse.y"
    { (yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode); ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 830 "irParse.y"
    {
		irUpdateCallExprList((yyvsp[(1) - (2)].y_stmt), (yyvsp[(2) - (2)].y_exprlist));
		if (shared_stmt) {
			(yyvsp[(1) - (2)].y_stmt)->shared = 1;
			shared_stmt = 0;
		}
		(yyval.y_stmt) = (yyvsp[(1) - (2)].y_stmt);
		called_module = NULL;
	;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 839 "irParse.y"
    {
		irUpdateCallExprList((yyvsp[(3) - (4)].y_stmt), (yyvsp[(4) - (4)].y_exprlist));
		if (shared_stmt) {
			(yyvsp[(3) - (4)].y_stmt)->shared = 1;
			shared_stmt = 0;
		}

		char *msg = 0;
		IrStmt stmt = irStmtCreate(IR_ASSGN, 0, NULL, NULL); 
		stmt->content = irCreateRetExprList(2, called_module, 
			current_module, (yyvsp[(1) - (4)].y_fnode), &msg);
		if (msg) irerror(msg);

		if (isSharedGlobal((yyvsp[(1) - (4)].y_fnode)->name)) stmt->shared = 1;
		(yyvsp[(3) - (4)].y_stmt)->next = stmt;
		(yyval.y_stmt) = (yyvsp[(3) - (4)].y_stmt);
		called_module = NULL;
	;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 857 "irParse.y"
    {
		irUpdateCallExprList((yyvsp[(3) - (4)].y_stmt), (yyvsp[(4) - (4)].y_exprlist));
		if (shared_stmt) {
			(yyvsp[(3) - (4)].y_stmt)->shared = 1;
			shared_stmt = 0;
		}

		char *msg = 0;
		IrStmt stmt = irStmtCreate(IR_ASSGN, 0, NULL, NULL);
		stmt->content = irCreateRetExprList(2, called_module, 
			current_module, (yyvsp[(1) - (4)].y_fnode), &msg);
		if (msg) irerror(msg);

		if (isSharedGlobal((yyvsp[(1) - (4)].y_fnode)->name)) stmt->shared = 1;
		(yyvsp[(3) - (4)].y_stmt)->next = stmt;
		(yyval.y_stmt) = (yyvsp[(3) - (4)].y_stmt);
		called_module = NULL;
	;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 875 "irParse.y"
    { 
		if (!current_module) irerror("return must be in module");
		(yyval.y_stmt) = (yyvsp[(1) - (1)].y_stmt);
	;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 879 "irParse.y"
    {
		(yyval.y_stmt) = irStmtCreate(IR_GOTO, 0, irGotoCreate((yyvsp[(2) - (3)].name), (yyvsp[(3) - (3)].y_exprlist)), NULL);
		if (shared_stmt) {
			(yyval.y_stmt)->shared = 1;
			shared_stmt = 0;
		}
	;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 886 "irParse.y"
    {
		(yyval.y_stmt) = irStmtCreate(IR_BREAK, 0, NULL, NULL);
	;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 889 "irParse.y"
    {
		(yyval.y_stmt) = irStmtCreate(IR_SKIP, 0, (yyvsp[(2) - (2)].y_fnode), NULL);
	;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 892 "irParse.y"
    {
		wIdent midName = irNameStackFresh();
		IrCall forked = (IrCall) (yyvsp[(2) - (2)].y_stmt)->content;
		
		IrExprList forkedParam = irExprListCopy(forked->params);
		IrExprList paramPtr = forkedParam;
		IrFnode param;
		for (; paramPtr; paramPtr = paramPtr->next) {
			param = paramPtr->expr;
			if ((param->token == IR_EQ) || (param->token == IR_CMPEQ)) {
				irFnodeFree(param->right);
				param->right = irNamedFnodeCreate(
					IR_IDENT, param->left->name, 0, NULL, NULL);
			} else if (param->token == IR_FORALL) {
				param->right->right->name = param->right->left->name;
			} else {
				irerror("unexpected parameter");
			}
		}
		
		IrFork irfork = irForkCreate(midName, forked);
			
		IrCall midCall = irCallCreate(forked->module, forkedParam);
		IrStmt midStmt = irStmtCreate(IR_CALL, midName, midCall, NULL);
		IrModule midModule = irModuleCopy(forked->module);
		midModule->name = midName;
		midModule->first_stmt = midStmt;
		midModule->last_stmt = midStmt;
		midModule->next = ir_module;
		ir_module = midModule;
		
		(yyval.y_stmt) = irStmtCreate(IR_FORK, 0, irfork, NULL);
	;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 927 "irParse.y"
    {
		IrModule m;
		for (m = ir_module; m; m = m->next) 
			if (m->name == (yyvsp[(1) - (4)].name)) break;
		if (!m) irerror("module not found");
		
		called_module = m;
		char *msg = 0;
		(yyval.y_stmt) = irCreateCallStmt(current_module, m, (yyvsp[(3) - (4)].y_exprlist), &msg);
		if (msg) irerror(msg);
		irExprlistFree((yyvsp[(3) - (4)].y_exprlist));
	;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 941 "irParse.y"
    { (yyval.y_exprlist) = NULL; ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 942 "irParse.y"
    { (yyval.y_exprlist) = (yyvsp[(1) - (1)].y_exprlist); ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 945 "irParse.y"
    {
		(yyval.y_exprlist) = irExprListCreate((yyvsp[(1) - (1)].y_fnode), NULL);
	;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 948 "irParse.y"
    {
		(yyval.y_exprlist) = irExprListCreate((yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_exprlist));
	;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 953 "irParse.y"
    {
		(yyval.y_fnode) = (yyvsp[(1) - (1)].y_fnode);
	;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 956 "irParse.y"
    {
		(yyval.y_fnode) = NULL;
	;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 961 "irParse.y"
    {
		if (current_module->retvar->type != IR_VOID)
			irerror("invalid return type");
		(yyval.y_stmt) = irStmtCreate(IR_RET, 0, NULL, NULL);
	;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 966 "irParse.y"
    {
		if (current_module->retvar->type != IR_BOOL)
			irerror("invalid return type");
		(yyval.y_stmt) = irStmtCreate(IR_RET, 0, NULL, NULL);
		char *msg = 0;
		(yyval.y_stmt)->content = irCreateRetExprList(1, current_module, 
			current_module, (yyvsp[(2) - (2)].y_fnode), &msg);
		if (msg) irerror(msg);
	;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 975 "irParse.y"
    {
		if (current_module->retvar->type == IR_VOID)
			irerror("invalid return type");
		(yyval.y_stmt) = irStmtCreate(IR_RET, 0, NULL, NULL);
		char *msg = 0;
		(yyval.y_stmt)->content = irCreateRetExprList(1, current_module, 
			current_module, (yyvsp[(2) - (2)].y_fnode), &msg);
		if (msg) irerror(msg);	
	;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 986 "irParse.y"
    {
		(yyval.y_exprlist) = NULL;
	;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 989 "irParse.y"
    {
		(yyval.y_exprlist) = (yyvsp[(2) - (2)].y_exprlist);
	;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 994 "irParse.y"
    {
		(yyval.y_exprlist) = (yyvsp[(1) - (1)].y_exprlist);
	;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 997 "irParse.y"
    {
		IrExprList exprlist;
		for (exprlist = (yyvsp[(1) - (3)].y_exprlist); exprlist->next; exprlist = exprlist->next)
			;
		exprlist->next = (yyvsp[(3) - (3)].y_exprlist);
		(yyval.y_exprlist) = (yyvsp[(1) - (3)].y_exprlist);
	;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 1006 "irParse.y"
    {
		
		/* aray = array */
		IrVar lvar = irVarGetUnformatted((yyvsp[(1) - (3)].y_fnode)->name, current_module, called_module);
		IrVar rvar = NULL;
		if ((yyvsp[(3) - (3)].y_fnode)->token == IR_IDENT) 
			rvar = irVarGetUnformatted((yyvsp[(3) - (3)].y_fnode)->name, current_module, called_module);

		if (lvar && lvar->dim && !(yyvsp[(1) - (3)].y_fnode)->right) {

			if (!rvar || !rvar->dim || (yyvsp[(3) - (3)].y_fnode)->right 
					|| irDimEquals(lvar->dim, rvar->dim) == 0)
				irerror("invalid array assignment");

			int i, j;
			irFlattenDim(lvar->dim, &i, &j);
			IrFnode eqnode;
			IrExprList exprlist = NULL;
			if (lvar->type == IR_STRUCT) {
				IrVar slvar = lvar->extra;
				IrVar srvar = rvar->extra;
				while(slvar) {
					int type, op;
					if (slvar->type == IR_BOOL) {
						type = IR_BVAR;
						op = IR_CMPEQ;
					} else {
						type = IR_IDENT;
						op = IR_EQ;
					}
					eqnode = irFnodeAllAssgn(type, op, slvar->name, srvar->name, i, j);
					exprlist = irExprListCreate(eqnode, exprlist);
					slvar = slvar->extra;
					srvar = srvar->extra;
				}
			} else {
				eqnode = irFnodeAllAssgn(IR_IDENT, IR_EQ, lvar->name, rvar->name, i, j);
				exprlist = irExprListCreate(eqnode, NULL);
			}
			(yyval.y_exprlist) = exprlist;
		} 

		/* struct = struct */
		else if (lvar && lvar->type == IR_STRUCT && (yyvsp[(3) - (3)].y_fnode)->token == IR_IDENT) {
			if (rvar && rvar->type == IR_STRUCT) {

				if (lvar->type_ptr != rvar->type_ptr)
					irerror("struct type mismatched");

				IrVar slvar = lvar->extra;
				IrVar srvar = rvar->extra;
				IrFnode eqnode;
				IrExprList exprlist = NULL;
				while(slvar) {
					int type, op;
					if (slvar->type == IR_BOOL) {
						type = IR_BVAR;
						op = IR_CMPEQ;
					} else {
						type = IR_IDENT;
						op = IR_EQ;
					}
					if (slvar->dim) {
						int i, j;
						irFlattenDim(slvar->dim, &i, &j);
						eqnode = irFnodeAllAssgn(type, op, slvar->name, srvar->name, i, j);
					} else {
						IrFnode lnode = irNamedFnodeCreate(type, slvar->name, 1, NULL, NULL);
						IrFnode rnode = irNamedFnodeCreate(type, srvar->name, 0, NULL, NULL);
						eqnode = irFnodeCreate(op, 0, lnode, rnode);
					}
					exprlist = irExprListCreate(eqnode, exprlist);
					slvar = slvar->extra;
					srvar = srvar->extra;
				}

				irFnodeFree((yyvsp[(1) - (3)].y_fnode));
				irFnodeFree((yyvsp[(3) - (3)].y_fnode));
				(yyval.y_exprlist) = exprlist;
			}
		} else {
			(yyvsp[(1) - (3)].y_fnode)->value = 1;
			(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_EQ, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode)), NULL);
			if (isSharedGlobal((yyvsp[(1) - (3)].y_fnode)->name)) shared_stmt = 1;
		}
	;}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 1092 "irParse.y"
    {

		/* aray = array */
		IrVar lvar = irVarGetUnformatted((yyvsp[(1) - (3)].y_fnode)->name, current_module, called_module);
		if (lvar && lvar->dim && !(yyvsp[(1) - (3)].y_fnode)->right && (yyvsp[(3) - (3)].y_fnode)->token == IR_BVAR) {
			IrVar rvar = irVarGetUnformatted((yyvsp[(3) - (3)].y_fnode)->name, current_module, called_module);

			if (!rvar->dim || (yyvsp[(3) - (3)].y_fnode)->right 
					|| irDimEquals(lvar->dim, rvar->dim) == 0)
				irerror("invalid array assignment");

			int i, j;
			irFlattenDim(lvar->dim, &i, &j);
			IrFnode eqnode = irFnodeAllAssgn(IR_BVAR, IR_CMPEQ, lvar->name, rvar->name, i, j);
			(yyval.y_exprlist) = irExprListCreate(eqnode, NULL);
		} else {
			(yyvsp[(1) - (3)].y_fnode)->value = 1;
			(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_CMPEQ, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode)), NULL);
			if (isSharedGlobal((yyvsp[(1) - (3)].y_fnode)->name)) shared_stmt = 1;
		}
	;}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 1113 "irParse.y"
    {
		IrVar var = irVarGetUnformatted((yyvsp[(1) - (3)].y_fnode)->name, current_module, called_module);
		if (!var) var = irVarGet((yyvsp[(1) - (3)].y_fnode)->name, current_module, called_module);

		/* struct = undef */
		if (var && var->type == IR_STRUCT) {
			IrVar sv;
			IrFnode vnode, eq;
			IrExprList exprlist = NULL;
			for (sv = var->extra; sv; sv = sv->extra) {
				vnode = irNamedFnodeCreate(IR_IDENT, sv->name, 1, 
					irFnodeCopy((yyvsp[(1) - (3)].y_fnode)->left),
					irFnodeCopy((yyvsp[(1) - (3)].y_fnode)->right));
				if (sv->type == IR_ENUM) {
					int size = ((IrEnum) sv->type_ptr)->size;
					if (sv->dim) {
						int i,j;
						irFlattenDim(sv->dim, &i, &j);
						eq = irFnodeAllLt(sv->name, i, j, size);
					} else {
						IrFnode snode = irFnodeCreate(IR_NUMBER, size, NULL, NULL);
						eq = irFnodeCreate(IR_LT, 0, vnode, snode);
					}
				} else {
					int token = (sv->type == IR_BOOL) ? IR_CMPEQ : IR_EQ;
					eq = irFnodeCreate(token, 0, vnode, NULL);
				}
				exprlist = irExprListCreate(eq, exprlist);
			}
			(yyval.y_exprlist) = exprlist;
			if (isSharedGlobal((yyvsp[(1) - (3)].y_fnode)->name)) shared_stmt = 1;
			irFnodeFree((yyvsp[(1) - (3)].y_fnode));
		} /* enum = undef */
		else if (var && var->type == IR_ENUM) {
			int size = ((IrEnum) var->type_ptr)->size;
			IrFnode fnode;
			if (var->dim) {
				int i,j;
				irFlattenDim(var->dim, &i, &j);
				fnode = irFnodeAllLt(var->name, i, j, size);
				irFnodeFree((yyvsp[(1) - (3)].y_fnode));
			} else {
				IrFnode snode = irFnodeCreate(IR_NUMBER, size, NULL, NULL);
				(yyvsp[(1) - (3)].y_fnode)->value = 1;
				fnode = irFnodeCreate(IR_LT, 0, (yyvsp[(1) - (3)].y_fnode), snode);
			}
			(yyval.y_exprlist) = irExprListCreate(fnode, NULL);
		} else {
			(yyvsp[(1) - (3)].y_fnode)->value = 1;
			(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_EQ, 0, (yyvsp[(1) - (3)].y_fnode), NULL), NULL);
			if (isSharedGlobal((yyvsp[(1) - (3)].y_fnode)->name)) shared_stmt = 1;
		}
	;}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 1166 "irParse.y"
    {
		(yyvsp[(1) - (3)].y_fnode)->value = 1;
		(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_CMPEQ, 0, (yyvsp[(1) - (3)].y_fnode), NULL), NULL);
		if (isSharedGlobal((yyvsp[(1) - (3)].y_fnode)->name)) shared_stmt = 1;
	;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 1171 "irParse.y"
    {
		(yyvsp[(1) - (3)].y_fnode)->value = 1;
		(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_NE, 0, (yyvsp[(1) - (3)].y_fnode), (yyvsp[(3) - (3)].y_fnode)), NULL);
		if (isSharedGlobal((yyvsp[(1) - (3)].y_fnode)->name)) shared_stmt = 1;
	;}
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 1176 "irParse.y"
    {
		if ((yyvsp[(3) - (3)].y_exprlist)->next) irerror("parallel assignment inside quantification");
		(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_FORALL, 0, (yyvsp[(2) - (3)].y_fnode), (yyvsp[(3) - (3)].y_exprlist)->expr), NULL);
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	;}
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 1183 "irParse.y"
    {
		if ((yyvsp[(3) - (3)].y_exprlist)->next) irerror("parallel assignment inside quantification");
		(yyval.y_exprlist) = irExprListCreate(irFnodeCreate(IR_EXISTS, 0, (yyvsp[(2) - (3)].y_fnode), (yyvsp[(3) - (3)].y_exprlist)->expr), NULL);
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	;}
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 1192 "irParse.y"
    {
		if ((yyvsp[(4) - (5)].y_stmt)) {
			if ((yyvsp[(4) - (5)].y_stmt)->label && ((yyvsp[(4) - (5)].y_stmt)->label != (yyvsp[(1) - (5)].y_module)->name)) {
				(yyvsp[(4) - (5)].y_stmt) = irStmtCreate(IR_SKIP, 0, NULL, (yyvsp[(4) - (5)].y_stmt));
			}
			(yyvsp[(4) - (5)].y_stmt)->label = (yyvsp[(1) - (5)].y_module)->name;
			(yyvsp[(1) - (5)].y_module)->first_stmt = (yyvsp[(4) - (5)].y_stmt);
			IrStmt last_stmt;
			for (last_stmt = (yyvsp[(4) - (5)].y_stmt); last_stmt->next; last_stmt = last_stmt->next)
				;
			
			/* implicit return */
			if ((current_module->retvar->type == IR_VOID)
					&& (last_stmt->type != IR_RET)) {
				IrStmt ret_stmt = irStmtCreate(IR_RET, 0, NULL, NULL);
				last_stmt->next = ret_stmt;
				last_stmt = ret_stmt;
			}
			(yyvsp[(1) - (5)].y_module)->last_stmt = last_stmt;

			/* if the last statement is not a return statment*/
			if (!irIsReturnInside(last_stmt)) {
				irerror("return statement expected");
			}
		} else if (current_module->retvar->type == IR_VOID) {
			(yyvsp[(4) - (5)].y_stmt) = irStmtCreate(IR_RET, (yyvsp[(1) - (5)].y_module)->name, NULL, NULL);
			(yyvsp[(1) - (5)].y_module)->first_stmt = (yyvsp[(4) - (5)].y_stmt);
			(yyvsp[(1) - (5)].y_module)->last_stmt = (yyvsp[(4) - (5)].y_stmt);
		}
		(yyval.y_stmt) = (yyvsp[(4) - (5)].y_stmt);
		
		current_module = NULL; //current_module->next;
	;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 1227 "irParse.y"
    {
		IrModule module;
		
		for (module = ir_module; module; module = module->next) {
			if (module->name == (yyvsp[(3) - (6)].name))
				break;
		}
		
		if (!module) {
			module = irModuleCreate((yyvsp[(3) - (6)].name), (yyvsp[(2) - (6)].y_var), (yyvsp[(5) - (6)].y_varlist));
			module->next = ir_module;
			ir_module = module;
		}
		(yyval.y_module) = module;
		
		current_module = module;
	;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 1247 "irParse.y"
    {
		IrVarList vl;
		for (vl = (yyvsp[(2) - (2)].y_varlist); vl->next; vl = vl->next) 
			;	
		vl->next = current_module->localvar;
		current_module->localvar = (yyvsp[(2) - (2)].y_varlist);
	  ;}
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 1258 "irParse.y"
    { 
		(yyval.y_var) = irVarTypeCreate(IR_VOID);
	;}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 1261 "irParse.y"
    { 
		(yyval.y_var) = irVarTypeCreate(IR_BOOL);
		(yyval.y_var)->dim = (yyvsp[(2) - (2)].y_dim);
	;}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 1265 "irParse.y"
    { 
		(yyval.y_var) = irVarTypeCreate(IR_INT);
		(yyval.y_var)->dim = (yyvsp[(2) - (3)].y_dim);
		if ((yyvsp[(3) - (3)].integer)) (yyval.y_var)->bits = (yyvsp[(3) - (3)].integer);
		else if (int_bits) (yyval.y_var)->bits = int_bits;
		else irerror("unspecified integer range");
	;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 1272 "irParse.y"
    { 
		irerror("current implementation does not support signed int");
		(yyval.y_var) = irVarTypeCreate(IR_SGNINT);
		(yyval.y_var)->dim = (yyvsp[(2) - (3)].y_dim);
		if ((yyvsp[(3) - (3)].integer)) (yyval.y_var)->bits = (yyvsp[(3) - (3)].integer); 
		else if (sgnint_bits) (yyval.y_var)->bits = sgnint_bits;
		else irerror("unspecified integer range");
	;}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 1280 "irParse.y"
    { 
		(yyval.y_var) = irVarTypeCreate(IR_ENUM);
		IrEnum e;
		for (e = ir_enum; e; e = e->next)
			if (e->name == (yyvsp[(2) - (3)].name))
				break;
		if (!e)
			irerror("invalid enum type");
		
		(yyval.y_var)->type_ptr = e;
		(yyval.y_var)->bits = e->bits;
		(yyval.y_var)->dim = (yyvsp[(3) - (3)].y_dim);
	;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 1293 "irParse.y"
    { 
		(yyval.y_var) = irVarTypeCreate(IR_STRUCT);
		IrStruct s;
		for (s = ir_struct; s; s = s->next)
			if (s->name == (yyvsp[(2) - (3)].name))
				break;
		if (s)
			(yyval.y_var)->type_ptr = s;
		else
			irerror("invalid struct type");

		if ((yyvsp[(3) - (3)].y_dim)) {
			IrVarList f = s->fields;
			for (; f; f = f->next)
				if (f->var->dim)
					irerror("array of struct of array not allowed");
			(yyval.y_var)->dim = (yyvsp[(3) - (3)].y_dim);
		}
	;}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 1314 "irParse.y"
    { (yyval.y_varlist) = NULL; ;}
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 1315 "irParse.y"
    { (yyval.y_varlist) = (yyvsp[(1) - (1)].y_varlist); ;}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 1318 "irParse.y"
    {
		(yyval.y_varlist) = irVarListCreate((yyvsp[(1) - (1)].y_var), NULL);
	;}
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 1321 "irParse.y"
    {
		(yyval.y_varlist) = irVarListCreate((yyvsp[(1) - (3)].y_var), (yyvsp[(3) - (3)].y_varlist));
	;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 1326 "irParse.y"
    {
		if (irVarGetUnformatted((yyvsp[(2) - (2)].y_var)->name, NULL, NULL))
			irerror("two variables have the same name");
		(yyvsp[(2) - (2)].y_var)->type = IR_BOOL;
		(yyval.y_var) = (yyvsp[(2) - (2)].y_var);
	;}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 1332 "irParse.y"
    {
		if (irVarGetUnformatted((yyvsp[(2) - (2)].y_var)->name, NULL, NULL))
			irerror("two variables have the same name");
		(yyvsp[(2) - (2)].y_var)->type = IR_INT;
		if (!(yyvsp[(2) - (2)].y_var)->bits) {
			if (!int_bits)
				irerror("unspecified integer range");
			else
				(yyvsp[(2) - (2)].y_var)->bits = int_bits;
		}
		(yyval.y_var) = (yyvsp[(2) - (2)].y_var);
	;}
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 1344 "irParse.y"
    {
		irerror("current implementation does not support signed int");
		if (irVarGetUnformatted((yyvsp[(2) - (2)].y_var)->name, NULL, NULL))
			irerror("two variables have the same name");
		(yyvsp[(2) - (2)].y_var)->type = IR_SGNINT;
		if (!(yyvsp[(2) - (2)].y_var)->bits) {
			if (!sgnint_bits)
				irerror("unspecified integer range");
			else
				(yyvsp[(2) - (2)].y_var)->bits = sgnint_bits;
		}
		(yyval.y_var) = (yyvsp[(2) - (2)].y_var);
	;}
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 1357 "irParse.y"
    {
		if (irVarGetUnformatted((yyvsp[(3) - (3)].y_var)->name, NULL, NULL))
			irerror("two variables have the same name");
		(yyvsp[(3) - (3)].y_var)->type = IR_ENUM;
		IrEnum e;
		for (e = ir_enum; e; e = e->next)
			if (e->name == (yyvsp[(2) - (3)].name))
				break;
		if (!e)
			irerror("invalid enum type");
		else
			(yyvsp[(3) - (3)].y_var)->type_ptr = e;
		(yyval.y_var) = (yyvsp[(3) - (3)].y_var);
	;}
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 1371 "irParse.y"
    {
		if (irVarGetUnformatted((yyvsp[(3) - (3)].y_var)->name, NULL, NULL))
			irerror("two variables have the same name");
		(yyvsp[(3) - (3)].y_var)->type = IR_STRUCT;
		IrStruct s;
		for (s = ir_struct; s; s = s->next)
			if (s->name == (yyvsp[(2) - (3)].name))
				break;
		if (!s)
			irerror("invalid struct type");
		else
			(yyvsp[(3) - (3)].y_var)->type_ptr = s;
		
		irAddStructVar((yyvsp[(3) - (3)].y_var));
		(yyval.y_var) = (yyvsp[(3) - (3)].y_var);
	;}
    break;



/* Line 1464 of yacc.c  */
#line 3963 "irParse.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 1389 "irParse.y"


#include "irLex.c"

void irerror (char *info) { 
	printf("\nsyntax error in Remopla: %s, line %d\n",
		info, irlinenumber);
	irFree();
	finish_ir();
	exit(1);
}

void irParse (char *filename) {
	if (!(irin = fopen(filename,"r"))) {
		fprintf(stderr,"modelfile %s not found\n", filename);
		exit(1);
	}

	if (irparse())
		printf("parse errors encountered while reading input\n");

	fclose(irin);
}


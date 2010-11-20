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
#define yyparse         chkparse
#define yylex           chklex
#define yyerror         chkerror
#define yylval          chklval
#define yychar          chkchar
#define yydebug         chkdebug
#define yynerrs         chknerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "chkParse.y"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chkLang.h"

int chklex(void);
void chkerror(char* info);
char current_letter = 65;
FILE *output;

fpos_t pos;
char* current_module;
char* current_name;
char* init_name;



int init_nummer = 0;
int one_call = 0;
int check_errors = 0;
int after_label = 0;


IrLang ir_lang = NULL;
Alpha alphabet = NULL;
InitList init_list = NULL;
int lineno = 1;
//for debugging information
int debuginfo = 0;






/* Line 189 of yacc.c  */
#line 119 "chkParse.c"

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
     DEFINE_LANGUAGES = 258,
     END_DEFINE = 259,
     CHECK = 260,
     MODULE = 261,
     INIT = 262,
     ERROR = 263,
     ALL = 264,
     SPEC = 265,
     IDENT = 266,
     CALLIDENT = 267,
     LETTER = 268,
     TYPE = 269,
     OPERATOR = 270,
     NEW_LETTER = 271,
     NEW_LANGUAGE = 272,
     LABEL = 273,
     ARGUMENT = 274,
     LANGUAGE = 275,
     CONCAT = 276,
     NOT = 277,
     NEXT = 278,
     EXIST = 279,
     GLOBAL = 280,
     AND = 281,
     OR = 282,
     UNTIL = 283
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 40 "chkParse.y"

	char *string;
	struct IrDefAlphabet *y_alpha;
	struct IrDefLanguageStruct *y_lang;



/* Line 214 of yacc.c  */
#line 191 "chkParse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 203 "chkParse.c"

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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   134

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNRULES -- Number of states.  */
#define YYNSTATES  133

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    37,     2,     2,     2,     2,     2,
      33,    34,    23,    22,     2,    24,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    39,    32,
       2,    38,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    35,     2,    36,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    25,    26,    27,
      28,    29,    30,    31
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     9,    10,    15,    17,    21,    25,    28,
      31,    35,    38,    41,    45,    49,    53,    57,    59,    63,
      64,    66,    70,    76,    82,    86,    88,    91,    93,    96,
      98,   102,   104,   106,   108,   109,   112,   115,   118,   121,
     123,   127,   128,   131,   135,   138,   141,   144,   147,   150,
     151,   154,   158,   162,   163,   166,   169,   175,   181,   185,
     187,   188,   190,   191,   193,   197,   201,   203,   208,   211,
     213,   219,   222,   225,   230
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      41,     0,    -1,    45,    42,    52,    56,    51,    -1,    -1,
      10,    11,    43,    32,    -1,    11,    -1,    43,    29,    43,
      -1,    43,    30,    43,    -1,    25,    43,    -1,    26,    43,
      -1,    43,    31,    43,    -1,    27,    43,    -1,    28,    43,
      -1,    33,    43,    34,    -1,    35,    44,    36,    -1,    37,
      20,    37,    -1,    44,    15,    11,    -1,    11,    -1,     3,
      46,     4,    -1,    -1,     1,    -1,    46,    11,     1,    -1,
      46,    11,    38,    47,     1,    -1,    46,    11,    38,    47,
      32,    -1,    47,    22,    48,    -1,    48,    -1,    48,    49,
      -1,    49,    -1,    49,    23,    -1,    50,    -1,    33,    47,
      34,    -1,    13,    -1,    11,    -1,     9,    -1,    -1,    65,
      51,    -1,    63,    51,    -1,    64,    51,    -1,    53,    51,
      -1,     1,    -1,    58,    32,    52,    -1,    -1,    58,     1,
      -1,    54,    55,    36,    -1,    58,    35,    -1,    58,     1,
      -1,    55,    65,    -1,    55,    64,    -1,    55,    63,    -1,
      -1,    55,     1,    -1,     7,    57,    32,    -1,     7,    57,
       1,    -1,    -1,    57,    13,    -1,    57,    11,    -1,     6,
      13,    33,    60,    34,    -1,     6,    16,    33,    60,    34,
      -1,     6,    13,     1,    -1,    61,    -1,    -1,    62,    -1,
      -1,    19,    -1,    61,    15,    19,    -1,    33,    61,    34,
      -1,     1,    -1,    62,    15,    14,    19,    -1,    14,    19,
      -1,     1,    -1,     5,    33,    20,    34,    32,    -1,     5,
       1,    -1,    13,    39,    -1,    13,    33,    59,    34,    -1,
      12,    38,    13,    33,    59,    34,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   100,   100,   123,   124,   132,   134,   138,   142,   145,
     148,   152,   155,   158,   161,   164,   229,   234,   241,   246,
     247,   248,   249,   250,   377,   382,   387,   392,   397,   401,
     406,   410,   420,   432,   440,   441,   442,   443,   444,   445,
     448,   449,   450,   453,   460,   471,   476,   477,   478,   479,
     480,   485,   486,   489,   490,   497,   507,   542,   582,   587,
     588,   591,   592,   601,   606,   611,   615,   622,   629,   636,
     643,   705,   709,   743,   793
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFINE_LANGUAGES", "END_DEFINE",
  "CHECK", "MODULE", "INIT", "ERROR", "ALL", "SPEC", "IDENT", "CALLIDENT",
  "LETTER", "TYPE", "OPERATOR", "NEW_LETTER", "NEW_LANGUAGE", "LABEL",
  "ARGUMENT", "LANGUAGE", "CONCAT", "'+'", "'*'", "'-'", "NOT", "NEXT",
  "EXIST", "GLOBAL", "AND", "OR", "UNTIL", "';'", "'('", "')'", "'{'",
  "'}'", "'\"'", "'='", "':'", "$accept", "startsym", "optformula",
  "formula", "varexpr", "language", "languages", "expression", "konk",
  "stern", "klammer", "rest", "moduledecl", "moduleimpl", "modulehead",
  "modulebody", "init", "initlist", "module", "optarglist", "optvarlist",
  "arglist", "varlist", "checkstmt", "label", "callstmt", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    43,    42,    45,   277,   278,   279,   280,   281,
     282,   283,    59,    40,    41,   123,   125,    34,    61,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    42,    42,    43,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    44,    44,    45,    46,
      46,    46,    46,    46,    47,    47,    48,    48,    49,    49,
      50,    50,    50,    50,    51,    51,    51,    51,    51,    51,
      52,    52,    52,    53,    54,    54,    55,    55,    55,    55,
      55,    56,    56,    57,    57,    57,    58,    58,    58,    59,
      59,    60,    60,    61,    61,    61,    61,    62,    62,    62,
      63,    63,    64,    65,    65
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     5,     0,     4,     1,     3,     3,     2,     2,
       3,     2,     2,     3,     3,     3,     3,     1,     3,     0,
       1,     3,     5,     5,     3,     1,     2,     1,     2,     1,
       3,     1,     1,     1,     0,     2,     2,     2,     2,     1,
       3,     0,     2,     3,     2,     2,     2,     2,     2,     0,
       2,     3,     3,     0,     2,     2,     5,     5,     3,     1,
       0,     1,     0,     1,     3,     3,     1,     4,     2,     1,
       5,     2,     2,     4,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     3,    20,     0,     1,     0,    41,    18,
       0,     0,     0,     0,     0,    21,     0,     5,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    53,     0,
      42,    41,    33,    32,    31,     0,     0,    25,    27,    29,
       8,     9,    11,    12,     0,    17,     0,     0,     0,     0,
       0,     4,    58,     0,     0,     0,    39,     0,     0,     0,
       2,     0,    49,     0,     0,     0,     0,    40,     0,    22,
       0,    23,    26,    28,    13,     0,    14,    15,     6,     7,
      10,    69,     0,     0,    61,     0,    52,    55,    54,    51,
      71,     0,     0,     0,    72,    38,     0,    45,    44,    36,
      37,    35,    30,    24,    16,    68,    56,     0,    57,     0,
       0,    66,    63,     0,     0,    59,    50,    43,    48,    47,
      46,     0,     0,     0,     0,    73,     0,    67,    70,     0,
      65,    64,    74
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     8,    25,    46,     3,     5,    36,    37,    38,
      39,    60,    13,    61,    62,    96,    29,    55,    63,   114,
      83,   115,    84,    64,    65,    66
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -38
static const yytype_int8 yypact[] =
{
       5,    78,    12,    19,   -38,    47,   -38,    20,    46,   -38,
       0,    37,    86,    64,    23,   -38,    17,   -38,    37,    37,
      37,    37,    37,    62,    57,    54,     9,    45,   -38,    75,
     -38,    46,   -38,   -38,   -38,    17,    22,    17,    71,   -38,
      74,    74,    74,    74,    61,   -38,   -11,    63,    37,    37,
      37,   -38,   -38,     6,     6,    21,   -38,    10,    68,    28,
     -38,    75,   -38,     4,    75,    75,    75,   -38,    35,   -38,
      17,   -38,    71,   -38,   -38,    90,   -38,   -38,    29,    76,
     -38,   -38,    89,    77,    94,    79,   -38,   -38,   -38,   -38,
     -38,    92,    97,     2,   -38,   -38,     1,   -38,   -38,   -38,
     -38,   -38,   -38,    17,   -38,   -38,   -38,   100,   -38,    81,
      83,   -38,   -38,     8,    84,   102,   -38,   -38,   -38,   -38,
     -38,   101,    87,     2,    34,   -38,   103,   -38,   -38,    91,
     -38,   -38,   -38
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -38,   -38,   -38,    -3,   -38,   -38,   -38,    88,    51,   -37,
     -38,    32,    93,   -38,   -38,   -38,   -38,   -38,    25,     3,
      73,    15,   -38,    33,    36,    38
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -63
static const yytype_int16 yytable[] =
{
      72,    15,   116,   111,    75,    97,    57,    81,     1,   111,
      52,    90,     6,    58,    59,    40,    41,    42,    43,    44,
      82,   112,    86,    69,    30,    76,    32,   112,    33,     7,
      34,    11,    87,    14,    88,   113,   -60,   117,    16,    98,
     -62,   113,    53,    91,    70,    78,    79,    80,    17,   126,
      35,     9,    12,    89,    71,    31,    14,    70,    10,    49,
      50,    93,    18,    19,    20,    21,    72,    94,   130,   102,
      22,    28,    23,    45,    24,   -34,    56,    47,    54,     4,
      57,    12,   -19,    48,    49,    50,    51,    58,    59,   -19,
      48,    49,    50,    95,    73,    74,    99,   100,   101,    26,
      77,   104,    27,    48,    49,    50,    92,    50,   105,   107,
     110,   106,   109,   108,   121,   122,   123,   126,   125,   128,
     127,   103,   131,    68,    67,   132,   129,    85,   124,   118,
       0,     0,   119,     0,   120
};

static const yytype_int8 yycheck[] =
{
      37,     1,     1,     1,    15,     1,     5,     1,     3,     1,
       1,     1,     0,    12,    13,    18,    19,    20,    21,    22,
      14,    19,     1,     1,     1,    36,     9,    19,    11,    10,
      13,    11,    11,     8,    13,    33,    34,    36,    38,    35,
      34,    33,    33,    33,    22,    48,    49,    50,    11,    15,
      33,     4,     6,    32,    32,    32,    31,    22,    11,    30,
      31,    33,    25,    26,    27,    28,   103,    39,    34,    34,
      33,     7,    35,    11,    37,     0,     1,    20,    33,     1,
       5,     6,     4,    29,    30,    31,    32,    12,    13,    11,
      29,    30,    31,    61,    23,    34,    64,    65,    66,    13,
      37,    11,    16,    29,    30,    31,    38,    31,    19,    15,
      13,    34,    20,    34,    14,    34,    33,    15,    34,    32,
      19,    70,    19,    35,    31,    34,   123,    54,   113,    96,
      -1,    -1,    96,    -1,    96
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    41,    45,     1,    46,     0,    10,    42,     4,
      11,    11,     6,    52,    58,     1,    38,    11,    25,    26,
      27,    28,    33,    35,    37,    43,    13,    16,     7,    56,
       1,    32,     9,    11,    13,    33,    47,    48,    49,    50,
      43,    43,    43,    43,    43,    11,    44,    20,    29,    30,
      31,    32,     1,    33,    33,    57,     1,     5,    12,    13,
      51,    53,    54,    58,    63,    64,    65,    52,    47,     1,
      22,    32,    49,    23,    34,    15,    36,    37,    43,    43,
      43,     1,    14,    60,    62,    60,     1,    11,    13,    32,
       1,    33,    38,    33,    39,    51,    55,     1,    35,    51,
      51,    51,    34,    48,    11,    19,    34,    15,    34,    20,
      13,     1,    19,    33,    59,    61,     1,    36,    63,    64,
      65,    14,    34,    33,    61,    34,    15,    19,    32,    59,
      34,    19,    34
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
#line 100 "chkParse.y"
    { //printf("startsym\n");
	InitList initl = init_list;
	while(initl != NULL)
	{
		//printf("initl->name = %s\n",initl->initName);
		if(initl->ifModule)
		{
			fprintf(output, "\nstart_init_%d:\t%s(", initl->initNummer, initl->initName);
			printInitialParameters();
			fprintf(output, ");\n");
		}
		else
		{
			fprintf(output, "\nstart_init_%d:\tgoto %s;\n", initl->initNummer, initl->initName);
		}
		initl = initl->next;
	}
	
	if(!check_errors)printf("the program was translated successfully\n");
 	;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 123 "chkParse.y"
    {;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 124 "chkParse.y"
    { // at the end the result is stored on the top of the stack
				  fprintf(output, "\nspec %s %s;\n", (yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].string));
			  	  if(debuginfo) printf("output formel = %s\n", (yyvsp[(3) - (4)].string));
				  free((yyvsp[(2) - (4)].string));
				  free((yyvsp[(3) - (4)].string)); ;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 132 "chkParse.y"
    { (yyval.string) = strdup((yyvsp[(1) - (1)].string)); 
				  free((yyvsp[(1) - (1)].string));  ;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 134 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (3)].string)) + strlen((yyvsp[(3) - (3)].string)) + 3); 
				  sprintf((yyval.string), "%s&&%s", (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));	
				  free((yyvsp[(1) - (3)].string));
				  free((yyvsp[(3) - (3)].string));;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 138 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (3)].string)) + strlen((yyvsp[(3) - (3)].string)) + 3); 
				  sprintf((yyval.string), "%s||%s", (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));	
				  free((yyvsp[(1) - (3)].string));
				  free((yyvsp[(3) - (3)].string));;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 142 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (2)].string)) + 3); 
				  sprintf((yyval.string), "!%s", (yyvsp[(2) - (2)].string));	
				  free((yyvsp[(2) - (2)].string));;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 145 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (2)].string)) + 3); 
				  sprintf((yyval.string), "X%s", (yyvsp[(2) - (2)].string));	
				  free((yyvsp[(2) - (2)].string));;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 148 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (3)].string)) + strlen((yyvsp[(3) - (3)].string)) + 3); 
				  sprintf((yyval.string), "%sU%s", (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));	
				  free((yyvsp[(1) - (3)].string));
				  free((yyvsp[(3) - (3)].string));;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 152 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (2)].string)) + 3); 
				  sprintf((yyval.string), "E%s", (yyvsp[(2) - (2)].string));	
				  free((yyvsp[(2) - (2)].string));;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 155 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (2)].string)) + 3); 
				  sprintf((yyval.string), "G%s", (yyvsp[(2) - (2)].string));	
				  free((yyvsp[(2) - (2)].string));;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 158 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (3)].string)) + 5); 
				  sprintf((yyval.string), "(%s)", (yyvsp[(2) - (3)].string));	
				  free((yyvsp[(2) - (3)].string));;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 161 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (3)].string)) + 5); 
				  sprintf((yyval.string), "{%s}", (yyvsp[(2) - (3)].string));	
				  free((yyvsp[(2) - (3)].string));;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 164 "chkParse.y"
    { char *name = (char*)malloc(strlen((yyvsp[(2) - (3)].string)) + 20);
				  sprintf(name, "Languages/%s", (yyvsp[(2) - (3)].string));
				  FILE *to_read = fopen(name, "r");
				  if(to_read == NULL){
					printf("Error: there is no language %s\n", (yyvsp[(2) - (3)].string));
					chkerror("couldn't translate the given formel");
		 	  	  }else{				  	
					  char *row;
					  row = (char*)malloc(30);
					  
					  int check_first = 1;
					  (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (3)].string)) + 25);
					  sprintf((yyval.string), "(");
					  while(fgets(row, 25, to_read) != NULL)			
		  			  {
						if(debuginfo) printf("row = %s\n", row);
						char *first_state = strtok(row, " ");
						char *second_state = strtok(NULL, " ");
						second_state = strtok(NULL, " \n");
					
						if(!strcmp(second_state, "(FINAL)"))
						{
							if(!strcmp(first_state, "0")){
								if(!check_first){
									 char *tmp;
									 if ((tmp = realloc((yyval.string), strlen((yyval.string)) + strlen((yyvsp[(2) - (3)].string)) + 30)) == NULL) fprintf(stderr, "ERROR: realloc failed\n");
									 (yyval.string) = tmp;
									 sprintf((yyval.string), "%s||", (yyval.string));	
								}
								sprintf((yyval.string), "%s{state_of_%s=0}", (yyval.string), (yyvsp[(2) - (3)].string));
								check_first = 0;
							
							}else if(!atoi(first_state))
								printf("Cannot convert string %s to integer\n", first_state);
							else{
								if(!check_first){
									 char *tmp;
									 if ((tmp = realloc((yyval.string), strlen((yyval.string)) + strlen((yyvsp[(2) - (3)].string)) + 30)) == NULL) fprintf(stderr, "ERROR: realloc failed\n");
									 (yyval.string) = tmp;
									 
									 sprintf((yyval.string), "%s||", (yyval.string));	
								}
								
								sprintf((yyval.string), "%s{state_of_%s=%d}", (yyval.string), (yyvsp[(2) - (3)].string), atoi(first_state));
								check_first = 0;
							}
						}
						first_state = NULL;
						second_state = NULL;
					  }
					
					
					  free(row);
					  sprintf((yyval.string), "%s)", (yyval.string));
				   }
				   if(to_read)fclose(to_read);
				   to_read = NULL;
				   free(name);
				   free((yyvsp[(2) - (3)].string));
					  
					 
					
				;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 229 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (3)].string)) + strlen((yyvsp[(2) - (3)].string)) + strlen((yyvsp[(3) - (3)].string)) + 1); 
				  sprintf((yyval.string), "%s%s%s", (yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].string));	
				  free((yyvsp[(1) - (3)].string));
				  free((yyvsp[(2) - (3)].string));
				  free((yyvsp[(3) - (3)].string));;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 241 "chkParse.y"
    {;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 246 "chkParse.y"
    {;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 247 "chkParse.y"
    { chkerror("error in language description"); ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 248 "chkParse.y"
    { chkerror("error in language syntax, probably '=' fails");;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 249 "chkParse.y"
    { chkerror("error in language syntax, probably ';' fails");;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 250 "chkParse.y"
    { 
		
		// fill the all-clause
		
		char* expr_to_search = strdup((yyvsp[(4) - (5)].string));
		
		char* after_expr;
		char* cur_expr = NULL;
		if(debuginfo)printf("expression before all = %s\n", (yyvsp[(4) - (5)].string));

		char* end_expr = strdup((yyvsp[(4) - (5)].string));
		char* all = strpbrk(expr_to_search, "A");
		if(all != NULL){

 			printf("not null\n");
			while(all != NULL)
			{
				int l = strcspn(expr_to_search, "A");
				if(debuginfo)printf("l = %d\n", l);
	
				char* vor_expr = (char*)malloc(l + 110);
				if(debuginfo)printf("expr_to_search = %s\n", expr_to_search);

				strncpy(vor_expr, expr_to_search, l);
				vor_expr[l] = 0;
				if(debuginfo)printf("vor_expr = %s\n", vor_expr);


				after_expr = (char*)malloc(strlen(expr_to_search)-l+1);
				strcpy(after_expr, all+1);
				if(debuginfo)printf("after_expr = %s\n", after_expr);

						
				sprintf(vor_expr, "%s(A", vor_expr);
				Alpha alpha = alphabet;
				while(alpha != NULL)
				{	sprintf(vor_expr, "%s+%c", vor_expr, alpha->letter);
					alpha = alpha->next;	}
				sprintf(vor_expr, "%s)", vor_expr);
				if(debuginfo)printf("vor_expr = %s\n", vor_expr);

				all = NULL;
				free(expr_to_search);
				expr_to_search = strdup(after_expr);
				if(end_expr != NULL)free(end_expr);
				if(cur_expr != NULL){
					end_expr = (char*)malloc(strlen(cur_expr)+strlen(vor_expr)+2);
					sprintf(end_expr, "%s%s", cur_expr, vor_expr);
					free(cur_expr);
				}else end_expr = strdup(vor_expr);
				
				cur_expr = strdup(end_expr);
				if(debuginfo)printf("end_expr = %s\n", end_expr);
				free(after_expr);
				free(vor_expr);
				all = strpbrk(expr_to_search, "A");
			}
			free(end_expr);
			end_expr = (char*)malloc(strlen(cur_expr)+strlen(expr_to_search)+2);
			sprintf(end_expr, "%s%s", cur_expr, expr_to_search);
			free(cur_expr);
		}
		
		free(expr_to_search);

		if(debuginfo)printf("expression after all = %s\n", end_expr);
		IrLang lang = irLangCreate((yyvsp[(2) - (5)].string), end_expr, ir_lang);
		if(debuginfo)printf("lang->expression = %s\n", lang->expression);
		ir_lang = lang;	
		free(end_expr);
  		char* execute = (char*)malloc(strlen(lang->expression) + 100);
		
		sprintf(execute, "echo \"%s\" | retofm |fmdeterm |fmmin >Languages/%s", lang->expression, lang->name);

		if(debuginfo)printf("execute = %s\n", execute);
		system(execute);
		free(execute);
		
		if(debuginfo)printf("before fopen, filename = %s\n", lang->name);
		
		char* name = (char*)malloc(50);
		sprintf(name, "Languages/%s", lang->name);

		FILE *automat = fopen(name, "r");
		free(name);
		if(automat == NULL)
			{printf("Error while translating the expression  %s into the automat\n", lang->expression); exit(1);}
		//else printf("file opened successfully\n");
		
		int unaccept = 0;
		//else printf("file opened\n");		
		char *row;
		row = (char*)malloc(30);
					
		  while(fgets(row, 25, automat) != NULL)
		  {
		  	if(debuginfo)printf("row = %s", row);
			int n = 0;
			char *state = strtok(row, " ");
			//printf("state = %s\n", state);
			if(atoi(state))
				n = atoi(state);
			
			if(n > unaccept) unaccept = n;

			free(row);
			row = (char*)malloc(30);
		  }
		
		free(row);
		lang->unaccept = unaccept + 1;
		
		

		if(automat)fclose(automat);
		automat = NULL;
		
		
			
		free((yyvsp[(2) - (5)].string));
		free((yyvsp[(4) - (5)].string));
		
		;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 377 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (3)].string)) + strlen((yyvsp[(3) - (3)].string)) + 2); 
					  sprintf((yyval.string), "%s+%s", (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));	
					  free((yyvsp[(1) - (3)].string));
					  free((yyvsp[(3) - (3)].string));
				;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 382 "chkParse.y"
    { (yyval.string) = strdup((yyvsp[(1) - (1)].string));
				  free((yyvsp[(1) - (1)].string));	
				;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 387 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (2)].string)) + strlen((yyvsp[(2) - (2)].string)) + 1);
					  sprintf((yyval.string), "%s%s", (yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string));
					  free((yyvsp[(1) - (2)].string));
					  free((yyvsp[(2) - (2)].string));
				;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 392 "chkParse.y"
    { (yyval.string) = strdup((yyvsp[(1) - (1)].string));
				  free((yyvsp[(1) - (1)].string));	
				;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 397 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (2)].string)) + 2);
					  sprintf((yyval.string), "%s*", (yyvsp[(1) - (2)].string));
					  free((yyvsp[(1) - (2)].string));
				;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 401 "chkParse.y"
    { (yyval.string) = strdup((yyvsp[(1) - (1)].string));
				  free((yyvsp[(1) - (1)].string));	
				;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 406 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (3)].string)) + 3);
					  sprintf((yyval.string), "(%s)", (yyvsp[(2) - (3)].string));	
					  free((yyvsp[(2) - (3)].string));
				;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 410 "chkParse.y"
    { (yyval.string) = (char*)malloc(2);
					  
					  char let;
					  let = getLetter((yyvsp[(1) - (1)].string));
					  
					  (yyval.string)[0]=let;
					  (yyval.string)[1]=0;
					  free((yyvsp[(1) - (1)].string));
					  				  
				;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 420 "chkParse.y"
    { if(current_letter == 122) chkerror("Maximum 51 letters are possible in the alphabet ");
					  else	if(current_letter == 90) current_letter = 97;
						else current_letter = current_letter + 1;
						
					  Alpha alpha = irAlphaCreate((yyvsp[(1) - (1)].string), current_letter, alphabet);
					  alphabet = alpha;
					  	 
					  (yyval.string) = (char*)malloc(2);
					  (yyval.string)[0]=current_letter;
					  (yyval.string)[1]=0;
					  free((yyvsp[(1) - (1)].string));
				;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 432 "chkParse.y"
    { (yyval.string) = (char*)malloc(2);
				  (yyval.string)[0] = 'A';
				  (yyval.string)[1] = 0;
				;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 440 "chkParse.y"
    {;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 441 "chkParse.y"
    {;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 442 "chkParse.y"
    {;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 443 "chkParse.y"
    {;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 444 "chkParse.y"
    {;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 445 "chkParse.y"
    {chkerror("error in program structure"); ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 448 "chkParse.y"
    {;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 449 "chkParse.y"
    {;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 450 "chkParse.y"
    { chkerror("error in module declaration, probably ';' expected"); ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 453 "chkParse.y"
    { if(current_module) free(current_module);
					  current_module = NULL;
					  one_call = 0;
					  
					;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 460 "chkParse.y"
    { if(!current_name){ chkerror("error within module: couldn't define the name of the current module"); exit(1);}
			  if(current_module) free(current_module);
			  current_module = strdup(current_name);
			  fprintf(output, "\n\n");
			  IrLang lang = ir_lang;
			  while(lang != NULL)
			  {
			  	fprintf(output, "int local_state_of_%s;\n", lang->name);
			  	lang = lang->next;
			  }
			;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 471 "chkParse.y"
    { chkerror("error in modulehead, '{' is expected"); ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 476 "chkParse.y"
    {;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 477 "chkParse.y"
    {;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 478 "chkParse.y"
    {;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 479 "chkParse.y"
    {;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 480 "chkParse.y"
    {chkerror("unexpected symbol in modulebody found"); ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 485 "chkParse.y"
    { ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 486 "chkParse.y"
    { chkerror("error in init list declaration, probably ';' fails"); ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 489 "chkParse.y"
    { ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 490 "chkParse.y"
    { InitList new_init = InitListIdentCreate(init_nummer, (yyvsp[(2) - (2)].string), init_list);
				  init_list = new_init;
				  fprintf(output, " start_init_%d", init_nummer);
				  init_nummer++;
				  free((yyvsp[(2) - (2)].string));
				;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 497 "chkParse.y"
    { InitList new_init = InitListIdentCreate(init_nummer, (yyvsp[(2) - (2)].string), init_list);
				  init_list = new_init;
				  fprintf(output, " start_init_%d", init_nummer);
				  init_nummer++;
				  
				  free((yyvsp[(2) - (2)].string));
				;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 507 "chkParse.y"
    { 
					    if(current_name)free(current_name);
					    current_name = NULL;
					    if(debuginfo)printf("varlist = '%s'\n", (yyvsp[(4) - (5)].string));
					    current_name = strdup((yyvsp[(2) - (5)].string));
					    if(current_name == NULL) printf("malloc error in module (letter)!\n");
					    if(debuginfo)printf("in module '%s'\n", (yyvsp[(2) - (5)].string));
					    if(debuginfo)printf("cur_name (letter) = '%s'\n", current_name);
					    fprintf(output, " %s( %s", (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].string));
					    	
					if(strlen((yyvsp[(4) - (5)].string)) != 0) fprintf(output, ",");	
							 								
					InitList initl = init_list; 
					while(initl != NULL)						
					{
						if(!strcmp(initl->initName, (yyvsp[(2) - (5)].string)))
							initl->ifModule = 1;
						initl = initl->next;
					}			
							  
					IrLang lang = ir_lang;
					while(lang != NULL)	
					{
					  	if(lang->next == NULL)
							fprintf(output, "int state_of_%s ", lang->name);
						else
					 		fprintf(output, "int state_of_%s, ", lang->name);
						lang = lang->next;
					}
					fprintf(output, " )");
					
					free((yyvsp[(2) - (5)].string));
					free((yyvsp[(4) - (5)].string));
						 
					;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 542 "chkParse.y"
    {  
						if(current_name) free(current_name);
						current_name = NULL;
						if(debuginfo)printf("varlist = '%s'\n", (yyvsp[(4) - (5)].string));
						if(debuginfo)printf("in new module %s\n", (yyvsp[(2) - (5)].string));
						current_name = strdup((yyvsp[(2) - (5)].string));
						if(current_name == NULL) printf("malloc error!\n");
						//printf("in new module %s\n", $2);
						if(debuginfo)printf("cur_name (new_letter) = %s\n", current_name);
						Alpha alpha = irAlphaCreate((yyvsp[(2) - (5)].string), '1', alphabet);
						
						alphabet = alpha;
						
						fprintf(output, " %s( %s", (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].string));
					    	
					    		  if(strlen((yyvsp[(4) - (5)].string)) != 0) fprintf(output, ",");	
							 						  
							  InitList initl = init_list; 
							  while(initl != NULL)						
							  {
							  	if(!strcmp(initl->initName, (yyvsp[(2) - (5)].string)))
									initl->ifModule = 1;
							  	initl = initl->next;
							  }			
							  							  
							  IrLang lang = ir_lang;
					  		  while(lang != NULL)	
							  {
							  	if(lang->next == NULL)
					  				fprintf(output, "int state_of_%s ", lang->name);
								else
									fprintf(output, "int state_of_%s, ", lang->name);
								lang = lang->next;
							  }
					  		  fprintf(output, " )");
							  
						free((yyvsp[(2) - (5)].string));
						free((yyvsp[(4) - (5)].string));
						;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 582 "chkParse.y"
    {chkerror("error in module definition, probably '(' fails");;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 587 "chkParse.y"
    { (yyval.string) = strdup((yyvsp[(1) - (1)].string));;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 588 "chkParse.y"
    { (yyval.string) = strdup("");;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 591 "chkParse.y"
    { (yyval.string) = strdup((yyvsp[(1) - (1)].string));;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 592 "chkParse.y"
    { (yyval.string) = strdup("");;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 601 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (1)].string)) + 1);
					  sprintf((yyval.string), "%s", (yyvsp[(1) - (1)].string));
					 
					  free((yyvsp[(1) - (1)].string));
					;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 606 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (3)].string))+strlen((yyvsp[(2) - (3)].string)) + strlen((yyvsp[(3) - (3)].string)) + 5);
					  sprintf((yyval.string), "%s %s %s", (yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].string));
					  free((yyvsp[(1) - (3)].string));
					  free((yyvsp[(2) - (3)].string));
					  free((yyvsp[(3) - (3)].string));;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 611 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(2) - (3)].string)) + 5);
					  sprintf((yyval.string), "(%s)", (yyvsp[(2) - (3)].string));
					  free((yyvsp[(2) - (3)].string));;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 615 "chkParse.y"
    { chkerror("error in list of parameters found");;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 622 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (4)].string))+strlen((yyvsp[(3) - (4)].string))+strlen((yyvsp[(4) - (4)].string)) + 10);
					  sprintf((yyval.string), "%s, %s %s", (yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].string), (yyvsp[(4) - (4)].string));	
					  
					  free((yyvsp[(1) - (4)].string));
					  free((yyvsp[(3) - (4)].string));
					  free((yyvsp[(4) - (4)].string));
					;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 629 "chkParse.y"
    { (yyval.string) = (char*)malloc(strlen((yyvsp[(1) - (2)].string))+strlen((yyvsp[(2) - (2)].string))+10);
					  
					  sprintf((yyval.string), "%s %s", (yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string));
					  
					  free((yyvsp[(1) - (2)].string));
					  free((yyvsp[(2) - (2)].string));
					;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 636 "chkParse.y"
    { chkerror("error in list of arguments found");;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 643 "chkParse.y"
    { if(debuginfo)printf("check statement with language %s was parsed\n", (yyvsp[(3) - (5)].string));
					  fprintf(output, "\n skip(");
					  
					  
					  char* filename;
					  filename = strdup((yyvsp[(3) - (5)].string));
					  char* name = (char*)malloc(20 + strlen(filename));
					  sprintf(name, "Languages/%s", filename);
					  FILE *to_read = fopen(name, "r");
					  free(name);
					  					  					  
					  if(to_read == NULL)
						printf("Error while reading the file  %s", filename);
					 
						
						
						
					  	
					  char *row;
					  row = (char*)malloc(30);
					  
					  int check_first = 1;
					  while(fgets(row, 25, to_read) != NULL)			
		  			{
					
		  			char *first_state = strtok(row, " ");
					char *second_state = strtok(NULL, " ");
					second_state = strtok(NULL, " \n");
					
					if(!strcmp(second_state, "(FINAL)"))
					{
						
						if(!strcmp(first_state, "0")){
							if(!check_first) fprintf(output, "||");
							fprintf(output, "(state_of_%s == 0)", filename);
							check_first = 0;
						}else if(!atoi(first_state))
							printf("Cannot convert string %s to integer\n", first_state);
						else{
						  if(!check_first) fprintf(output, "||");
				fprintf(output, "(state_of_%s == %d)", filename, atoi(first_state));
				check_first = 0;}
					}
					first_state = NULL;
					second_state = NULL;
					
			
		  			}
					
					
					  free(row);
					  
					  if(to_read)fclose(to_read);
					  to_read = NULL;
					  free(filename);
					  fprintf(output, ");\n");
					 
					  				  
					  //free($3);
					  	
						
					;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 705 "chkParse.y"
    {chkerror("error in check statement"); ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 709 "chkParse.y"
    { if(current_module == NULL){ 
				char* er = (char*)malloc(100);
				sprintf(er, "you can not use the label: '%s' out of any module", (yyvsp[(1) - (2)].string));
				chkerror(er);
				free(er);}
			  else {
				fprintf(output, "%s: ", (yyvsp[(1) - (2)].string));
				after_label = 1;
				char let = getLetter(current_module);	
			  	

				/*if(let == '1')
				{
					fprintf(output, "if\n");
					IrLang lang = ir_lang;
					while(lang != NULL){
						int find = printForLetter(lang->name, "state_of_", 'A');							
						if(find)fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
						else fprintf(output, "\t::true -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
						fprintf(output, "fi;\n");
						lang = lang->next;
					}
				}
			  	else*/
				printStateTransfer("state_of_", let);
				let = getLetter((yyvsp[(1) - (2)].string));
				//if(let != '1') -- if we know it's a LETTER => let != '1'
				printStateTransfer("local_state_of_", let);
				free((yyvsp[(1) - (2)].string));
			   }

			;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 743 "chkParse.y"
    { 
					  char let;
					  				  
					  if(current_module == NULL)
					  { 
					  	fprintf(output, "%s(", (yyvsp[(1) - (4)].string));
						if(strlen((yyvsp[(3) - (4)].string)) > 0)
					  	{	fprintf(output, "%s,", (yyvsp[(3) - (4)].string));
							free((yyvsp[(3) - (4)].string));
						}
						printInitialParameters();
						fprintf(output, ")");
					  }
					  else {
					  if(!after_label)
					  {
					  	let = getLetter(current_module);
					  	/*if(let == '1')
					  	{
							fprintf(output, "if\n");
							IrLang lang = ir_lang;
							while(lang != NULL){
								int find = printForLetter(lang->name, "state_of", 'A');								
								if(find)fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								else fprintf(output, "\t::true -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								fprintf(output, "fi;\n");
								lang = lang->next;
							}
					  							
					  	}
						else*/ 
							printStateTransfer("state_of_", let);
					   }
						
						after_label = 0;					
						fprintf(output, "%s(", (yyvsp[(1) - (4)].string));
						
					  	if(strlen((yyvsp[(3) - (4)].string)) > 0)
					  	{	
							fprintf(output, "%s,", (yyvsp[(3) - (4)].string));
							free((yyvsp[(3) - (4)].string));
						}
						printLocalStates();
						fprintf(output, ")");

					}
					free((yyvsp[(1) - (4)].string));
					  
				;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 793 "chkParse.y"
    {
								char let;
					  				  
					  if(current_module == NULL)
					  { 
					  	fprintf(output, "%s = %s(", (yyvsp[(1) - (6)].string), (yyvsp[(3) - (6)].string));
						if(strlen((yyvsp[(5) - (6)].string)) > 0)
					  	{	fprintf(output, "%s,", (yyvsp[(5) - (6)].string));
							free((yyvsp[(5) - (6)].string));
						}
						printInitialParameters();
						fprintf(output, ")");
					  }
					  else{ 
					  if(!after_label)
					  {
					  	let = getLetter(current_module);
					  	/*if(let == '1')
					  	{
							fprintf(output, "if\n");
							IrLang lang = ir_lang;
							while(lang != NULL)
							{
								int find = printForLetter(lang->name, "local_state_of", 'A');
							
								if(find)fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								else fprintf(output, "\t::true -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								fprintf(output, "fi;\n");
								lang = lang->next;
							}
					  		
						}
						else */
							printStateTransfer("state_of_", let);
					   }	
						
						after_label = 0;
					  	fprintf(output, "%s = %s(", (yyvsp[(1) - (6)].string), (yyvsp[(3) - (6)].string));
						
					  	if(strlen((yyvsp[(5) - (6)].string)) > 0)
					  	{	
							fprintf(output, "%s,", (yyvsp[(5) - (6)].string));
							free((yyvsp[(5) - (6)].string));
						}
						printLocalStates();
						fprintf(output, ")");							
					  
					  }
						free((yyvsp[(1) - (6)].string));
						free((yyvsp[(3) - (6)].string));
				;}
    break;



/* Line 1464 of yacc.c  */
#line 2612 "chkParse.c"
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
#line 847 "chkParse.y"


#include "chkLex.c"

extern FILE *chkin;
extern FILE *chkout;
//void FreeStruct();



/******************************* Constructor ********************************/

IrLang irLangCreate(char* name, char* expression, IrLang next)
{
	IrLang lang = malloc(sizeof(struct IrDefLanguageStruct));
	lang->name = strdup(name);
	lang->expression = strdup(expression);
	lang->next = next;
	
	return lang;
}


Alpha irAlphaCreate(char* word, char letter, Alpha next)
{
	Alpha alpha = malloc(sizeof(struct IrDefAlphabet));
	
	alpha->word = malloc(strlen(word)+1);
	strcpy(alpha->word, word);
	
	alpha->letter = letter;
	alpha->next = next;

	return alpha;
}


InitList InitListIdentCreate(int nummer, char* name, InitList next)
{
	InitList new_init = malloc(sizeof(struct InitListStruct));
	
	new_init->initNummer = nummer;
	new_init->initName = strdup(name);
	new_init->ifModule = 0;
	new_init->next = next; 
	
	
	return new_init;

}

/****************************************************************************/
char getLetter(char *word)
{
	Alpha alpha;
	alpha = alphabet;
	
	
	
	while(alpha != NULL)
	{
		if(!strcmp(alpha->word, word))
		{
			
			return alpha->letter;
		}
		alpha = alpha->next;
	
	}
	
	return 0;

}

char* getWord(char letter) 
{
	Alpha alpha = alphabet;
	
	while(alpha != NULL)
	{
		if(alpha->letter == letter)
			return alpha->word;
		alpha = alpha->next;
	
	}
	
	return NULL;
}
/****************************************************************************/

void printUnacceptStates(){

	IrLang lang = ir_lang;
	while(lang != NULL)
	{
		fprintf(output, "%d", lang->unaccept);
		lang = lang->next;
		if(lang != NULL) fprintf(output, ", ");
	}

	return;
}
/****************************************************************************/

void printLocalStates(){

	IrLang lang = ir_lang;
	while(lang != NULL)
	{
		fprintf(output, "local_state_of_%s", lang->name);
		lang = lang->next;
		if(lang != NULL) fprintf(output, ", ");
	}

	return;
}
/****************************************************************************/

void printStateTransfer(const char* stateName, char let)
{
	IrLang lang = ir_lang;
	int find;
	while(lang != NULL)
	{
		fprintf(output, "if\n");

		find = printForLetter(lang->name, stateName, let);
		if(!find){ find = printForLetter(lang->name, stateName, 'A');}

		fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
		fprintf(output, "fi;\n");

		lang = lang->next;
	}
	
	
	return;
}

/****************************************************************************/

int printForLetter(char* lang_name, const char* stateName, char let)
{
	char* name = (char*)malloc(50);
		sprintf(name, "Languages/%s", lang_name);
		FILE *autom = fopen(name, "r");
		free(name);
		if(autom == NULL)
			printf("Error while reading the file  %s\n ", lang_name);
			
		
		
				
		char *row;
		row = (char*)malloc(20);
		
		int find = 0;	
		while(fgets(row, 20, autom) != NULL)			
		{
			char *first_state = strtok(row, " ");
			char *symbol = strtok(NULL, " ");
			char sym = symbol[0];
			if(sym == let)
			{
				char *second_state = strtok(NULL, " \n");
				int s1 = atoi(first_state);
				int s2 = atoi(second_state);
				fprintf(output, "\t::(%s%s == %d) -> local_state_of_%s = %d;\n", stateName, lang_name, s1, lang_name, s2);		
				find = 1;			
			}
					
			
		 }
		
		free(row);
		if(autom)fclose(autom);
		autom = NULL;
		return find;
}

/****************************************************************************/

void printInitialParameters()
{
			IrLang lang;
			lang = ir_lang;
			
			while(lang != NULL)
			{
				char* name = (char*)malloc(50);
				sprintf(name, "Languages/%s", lang->name);
				FILE *automat1 = fopen(name, "r");
				free(name);
				if(automat1 == NULL)
				printf("Error while reading the file  %s\n ", lang->name);
		
		
				char *row;
				row = (char*)malloc(30);
				
				while(fgets(row, 25, automat1) != NULL)			
		  		{
					
		  			char *first_state = strtok(row, " ");
					
					if(!strcmp(first_state, "(START)"))
					{
						char *second_state = strtok(NULL, " ");
						second_state = strtok(NULL, " \n");
						
						if(!strcmp(second_state, "0"))
							fprintf(output, "0");
						else if(!atoi(second_state))
							printf("Cannot convert string %s to integer\n", second_state);
						     else fprintf(output, "%d", atoi(second_state));
						second_state = NULL;
					}
					 first_state = NULL;
					
			
		  		}
		
				
				free(row);
				if(automat1)fclose(automat1);
				automat1 = NULL;
				lang = lang->next;
				if(lang != NULL) fprintf(output, ",");
			}
			
	
	return;
}

/******************************* Destructor ********************************/

void irLangFree(IrLang irlang)
{
	if (!irlang) return;
	
	if (irlang->next) irLangFree(irlang->next);
	if(irlang->name)free(irlang->name);
	if(irlang->expression)free(irlang->expression);
	free(irlang);
	
	return;
}

void irAlphaFree(Alpha alpha)
{
	if(!alpha) return;
	//printf("alpha word = %s\n", alpha->word);
	if(alpha->next) irAlphaFree(alpha->next);
	
	if(alpha->word)free(alpha->word);
	free(alpha);
	
	return;
}

void InitListFree(InitList initl)
{
	if(!initl) return;
	
	if(initl->next) InitListFree(initl->next);
	free(initl->initName);
	
	free(initl);
	
	return;
}


void FreeStruct() 
{

	irLangFree(ir_lang);
	irAlphaFree(alphabet);
	InitListFree(init_list);


}


void chkerror(char *info)
{	
	printf("line %d: %s\n ", lineno, info);
	check_errors = 1;
	
}

void chkParse (char *filename) {
	if (!(chkin = fopen(filename,"r"))) {
		printf("modelfile %s not found\n", filename);
		exit(1);
	}
	
	char* exitfile = malloc(strlen(filename) + 7);
	char* newp = strrchr(filename, '/');
	char* exfile = malloc(strlen(filename) + 1);
	if(newp != NULL){ newp++; strcpy(exfile, newp);}
	else strcpy(exfile, filename);
	sprintf(exitfile, "%s%s",  "check_", exfile);
	free(exfile);
	printf("\nexitfile = %s\n", exitfile);
	output = fopen(exitfile, "w");
	chkout = output;
	if (chkparse())
		printf("parse errors encountered while reading input\n");

	//yyterminate();
	fclose(chkin);
	fclose(chkout);
	fclose(output);
	free(exitfile);
	IrLang lang = ir_lang;
	char* name = (char*)malloc(50);
	while(lang != NULL){
		sprintf(name, "Languages/%s", lang->name);
		remove(name);
		lang = lang->next;
	}
	free(name);
	FreeStruct();
	
}

/*int main( int argc, char *argv[])
{

	//chkin = fopen(argv[1], "r");
	//chkout = fopen("rest.c", "w"); 
	output = fopen("new_file_1.c", "w");
	chkout = output;
	//alphabet = NULL;
	//ir_lang = NULL;
	chkParse(argv[1]);
	//chkParse("test1.c");
	//free(current_module);
	FreeStruct();
	
}*/


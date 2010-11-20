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
#define yyparse         bpparse
#define yylex           bplex
#define yyerror         bperror
#define yylval          bplval
#define yychar          bpchar
#define yydebug         bpdebug
#define yynerrs         bpnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "bpParse.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bp.h"

int bplex();
void yyerror(char*);
extern int line;

bp_ident_t bp_globals;
bp_ident_t bp_parms;	// sichert temporär den Inhalt von bp_decls
bp_fun_t bp_functions;
bp_tree_t bp_nametree;
bp_tree_t bp_funtree;
int bp_maxrets;
int bp_numglobals;
int bp_maxlocs;

int bp_labelcount;	// Zähler, um eindeutige Labels zu vergeben

short bp_funcount;	// zählt die Funktionen
int bp_declcount;	// zählt Anzahl Variablen in einem Deklarationsblock
char bp_doing_parms;	// zeigt an, ob gerade Parameter oder lokale Variablen
			// geparst werden
bp_ident_t bp_decls;	// Sammelt die Liste der Ids in einer Deklaration

bp_tree_t bp_labeltree;	// wird hier nicht gebraucht, erst später

bp_ident_t bp_locids;	// Liste von Programmvariablen
bp_ident_t bp_globids;	// (wird u.a. in reorder.c gebraucht)
bp_ident_t bp_allglobids;



/* Line 189 of yacc.c  */
#line 117 "bpParse.c"

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
     IDENTIFIER = 258,
     INTEGER = 259,
     NONDET = 260,
     ASSIGN = 261,
     EQ_OP = 262,
     NE_OP = 263,
     IMPLIES = 264,
     DECL = 265,
     ENFORCE = 266,
     AND = 267,
     OR = 268,
     IF = 269,
     THEN = 270,
     ELSE = 271,
     ELSIF = 272,
     FI = 273,
     WHILE = 274,
     DO = 275,
     OD = 276,
     RETURN = 277,
     SKIP = 278,
     GOTO = 279,
     BEG = 280,
     END = 281,
     BOOL = 282,
     VOID = 283,
     ASSERT = 284,
     ASSUME = 285,
     PRINT = 286,
     CHOOSE = 287,
     CONSTRAIN = 288
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 37 "bpParse.y"

	bp_stmt_t stmt;
	bp_expr_t expr;
	bp_ident_t ident;
	bp_idref_t idref;
	bp_fun_t fun;
	char* c;
	int   i;



/* Line 214 of yacc.c  */
#line 198 "bpParse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 210 "bpParse.c"

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
#define YYLAST   197

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  77
/* YYNRULES -- Number of states.  */
#define YYNSTATES  158

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    43,     2,     2,     2,     2,     2,    41,
      38,    39,     2,     2,    35,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    40,    34,
      36,     2,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    45,     2,    46,    44,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    42,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     7,     8,    11,    12,    17,    18,    20,
      24,    26,    28,    31,    40,    42,    47,    49,    53,    54,
      59,    61,    62,    64,    67,    71,    73,    76,    79,    82,
      85,    87,    89,    92,    96,   100,   102,   106,   108,   113,
     118,   123,   128,   133,   137,   145,   147,   151,   159,   167,
     169,   172,   174,   177,   179,   182,   184,   188,   190,   193,
     195,   197,   199,   203,   207,   209,   213,   215,   219,   221,
     225,   227,   231,   233,   235,   237,   244,   246
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      48,     0,    -1,    49,    50,    55,    -1,    -1,    51,    52,
      -1,    -1,    10,    53,    34,    52,    -1,    -1,    54,    -1,
      53,    35,    54,    -1,     3,    -1,    56,    -1,    56,    55,
      -1,    57,     3,    59,    25,    52,    58,    61,    26,    -1,
      27,    -1,    27,    36,     4,    37,    -1,    28,    -1,    11,
      81,    34,    -1,    -1,    51,    38,    60,    39,    -1,    53,
      -1,    -1,    62,    -1,    62,    61,    -1,     3,    40,    63,
      -1,    63,    -1,    64,    34,    -1,    67,    34,    -1,    68,
      34,    -1,    69,    34,    -1,    70,    -1,    72,    -1,    73,
      34,    -1,    65,     6,    84,    -1,    65,     6,    69,    -1,
      66,    -1,    65,    35,    66,    -1,     3,    -1,    29,    38,
      82,    39,    -1,    30,    38,    82,    39,    -1,    33,    38,
      82,    39,    -1,    31,    38,    84,    39,    -1,     3,    38,
      84,    39,    -1,     3,    38,    39,    -1,    14,    38,    82,
      39,    15,    61,    71,    -1,    18,    -1,    16,    61,    18,
      -1,    17,    38,    82,    39,    15,    61,    71,    -1,    19,
      38,    82,    39,    20,    61,    21,    -1,    22,    -1,    22,
      84,    -1,    23,    -1,    24,     3,    -1,     3,    -1,    41,
       3,    -1,     4,    -1,    38,    81,    39,    -1,    74,    -1,
      76,    74,    -1,    42,    -1,    43,    -1,    75,    -1,    77,
       7,    75,    -1,    77,     8,    75,    -1,    77,    -1,    78,
      12,    77,    -1,    78,    -1,    79,    44,    78,    -1,    79,
      -1,    80,    13,    79,    -1,    80,    -1,    81,     9,    80,
      -1,     5,    -1,    81,    -1,    81,    -1,    32,    45,    81,
      35,    81,    46,    -1,    83,    -1,    84,    35,    83,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    79,    79,    85,   101,   108,   116,   117,   120,   121,
     123,   141,   142,   146,   192,   193,   194,   197,   198,   201,
     206,   207,   213,   214,   219,   223,   228,   229,   230,   231,
     232,   233,   234,   237,   259,   266,   267,   270,   278,   283,
     288,   294,   300,   307,   316,   325,   327,   329,   340,   348,
     353,   358,   362,   371,   386,   394,   403,   407,   408,   412,
     413,   416,   417,   419,   423,   424,   428,   429,   433,   434,
     438,   439,   443,   445,   448,   449,   453,   459
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "INTEGER", "NONDET",
  "ASSIGN", "EQ_OP", "NE_OP", "IMPLIES", "DECL", "ENFORCE", "AND", "OR",
  "IF", "THEN", "ELSE", "ELSIF", "FI", "WHILE", "DO", "OD", "RETURN",
  "SKIP", "GOTO", "BEG", "END", "BOOL", "VOID", "ASSERT", "ASSUME",
  "PRINT", "CHOOSE", "CONSTRAIN", "';'", "','", "'<'", "'>'", "'('", "')'",
  "':'", "'\\''", "'~'", "'!'", "'^'", "'['", "']'", "$accept", "program",
  "pstart", "global_decls", "decl_start", "decl_list", "identifier_list",
  "decl_id", "fun_list", "function_definition", "fun_type", "enforce",
  "parms", "parameter_list", "statement_list", "labelled_statement",
  "statement", "parallel_assign", "assign_id_list", "asgn_id", "assert",
  "print", "call", "selection_statement", "elsif_list",
  "iteration_statement", "jump_statement", "primary_expression",
  "unary_expression", "unary_operator", "equality_expression",
  "and_expression", "exclusive_or_expression", "or_expression",
  "expression", "decider", "choose_expression", "expression_list", 0
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
     285,   286,   287,   288,    59,    44,    60,    62,    40,    41,
      58,    39,   126,    33,    94,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    49,    50,    51,    52,    52,    53,    53,
      54,    55,    55,    56,    57,    57,    57,    58,    58,    59,
      60,    60,    61,    61,    62,    62,    63,    63,    63,    63,
      63,    63,    63,    64,    64,    65,    65,    66,    67,    67,
      67,    68,    69,    69,    70,    71,    71,    71,    72,    73,
      73,    73,    73,    74,    74,    74,    74,    75,    75,    76,
      76,    77,    77,    77,    78,    78,    79,    79,    80,    80,
      81,    81,    82,    82,    83,    83,    84,    84
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     0,     2,     0,     4,     0,     1,     3,
       1,     1,     2,     8,     1,     4,     1,     3,     0,     4,
       1,     0,     1,     2,     3,     1,     2,     2,     2,     2,
       1,     1,     2,     3,     3,     1,     3,     1,     4,     4,
       4,     4,     4,     3,     7,     1,     3,     7,     7,     1,
       2,     1,     2,     1,     2,     1,     3,     1,     2,     1,
       1,     1,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     1,     1,     6,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     5,     1,     0,     7,    14,    16,     2,    11,
       0,     0,     4,     0,    12,     5,    10,     0,     8,     0,
       0,     0,     7,     0,    15,    21,     7,     6,     9,    20,
       0,    18,    19,     0,     0,    53,    55,     0,     0,    59,
      60,    57,    61,     0,    64,    66,    68,    70,     0,    37,
       0,     0,    49,    51,     0,     0,     0,     0,     0,     0,
      22,    25,     0,     0,    35,     0,     0,     0,    30,    31,
       0,     0,    54,    58,     0,     0,     0,     0,     0,     0,
      17,     0,     0,     0,     0,     0,    74,    76,    50,    52,
       0,     0,     0,     0,    13,    23,    26,     0,     0,    27,
      28,    29,    32,    56,    62,    63,    65,    67,    69,    71,
      43,     0,    37,    24,    72,    73,     0,     0,     0,     0,
       0,     0,     0,     0,    53,    34,    33,    37,    36,    42,
       0,     0,     0,    77,    38,    39,    41,    40,     0,     0,
       0,     0,     0,     0,     0,     0,    45,    44,    48,    75,
       0,     0,    46,     0,     0,     0,     0,    47
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     4,     5,    12,    17,    18,     8,     9,
      10,    34,    21,    30,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   147,    69,    70,    41,    42,    43,
      44,    45,    46,    47,   115,   116,    87,    88
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -84
static const yytype_int16 yypact[] =
{
     -84,    42,   -84,   -84,   -13,    39,    18,   -84,   -84,   -13,
      64,    71,   -84,    79,   -84,   -84,   -84,    74,   -84,    54,
      66,    77,    39,    71,   -84,    71,    39,   -84,   -84,    75,
      78,    94,   -84,    34,    70,   -84,   -84,    34,   115,   -84,
     -84,   -84,   -84,     9,    89,   107,    76,   111,    19,    50,
      88,    90,    14,   -84,   124,    91,    93,    95,    96,   104,
      70,   -84,    98,    30,   -84,   101,   102,   103,   -84,   -84,
     105,    12,   -84,   -84,    34,    34,    34,    34,    34,    34,
     -84,     2,    92,    20,    20,    97,   129,   -84,   106,   -84,
      20,    20,    14,    20,   -84,   -84,   -84,    28,   137,   -84,
     -84,   -84,   -84,   -84,   -84,   -84,    89,   107,    76,   111,
     -84,   -19,   108,   -84,   -84,   129,   109,   110,    34,    14,
     112,   113,    -9,   114,   108,   -84,   106,   -84,   -84,   -84,
     128,   125,     0,   -84,   -84,   -84,   -84,   -84,    70,    70,
      34,    62,   123,    -7,    70,   116,   -84,   -84,   -84,   -84,
     132,    20,   -84,   117,   140,    70,    62,   -84
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -84,   -84,   -84,   -84,   142,     7,   122,   135,   150,   -84,
     -84,   -84,   -84,   -84,   -57,   -84,    80,   -84,   -84,    63,
     -84,   -84,    67,   -84,     4,   -84,   -84,   120,    38,   -84,
      99,   100,    87,   118,   -33,   -83,    47,   -70
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      48,   117,    79,    95,    71,    35,    36,   120,   121,    79,
     123,   111,    35,    36,     6,     7,   119,    35,    36,    86,
     129,    79,   122,    35,    36,   114,   119,   126,    79,    27,
     136,   124,    36,    31,    85,   140,    97,    35,    36,   149,
      37,   110,     3,    38,    39,    40,    85,    37,    86,    11,
      38,   103,    37,    80,    13,    38,    39,    40,    37,    86,
      85,    38,    39,    40,    86,    98,    37,    15,   153,    38,
      39,    40,    37,    49,    16,    38,    39,    40,   144,   145,
     146,   141,   142,    19,    50,   132,    86,   150,    81,    51,
      82,    24,    52,    53,    54,   112,    74,    75,   156,    55,
      56,    57,    26,    58,    25,    33,    50,   143,    22,    23,
      23,    51,   104,   105,    52,    53,    54,    32,    72,    76,
      77,    55,    56,    57,    78,    58,    83,    89,    84,    90,
      94,    91,    96,    92,    93,    99,   100,   101,    79,   102,
     127,   119,   118,   138,   148,   139,    81,    29,   130,   131,
     152,   134,   135,   137,   151,   155,   154,    20,    28,    14,
     157,   128,   113,    73,   125,   108,   133,     0,     0,     0,
       0,     0,     0,     0,     0,   106,     0,   107,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   109
};

static const yytype_int16 yycheck[] =
{
      33,    84,     9,    60,    37,     3,     4,    90,    91,     9,
      93,    81,     3,     4,    27,    28,    35,     3,     4,    52,
      39,     9,    92,     3,     4,     5,    35,    97,     9,    22,
      39,     3,     4,    26,    32,    35,     6,     3,     4,    46,
      38,    39,     0,    41,    42,    43,    32,    38,    81,    10,
      41,    39,    38,    34,    36,    41,    42,    43,    38,    92,
      32,    41,    42,    43,    97,    35,    38,     3,   151,    41,
      42,    43,    38,     3,     3,    41,    42,    43,    16,    17,
      18,   138,   139,     4,    14,   118,   119,   144,    38,    19,
      40,    37,    22,    23,    24,     3,     7,     8,   155,    29,
      30,    31,    25,    33,    38,    11,    14,   140,    34,    35,
      35,    19,    74,    75,    22,    23,    24,    39,     3,    12,
      44,    29,    30,    31,    13,    33,    38,     3,    38,    38,
      26,    38,    34,    38,    38,    34,    34,    34,     9,    34,
       3,    35,    45,    15,    21,    20,    38,    25,    39,    39,
      18,    39,    39,    39,    38,    15,    39,    15,    23,     9,
     156,    98,    82,    43,    97,    78,   119,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    79
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    48,    49,     0,    50,    51,    27,    28,    55,    56,
      57,    10,    52,    36,    55,     3,     3,    53,    54,     4,
      51,    59,    34,    35,    37,    38,    25,    52,    54,    53,
      60,    52,    39,    11,    58,     3,     4,    38,    41,    42,
      43,    74,    75,    76,    77,    78,    79,    80,    81,     3,
      14,    19,    22,    23,    24,    29,    30,    31,    33,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    72,
      73,    81,     3,    74,     7,     8,    12,    44,    13,     9,
      34,    38,    40,    38,    38,    32,    81,    83,    84,     3,
      38,    38,    38,    38,    26,    61,    34,     6,    35,    34,
      34,    34,    34,    39,    75,    75,    77,    78,    79,    80,
      39,    84,     3,    63,     5,    81,    82,    82,    45,    35,
      82,    82,    84,    82,     3,    69,    84,     3,    66,    39,
      39,    39,    81,    83,    39,    39,    39,    39,    15,    20,
      35,    61,    61,    81,    16,    17,    18,    71,    21,    46,
      61,    38,    18,    82,    39,    15,    61,    71
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
#line 80 "bpParse.y"
    {
		bp_globals = (yyvsp[(2) - (3)].ident);
		bp_functions = (yyvsp[(3) - (3)].fun);
	  ;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 85 "bpParse.y"
    {
		bp_funcount = -1;
		bp_nametree = NULL;
		bp_funtree = NULL;
		bp_labelcount = 0;

		bp_maxlocs = 0;
		bp_maxrets = 0;
		bp_locids = NULL;
		bp_globids = NULL;
		bp_allglobids = NULL;
	  ;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 102 "bpParse.y"
    {
		(yyval.ident) = bp_decls;
		bp_numglobals = bp_declcount;
	  ;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 108 "bpParse.y"
    {
		bp_declcount = 0;
		bp_funcount++;
		bp_decls = NULL;
		bp_doing_parms = 1;
	  ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 124 "bpParse.y"
    {
		bp_ident_t newid = alloc_bp_ident();
		newid->varname = wIdentCreate((yyvsp[(1) - (1)].c));
		newid->index = bp_funcount? bp_doing_parms?
			bp_assign_parm_index(newid->varname,bp_funcount)
		      : bp_assign_loc_index(newid->varname,bp_funcount)
		      : bp_assign_glob_index(newid->varname);
		newid->fun = bp_funcount;
		bp_insert_decl(&bp_nametree,
				newid->varname,bp_funcount,newid);
		newid->next = bp_decls;
		bp_decls = newid;
	  ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 143 "bpParse.y"
    { (yyval.fun) = (yyvsp[(1) - (2)].fun); (yyval.fun)->next = (yyvsp[(2) - (2)].fun); ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 148 "bpParse.y"
    {
		int i;

		(yyval.fun) = alloc_bp_fun();
		(yyval.fun)->funname = wIdentCreate((yyvsp[(2) - (8)].c));
		(yyval.fun)->num_returns = (yyvsp[(1) - (8)].i);
		(yyval.fun)->num_args = (yyvsp[(3) - (8)].i);
		(yyval.fun)->num_locals = bp_declcount;
		(yyval.fun)->num_globals = bp_numglobals;
		(yyval.fun)->locals = bp_decls;
		(yyval.fun)->parms = bp_parms;
		(yyval.fun)->enforce = (yyvsp[(6) - (8)].expr);
		(yyval.fun)->stmt = (yyvsp[(7) - (8)].stmt);
		(yyval.fun)->next = NULL;

		(yyval.fun)->returns = NULL;
		(yyval.fun)->callbuf = NULL;

		for (i = 0; i < (yyval.fun)->num_returns; i++)
		{
			char tmp[16];
			bp_ident_t newid = alloc_bp_ident();
			sprintf(tmp,"*ret%d",i+1);
			newid->varname = wIdentCreate(tmp);
			newid->index = bp_assign_ret_index(newid->varname,i);
			newid->fun = 0;
			newid->next = (yyval.fun)->returns;
			(yyval.fun)->returns = newid;
		}

		for (i = 0; i < (yyval.fun)->num_locals; i++)
		{
			bp_ident_t newid = alloc_bp_ident();
			newid->index = -1;
			newid->fun = 0;
			newid->next = (yyval.fun)->callbuf;
			
			(yyval.fun)->callbuf = newid;
		}

		bp_insert_decl(&bp_funtree,(yyval.fun)->funname,-1,(bp_ident_t)(yyval.fun));
	  ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 192 "bpParse.y"
    { (yyval.i) = 1; ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 193 "bpParse.y"
    { (yyval.i) = (yyvsp[(3) - (4)].i); ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 194 "bpParse.y"
    { (yyval.i) = 0; ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 197 "bpParse.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 198 "bpParse.y"
    { (yyval.expr) = NULL; ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 202 "bpParse.y"
    { (yyval.i) = bp_declcount; bp_parms = bp_decls; bp_doing_parms = 0; ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 215 "bpParse.y"
    {	(yyval.stmt) = (yyvsp[(1) - (2)].stmt); (yyval.stmt)->next = (yyvsp[(2) - (2)].stmt);		;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 220 "bpParse.y"
    {
		(yyval.stmt) = (yyvsp[(3) - (3)].stmt); (yyval.stmt)->no_label_given = 0; (yyval.stmt)->label = wIdentCreate((yyvsp[(1) - (3)].c));
	  ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 224 "bpParse.y"
    {	(yyval.stmt) = (yyvsp[(1) - (1)].stmt); (yyval.stmt)->no_label_given = 1; (yyval.stmt)->label = ++bp_labelcount; ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 238 "bpParse.y"
    {
		bp_idref_t tmp1, tmp2, tmp3;

		new_bp_stmt((yyval.stmt),BP_ASSIGN);

		for (tmp1 = (yyvsp[(1) - (3)].idref), tmp2 = (yyvsp[(3) - (3)].idref); tmp1 && tmp2; tmp1 = tmp1->next)
		{
			tmp1->expr = tmp2->expr;
			tmp3 = tmp2->next;
			free(tmp2);
			tmp2 = tmp3;
		}

		if (tmp1 || tmp2)
		{
			yyerror("number of assign clauses doesn't match");
			YYERROR;
		}

		(yyval.stmt)->e.a.asgnlist = (yyvsp[(1) - (3)].idref);
	  ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 260 "bpParse.y"
    { 
		(yyval.stmt) = (yyvsp[(3) - (3)].stmt);
		(yyval.stmt)->e.a.asgnlist = (yyvsp[(1) - (3)].idref);
	  ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 268 "bpParse.y"
    {	(yyval.idref) = (yyvsp[(3) - (3)].idref); (yyval.idref)->next = (yyvsp[(1) - (3)].idref);	;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 271 "bpParse.y"
    {
		(yyval.idref) = alloc_bp_idref();
		(yyval.idref)->ident = bp_find_decl(wIdentCreate((yyvsp[(1) - (1)].c)),bp_funcount);
		(yyval.idref)->primed = 1;
		(yyval.idref)->next = NULL;
	  ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 279 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_ASSERT);
		(yyval.stmt)->e.c.expr = (yyvsp[(3) - (4)].expr);
	  ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 284 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_ASSUME);
		(yyval.stmt)->e.c.expr = (yyvsp[(3) - (4)].expr);
	  ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 289 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_CONSTRAIN);
		(yyval.stmt)->e.c.expr = (yyvsp[(3) - (4)].expr);
	  ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 295 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_PRINT);
		(yyval.stmt)->e.a.asgnlist = (yyvsp[(3) - (4)].idref);
	  ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 301 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_CALL);
		(yyval.stmt)->e.a.label = wIdentCreate((yyvsp[(1) - (4)].c));
		(yyval.stmt)->e.a.asgnlist = NULL;
		(yyval.stmt)->e.a.fmllist = (yyvsp[(3) - (4)].idref);
	  ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 308 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_CALL);
		(yyval.stmt)->e.a.label = wIdentCreate((yyvsp[(1) - (3)].c));
		(yyval.stmt)->e.a.asgnlist = NULL;
		(yyval.stmt)->e.a.fmllist = NULL;
	  ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 317 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_IF);
		(yyval.stmt)->e.c.expr = (yyvsp[(3) - (7)].expr);
		(yyval.stmt)->e.c.thenstmt = (yyvsp[(6) - (7)].stmt);
		(yyval.stmt)->e.c.elsestmt = (yyvsp[(7) - (7)].stmt);
	  ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 326 "bpParse.y"
    {	(yyval.stmt) = NULL;	;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 328 "bpParse.y"
    {	(yyval.stmt) = (yyvsp[(2) - (3)].stmt);	;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 330 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_IF);
		(yyval.stmt)->no_label_given = 1;
		(yyval.stmt)->label = ++bp_labelcount;
		(yyval.stmt)->e.c.expr = (yyvsp[(3) - (7)].expr);
		(yyval.stmt)->e.c.thenstmt = (yyvsp[(6) - (7)].stmt);
		(yyval.stmt)->e.c.elsestmt = (yyvsp[(7) - (7)].stmt);
	  ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 341 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_WHILE);
		(yyval.stmt)->e.c.expr = (yyvsp[(3) - (7)].expr);
		(yyval.stmt)->e.c.thenstmt = (yyvsp[(6) - (7)].stmt);
	  ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 349 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_RETURN);
		(yyval.stmt)->e.a.asgnlist = NULL;
	  ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 354 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_RETURN);
		(yyval.stmt)->e.a.asgnlist = (yyvsp[(2) - (2)].idref);
	  ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 359 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_SKIP);
	  ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 363 "bpParse.y"
    {
		new_bp_stmt((yyval.stmt),BP_GOTO);
		(yyval.stmt)->e.a.label = wIdentCreate((yyvsp[(2) - (2)].c));
	  ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 372 "bpParse.y"
    { 
		if ((*(yyvsp[(1) - (1)].c) == 'T' || *(yyvsp[(1) - (1)].c) == 'F') && !(yyvsp[(1) - (1)].c)[1])
		{
			new_bp_expr((yyval.expr),BP_CONST,(bp_expr_t)(*(yyvsp[(1) - (1)].c) == 'T'),NULL);
		}
		else
		{
			bp_idref_t ref = alloc_bp_idref();
			ref->ident =
			   bp_find_var(bp_nametree,wIdentCreate((yyvsp[(1) - (1)].c)),bp_funcount);
			ref->primed = 0;
			new_bp_expr((yyval.expr),BP_IDENT,(bp_expr_t)ref,NULL);
		}
	  ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 387 "bpParse.y"
    {
		bp_idref_t ref = alloc_bp_idref();
		ref->ident =
			bp_find_var(bp_nametree,wIdentCreate((yyvsp[(2) - (2)].c)),bp_funcount);
		ref->primed = 1;
		new_bp_expr((yyval.expr),BP_IDENT,(bp_expr_t)ref,NULL);
	  ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 395 "bpParse.y"
    { 
		if ((yyvsp[(1) - (1)].i) == 1 || (yyvsp[(1) - (1)].i) ==0) {
			new_bp_expr((yyval.expr),BP_CONST,(bp_expr_t)(yyvsp[(1) - (1)].i),NULL);
		} else {
		    yyerror("Boolean constants are 0 and 1\n");
		    YYERROR;
		}
	  ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 404 "bpParse.y"
    {	(yyval.expr) = (yyvsp[(2) - (3)].expr);	;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 409 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_NOT,(yyvsp[(2) - (2)].expr),NULL);		;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 418 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_EQ,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));	;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 420 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_NE,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));	;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 425 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_AND,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));	;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 430 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_NE,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));	;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 435 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_OR,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));	;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 440 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_IMP,(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr));	;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 444 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_ND,NULL,NULL);	;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 450 "bpParse.y"
    {	new_bp_expr((yyval.expr),BP_CHOOSE,(yyvsp[(3) - (6)].expr),(yyvsp[(5) - (6)].expr));	;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 454 "bpParse.y"
    {
		(yyval.idref) = alloc_bp_idref();
		(yyval.idref)->expr = (yyvsp[(1) - (1)].expr);
		(yyval.idref)->next = NULL;
	  ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 460 "bpParse.y"
    {
		(yyval.idref) = alloc_bp_idref();
		(yyval.idref)->expr = (yyvsp[(3) - (3)].expr);
		(yyval.idref)->next = (yyvsp[(1) - (3)].idref);
	  ;}
    break;



/* Line 1464 of yacc.c  */
#line 2095 "bpParse.c"
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
#line 466 "bpParse.y"


#include "bpLex.c"

extern int column;

void yyerror(char *s)
{
	fflush(stdout);
	printf("Line %d: syntax error.\n",line);
}

bddPds* bpReadBP (char *filename)
{
	line = column = 1;
	if (!(bpin = fopen(filename,"r")))
		bpError("program file %s not found",filename);
	if (bpparse())
		bpError("parse errors encountered");
	fclose(bpin);

	/* translate bp tree into pds */
	return bp_create_pds();
}

void bpError(char* msg, ...)
{
        va_list args;
        va_start(args,msg);
        fprintf(stderr,"error: ");
        vfprintf(stderr,msg,args);
        fprintf(stderr,"\n");
        va_end(args);
        exit(1);
}


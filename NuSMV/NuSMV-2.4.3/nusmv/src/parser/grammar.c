
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

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



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "grammar.y"

/**CFile***********************************************************************

  FileName    [grammar.y]

  PackageName [parser]

  Synopsis    [Grammar (for Yacc and Bison) of NuSMV input language]

  SeeAlso     [input.l]

  Author      [Andrei Tchaltsev, Marco Roveri]

  Copyright   [
  This file is part of the ``parser'' package of NuSMV version 2. 
  Copyright (C) 1998-2005 by CMU and ITC-irst. 

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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <setjmp.h>

#if HAVE_MALLOC_H
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif  
# include <malloc.h>
#elif HAVE_SYS_MALLOC_H
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif  
# include <sys/malloc.h>
#elif HAVE_STDLIB_H
# include <stdlib.h>
#endif

#include <limits.h>

#include "parserInt.h"
#include "utils/utils.h"
#include "utils/ustring.h"
#include "node/node.h"
#include "opt/opt.h"
#include "utils/error.h"

#include "symbols.h"
#include "mbp_symbols.h"

static char rcsid[] UTIL_UNUSED = "$Id: grammar.y,v 1.19.4.10.4.46.4.17 2007/05/17 09:23:22 nusmv Exp $";

#define YYMAXDEPTH INT_MAX

node_ptr parsed_tree; /* the returned value of parsing */

enum PARSE_MODE parse_mode_flag; /* the flag what should be parsed */

extern FILE * nusmv_stderr;
    
void yyerror ARGS((char *s));
void yyerror_lined ARGS((const char *s, int line));
static node_ptr context2maincontext ARGS((node_ptr context));


/* this enum is used to distinguish 
   different kinds of expressions: SIMPLE, NEXT, CTL and LTL.
   Since syntactically only one global kind of expressions exists,
   we have to invoke a special function which checks that an expression
   complies to the additional syntactic constrains.
   So, if an ltl-expression is expected then occurrences of NEXT or EBF
   operators will cause the termination of parsing.

   NB: An alternative to invoking a checking function would be to write quite
   intricate syntactic rules to distinguish all the cases.

   NB: This checking function could also be a part of the type checker,
   but it is more straightforward to write a separate function.
*/
  enum EXP_KIND {EXP_SIMPLE, EXP_NEXT, EXP_LTL, EXP_CTL}; 

  static boolean isCorrectExp ARGS((node_ptr exp, enum EXP_KIND expectedKind));

  static int nusmv_parse_psl ARGS((void));

  /* below vars are used if input file contains game definition */
  static node_ptr mbp_parser_spec_list = Nil;
  static node_ptr mbp_parser_player_1 = Nil;
  static node_ptr mbp_parser_player_2 = Nil;


/* Line 189 of yacc.c  */
#line 186 "grammar.c"

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
     TOK_CONSTRAINT = 258,
     TOK_GOTO = 259,
     TOK_MAXU = 260,
     TOK_MINU = 261,
     TOK_ABU = 262,
     TOK_EBU = 263,
     TOK_AU = 264,
     TOK_EU = 265,
     TOK_CONTEXT = 266,
     TOK_PROCESS = 267,
     TOK_MODULE = 268,
     TOK_COMPUTE = 269,
     TOK_LTLSPEC = 270,
     TOK_CTLSPEC = 271,
     TOK_SPEC = 272,
     TOK_INVAR = 273,
     TOK_TRANS = 274,
     TOK_INIT = 275,
     TOK_DEFINE = 276,
     TOK_IVAR = 277,
     TOK_VAR = 278,
     TOK_PSLSPEC = 279,
     TOK_CONSTANTS = 280,
     TOK_JUSTICE = 281,
     TOK_COMPASSION = 282,
     TOK_FAIRNESS = 283,
     TOK_INVARSPEC = 284,
     TOK_ASSIGN = 285,
     TOK_ISA = 286,
     TOK_SEMI = 287,
     TOK_CONS = 288,
     TOK_OF = 289,
     TOK_RCB = 290,
     TOK_LCB = 291,
     TOK_RB = 292,
     TOK_RP = 293,
     TOK_LP = 294,
     TOK_TWODOTS = 295,
     TOK_EQDEF = 296,
     TOK_SELF = 297,
     TOK_COLON = 298,
     TOK_ESAC = 299,
     TOK_CASE = 300,
     TOK_COMPWFF = 301,
     TOK_CTLWFF = 302,
     TOK_LTLPSL = 303,
     TOK_LTLWFF = 304,
     TOK_SIMPWFF = 305,
     TOK_INCONTEXT = 306,
     TOK_WORD = 307,
     TOK_REAL = 308,
     TOK_INTEGER = 309,
     TOK_BOOLEAN = 310,
     TOK_ARRAY = 311,
     TOK_WORD1 = 312,
     TOK_BOOL = 313,
     TOK_WAWRITE = 314,
     TOK_WAREAD = 315,
     TOK_TRUEEXP = 316,
     TOK_FALSEEXP = 317,
     TOK_ATOM = 318,
     TOK_NUMBER_WORD = 319,
     TOK_NUMBER_EXP = 320,
     TOK_NUMBER_REAL = 321,
     TOK_NUMBER_FRAC = 322,
     TOK_NUMBER = 323,
     TOK_NOT = 324,
     TOK_AND = 325,
     TOK_XNOR = 326,
     TOK_XOR = 327,
     TOK_OR = 328,
     TOK_IFF = 329,
     TOK_IMPLIES = 330,
     TOK_COMMA = 331,
     TOK_AA = 332,
     TOK_EE = 333,
     TOK_AG = 334,
     TOK_EG = 335,
     TOK_AF = 336,
     TOK_EF = 337,
     TOK_AX = 338,
     TOK_EX = 339,
     TOK_RELEASES = 340,
     TOK_TRIGGERED = 341,
     TOK_UNTIL = 342,
     TOK_SINCE = 343,
     TOK_MMAX = 344,
     TOK_MMIN = 345,
     TOK_BUNTIL = 346,
     TOK_ABG = 347,
     TOK_ABF = 348,
     TOK_EBG = 349,
     TOK_EBF = 350,
     TOK_OP_FUTURE = 351,
     TOK_OP_GLOBAL = 352,
     TOK_OP_NEXT = 353,
     TOK_OP_ONCE = 354,
     TOK_OP_HISTORICAL = 355,
     TOK_OP_NOTPRECNOT = 356,
     TOK_OP_PREC = 357,
     TOK_GE = 358,
     TOK_LE = 359,
     TOK_GT = 360,
     TOK_LT = 361,
     TOK_NOTEQUAL = 362,
     TOK_EQUAL = 363,
     TOK_RROTATE = 364,
     TOK_LROTATE = 365,
     TOK_RSHIFT = 366,
     TOK_LSHIFT = 367,
     TOK_SETIN = 368,
     TOK_UNION = 369,
     TOK_DIVIDE = 370,
     TOK_TIMES = 371,
     TOK_MINUS = 372,
     TOK_PLUS = 373,
     TOK_MOD = 374,
     TOK_CONCATENATION = 375,
     TOK_SMALLINIT = 376,
     TOK_NEXT = 377,
     TOK_BIT = 378,
     TOK_DOT = 379,
     TOK_LB = 380,
     TOK_PLAYER_2 = 381,
     TOK_PLAYER_1 = 382,
     TOK_GAME = 383,
     TOK_AVOIDTARGET = 384,
     TOK_REACHTARGET = 385,
     TOK_AVOIDDEADLOCK = 386,
     TOK_REACHDEADLOCK = 387,
     TOK_GENREACTIVITY = 388,
     TOK_BUCHIGAME = 389,
     TOK_MIRROR = 390,
     TOK_PREDSLIST = 391,
     TOK_PRED = 392
   };
#endif
/* Tokens.  */
#define TOK_CONSTRAINT 258
#define TOK_GOTO 259
#define TOK_MAXU 260
#define TOK_MINU 261
#define TOK_ABU 262
#define TOK_EBU 263
#define TOK_AU 264
#define TOK_EU 265
#define TOK_CONTEXT 266
#define TOK_PROCESS 267
#define TOK_MODULE 268
#define TOK_COMPUTE 269
#define TOK_LTLSPEC 270
#define TOK_CTLSPEC 271
#define TOK_SPEC 272
#define TOK_INVAR 273
#define TOK_TRANS 274
#define TOK_INIT 275
#define TOK_DEFINE 276
#define TOK_IVAR 277
#define TOK_VAR 278
#define TOK_PSLSPEC 279
#define TOK_CONSTANTS 280
#define TOK_JUSTICE 281
#define TOK_COMPASSION 282
#define TOK_FAIRNESS 283
#define TOK_INVARSPEC 284
#define TOK_ASSIGN 285
#define TOK_ISA 286
#define TOK_SEMI 287
#define TOK_CONS 288
#define TOK_OF 289
#define TOK_RCB 290
#define TOK_LCB 291
#define TOK_RB 292
#define TOK_RP 293
#define TOK_LP 294
#define TOK_TWODOTS 295
#define TOK_EQDEF 296
#define TOK_SELF 297
#define TOK_COLON 298
#define TOK_ESAC 299
#define TOK_CASE 300
#define TOK_COMPWFF 301
#define TOK_CTLWFF 302
#define TOK_LTLPSL 303
#define TOK_LTLWFF 304
#define TOK_SIMPWFF 305
#define TOK_INCONTEXT 306
#define TOK_WORD 307
#define TOK_REAL 308
#define TOK_INTEGER 309
#define TOK_BOOLEAN 310
#define TOK_ARRAY 311
#define TOK_WORD1 312
#define TOK_BOOL 313
#define TOK_WAWRITE 314
#define TOK_WAREAD 315
#define TOK_TRUEEXP 316
#define TOK_FALSEEXP 317
#define TOK_ATOM 318
#define TOK_NUMBER_WORD 319
#define TOK_NUMBER_EXP 320
#define TOK_NUMBER_REAL 321
#define TOK_NUMBER_FRAC 322
#define TOK_NUMBER 323
#define TOK_NOT 324
#define TOK_AND 325
#define TOK_XNOR 326
#define TOK_XOR 327
#define TOK_OR 328
#define TOK_IFF 329
#define TOK_IMPLIES 330
#define TOK_COMMA 331
#define TOK_AA 332
#define TOK_EE 333
#define TOK_AG 334
#define TOK_EG 335
#define TOK_AF 336
#define TOK_EF 337
#define TOK_AX 338
#define TOK_EX 339
#define TOK_RELEASES 340
#define TOK_TRIGGERED 341
#define TOK_UNTIL 342
#define TOK_SINCE 343
#define TOK_MMAX 344
#define TOK_MMIN 345
#define TOK_BUNTIL 346
#define TOK_ABG 347
#define TOK_ABF 348
#define TOK_EBG 349
#define TOK_EBF 350
#define TOK_OP_FUTURE 351
#define TOK_OP_GLOBAL 352
#define TOK_OP_NEXT 353
#define TOK_OP_ONCE 354
#define TOK_OP_HISTORICAL 355
#define TOK_OP_NOTPRECNOT 356
#define TOK_OP_PREC 357
#define TOK_GE 358
#define TOK_LE 359
#define TOK_GT 360
#define TOK_LT 361
#define TOK_NOTEQUAL 362
#define TOK_EQUAL 363
#define TOK_RROTATE 364
#define TOK_LROTATE 365
#define TOK_RSHIFT 366
#define TOK_LSHIFT 367
#define TOK_SETIN 368
#define TOK_UNION 369
#define TOK_DIVIDE 370
#define TOK_TIMES 371
#define TOK_MINUS 372
#define TOK_PLUS 373
#define TOK_MOD 374
#define TOK_CONCATENATION 375
#define TOK_SMALLINIT 376
#define TOK_NEXT 377
#define TOK_BIT 378
#define TOK_DOT 379
#define TOK_LB 380
#define TOK_PLAYER_2 381
#define TOK_PLAYER_1 382
#define TOK_GAME 383
#define TOK_AVOIDTARGET 384
#define TOK_REACHTARGET 385
#define TOK_AVOIDDEADLOCK 386
#define TOK_REACHDEADLOCK 387
#define TOK_GENREACTIVITY 388
#define TOK_BUCHIGAME 389
#define TOK_MIRROR 390
#define TOK_PREDSLIST 391
#define TOK_PRED 392




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 113 "grammar.y"

  node_ptr node;
  int lineno;



/* Line 214 of yacc.c  */
#line 503 "grammar.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 515 "grammar.c"

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
# if YYENABLE_NLS
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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   894

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  291
/* YYNRULES -- Number of states.  */
#define YYNSTATES  577

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   392

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     8,    11,    12,    15,    17,
      20,    22,    25,    28,    30,    32,    34,    36,    40,    42,
      44,    46,    48,    50,    52,    54,    56,    59,    61,    63,
      67,    71,    76,    83,    87,    90,    95,   100,   105,   109,
     116,   125,   127,   130,   135,   137,   141,   143,   147,   151,
     153,   157,   161,   163,   167,   169,   173,   177,   179,   181,
     185,   187,   191,   193,   197,   199,   203,   205,   209,   213,
     217,   221,   225,   229,   231,   233,   236,   239,   242,   245,
     248,   251,   258,   265,   273,   281,   285,   289,   293,   297,
     300,   302,   306,   308,   312,   316,   320,   322,   326,   328,
     332,   334,   336,   338,   341,   344,   347,   350,   353,   356,
     359,   362,   364,   368,   372,   376,   380,   382,   386,   388,
     392,   396,   400,   402,   406,   408,   412,   414,   416,   418,
     420,   422,   424,   426,   428,   433,   435,   439,   450,   455,
     457,   459,   462,   464,   468,   470,   472,   474,   476,   478,
     482,   484,   488,   493,   498,   500,   504,   506,   508,   511,
     514,   518,   520,   524,   529,   531,   535,   536,   539,   541,
     543,   545,   547,   549,   551,   553,   555,   557,   559,   561,
     563,   565,   567,   569,   571,   573,   575,   577,   580,   581,
     584,   587,   590,   592,   594,   596,   598,   600,   602,   603,
     606,   608,   610,   612,   614,   616,   618,   620,   623,   625,
     628,   633,   635,   638,   640,   643,   648,   651,   652,   658,
     661,   662,   665,   670,   678,   686,   690,   694,   698,   702,
     706,   714,   718,   722,   726,   730,   732,   736,   737,   739,
     743,   745,   748,   752,   759,   763,   765,   767,   772,   777,
     781,   785,   792,   803,   807,   814,   821,   824,   825,   827,
     829,   833,   837,   842,   844,   846,   850,   854,   859,   861,
     864,   866,   870,   874,   878,   882,   886,   890,   896,   900,
     906,   910,   916,   920,   926,   929,   931,   935,   940,   942,
     946,   947
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     139,     0,    -1,    -1,   140,   190,    -1,    -1,   141,   240,
      -1,    -1,   142,   182,    -1,    68,    -1,   118,    68,    -1,
      68,    -1,   118,    68,    -1,   117,    68,    -1,    64,    -1,
      67,    -1,    66,    -1,    65,    -1,   144,    40,   144,    -1,
      62,    -1,    61,    -1,   143,    -1,   145,    -1,   146,    -1,
     148,    -1,   147,    -1,   150,    -1,   117,   151,    -1,    63,
      -1,    42,    -1,   151,   124,    63,    -1,   151,   124,    68,
      -1,   151,   125,   179,    37,    -1,   151,   125,   179,    43,
     179,    37,    -1,    39,   178,    38,    -1,    69,   151,    -1,
      58,    39,   178,    38,    -1,    57,    39,   178,    38,    -1,
     122,    39,   178,    38,    -1,    45,   152,    44,    -1,    60,
      39,   179,    76,   179,    38,    -1,    59,    39,   179,    76,
     179,    76,   179,    38,    -1,   153,    -1,   153,   152,    -1,
     178,    43,   178,    32,    -1,   151,    -1,   154,   120,   151,
      -1,   154,    -1,   155,   116,   154,    -1,   155,   115,   154,
      -1,   155,    -1,   156,   118,   155,    -1,   156,   117,   155,
      -1,   156,    -1,   157,   119,   156,    -1,   157,    -1,   158,
     112,   157,    -1,   158,   111,   157,    -1,   158,    -1,   149,
      -1,    36,   160,    35,    -1,   178,    -1,   160,    76,   178,
      -1,   159,    -1,   161,   114,   159,    -1,   161,    -1,   162,
     113,   161,    -1,   162,    -1,   163,   108,   162,    -1,   163,
     107,   162,    -1,   163,   106,   162,    -1,   163,   105,   162,
      -1,   163,   104,   162,    -1,   163,   103,   162,    -1,   163,
      -1,   165,    -1,    84,   164,    -1,    83,   164,    -1,    82,
     164,    -1,    81,   164,    -1,    80,   164,    -1,    79,   164,
      -1,    77,   125,   170,    87,   170,    37,    -1,    78,   125,
     170,    87,   170,    37,    -1,    77,   125,   170,    91,   149,
     170,    37,    -1,    78,   125,   170,    91,   149,   170,    37,
      -1,    95,   149,   164,    -1,    93,   149,   164,    -1,    94,
     149,   164,    -1,    92,   149,   164,    -1,    69,   165,    -1,
     164,    -1,   166,    70,   164,    -1,   166,    -1,   167,    73,
     166,    -1,   167,    72,   166,    -1,   167,    71,   166,    -1,
     167,    -1,   168,    74,   167,    -1,   168,    -1,   168,    75,
     169,    -1,   169,    -1,   164,    -1,   172,    -1,    98,   171,
      -1,   102,   171,    -1,   101,   171,    -1,    97,   171,    -1,
     100,   171,    -1,    96,   171,    -1,    99,   171,    -1,    69,
     172,    -1,   171,    -1,   173,    87,   171,    -1,   173,    88,
     171,    -1,   173,    85,   171,    -1,   173,    86,   171,    -1,
     173,    -1,   174,    70,   173,    -1,   174,    -1,   175,    73,
     174,    -1,   175,    72,   174,    -1,   175,    71,   174,    -1,
     175,    -1,   176,    74,   175,    -1,   176,    -1,   176,    75,
     177,    -1,   177,    -1,   178,    -1,   178,    -1,   178,    -1,
     178,    -1,    55,    -1,    54,    -1,    53,    -1,    52,   125,
     179,    37,    -1,   149,    -1,    36,   185,    35,    -1,    56,
      52,   125,   179,    37,    34,    52,   125,   179,    37,    -1,
      56,   149,    34,   183,    -1,   183,    -1,   188,    -1,    12,
     188,    -1,   186,    -1,   185,    76,   186,    -1,   187,    -1,
     144,    -1,    62,    -1,    61,    -1,    63,    -1,   187,   124,
      63,    -1,    63,    -1,    63,    39,    38,    -1,    63,    39,
     189,    38,    -1,    56,   149,    34,   188,    -1,   179,    -1,
     189,    76,   179,    -1,   191,    -1,   196,    -1,   190,   191,
      -1,   190,   196,    -1,    13,   192,   194,    -1,    63,    -1,
      63,    39,    38,    -1,    63,    39,   193,    38,    -1,    63,
      -1,   193,    76,    63,    -1,    -1,   194,   195,    -1,   236,
      -1,   201,    -1,   204,    -1,   209,    -1,   212,    -1,   213,
      -1,   214,    -1,   207,    -1,   215,    -1,   216,    -1,   217,
      -1,   218,    -1,   219,    -1,   220,    -1,   221,    -1,   234,
      -1,   222,    -1,   225,    -1,   226,    -1,   128,   197,    -1,
      -1,   198,   197,    -1,   127,   199,    -1,   126,   199,    -1,
     228,    -1,   229,    -1,   230,    -1,   231,    -1,   232,    -1,
     233,    -1,    -1,   199,   200,    -1,   201,    -1,   209,    -1,
     212,    -1,   213,    -1,   214,    -1,   207,    -1,    23,    -1,
      23,   202,    -1,   203,    -1,   202,   203,    -1,   238,    43,
     184,    32,    -1,    22,    -1,    22,   205,    -1,   206,    -1,
     205,   206,    -1,   238,    43,   183,    32,    -1,    21,   208,
      -1,    -1,   208,   238,    41,   179,    32,    -1,    30,   210,
      -1,    -1,   210,   211,    -1,   239,    41,   179,    32,    -1,
     121,    39,   239,    38,    41,   179,    32,    -1,   122,    39,
     239,    38,    41,   180,    32,    -1,    20,   179,   237,    -1,
      18,   179,   237,    -1,    19,   180,   237,    -1,    28,   179,
     237,    -1,    26,   179,   237,    -1,    27,    39,   179,    76,
     179,    38,   237,    -1,    29,   179,   237,    -1,    17,   181,
     237,    -1,    16,   181,   237,    -1,    15,   182,   237,    -1,
      24,    -1,    25,   223,    32,    -1,    -1,   187,    -1,   223,
      76,   187,    -1,   225,    -1,   224,   225,    -1,   137,   179,
     237,    -1,   137,   125,    68,    37,   179,   237,    -1,   135,
     238,   237,    -1,   127,    -1,   126,    -1,   130,   227,   179,
     237,    -1,   129,   227,   179,   237,    -1,   132,   227,   237,
      -1,   131,   227,   237,    -1,   134,   227,    39,   189,    38,
     237,    -1,   133,   227,    39,   189,    38,    75,    39,   189,
      38,   237,    -1,    14,   235,   237,    -1,    90,   125,   181,
      76,   181,    37,    -1,    89,   125,   181,    76,   181,    37,
      -1,    31,    63,    -1,    -1,    32,    -1,    63,    -1,   238,
     124,    63,    -1,   238,   124,    68,    -1,   238,   125,   144,
      37,    -1,    63,    -1,    42,    -1,   239,   124,    63,    -1,
     239,   124,    68,    -1,   239,   125,   179,    37,    -1,   241,
      -1,     1,    32,    -1,     1,    -1,     4,   244,    32,    -1,
      20,   179,    32,    -1,    28,   179,    32,    -1,    19,   180,
      32,    -1,     3,   179,    32,    -1,    50,   179,    32,    -1,
      50,   179,    51,   242,    32,    -1,    47,   181,    32,    -1,
      47,   181,    51,   242,    32,    -1,    49,   182,    32,    -1,
      49,   182,    51,   242,    32,    -1,    46,   235,    32,    -1,
      46,   235,    51,   242,    32,    -1,   136,   224,    -1,    63,
      -1,   242,   124,    63,    -1,   242,   125,   179,    37,    -1,
      68,    -1,   243,   124,    68,    -1,    -1,   245,   243,   124,
      68,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   213,   213,   213,   243,   243,   250,   250,   265,   266,
     269,   270,   271,   275,   278,   280,   282,   285,   289,   290,
     291,   292,   293,   302,   311,   329,   330,   331,   332,   333,
     341,   349,   357,   362,   363,   364,   365,   366,   367,   369,
     372,   379,   381,   385,   390,   391,   395,   396,   397,   401,
     402,   403,   407,   408,   411,   412,   413,   419,   420,   421,
     424,   425,   429,   430,   433,   434,   438,   439,   440,   441,
     442,   443,   444,   447,   448,   452,   453,   454,   455,   456,
     457,   458,   460,   462,   464,   466,   467,   468,   469,   472,
     478,   479,   482,   483,   484,   485,   488,   489,   493,   494,
     497,   501,   502,   507,   508,   509,   510,   511,   512,   513,
     515,   520,   521,   523,   525,   532,   542,   543,   547,   548,
     549,   550,   554,   555,   559,   560,   563,   569,   572,   575,
     578,   588,   589,   597,   605,   607,   608,   610,   612,   617,
     618,   619,   623,   624,   627,   628,   629,   630,   633,   634,
     637,   638,   639,   641,   646,   647,   662,   663,   664,   665,
     668,   671,   672,   673,   676,   677,   682,   683,   685,   686,
     687,   688,   689,   690,   691,   692,   693,   694,   695,   696,
     697,   698,   699,   700,   701,   702,   703,   713,   738,   739,
     747,   763,   784,   785,   786,   787,   788,   789,   795,   796,
     800,   802,   803,   804,   805,   806,   818,   819,   821,   822,
     824,   826,   827,   829,   830,   832,   836,   838,   839,   843,
     845,   846,   848,   850,   855,   863,   865,   867,   871,   874,
     877,   882,   885,   886,   889,   892,   901,   906,   907,   908,
     912,   913,   916,   927,   938,   949,   950,   952,   955,   958,
     961,   965,   972,   981,   984,   986,   991,   995,   995,  1003,
    1004,  1005,  1006,  1009,  1010,  1011,  1012,  1013,  1021,  1022,
    1023,  1026,  1028,  1030,  1032,  1034,  1036,  1040,  1044,  1048,
    1052,  1056,  1060,  1064,  1068,  1080,  1081,  1082,  1085,  1086,
    1089,  1089
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_CONSTRAINT", "TOK_GOTO", "TOK_MAXU",
  "TOK_MINU", "TOK_ABU", "TOK_EBU", "TOK_AU", "TOK_EU", "TOK_CONTEXT",
  "TOK_PROCESS", "TOK_MODULE", "TOK_COMPUTE", "TOK_LTLSPEC", "TOK_CTLSPEC",
  "TOK_SPEC", "TOK_INVAR", "TOK_TRANS", "TOK_INIT", "TOK_DEFINE",
  "TOK_IVAR", "TOK_VAR", "TOK_PSLSPEC", "TOK_CONSTANTS", "TOK_JUSTICE",
  "TOK_COMPASSION", "TOK_FAIRNESS", "TOK_INVARSPEC", "TOK_ASSIGN",
  "TOK_ISA", "TOK_SEMI", "TOK_CONS", "TOK_OF", "TOK_RCB", "TOK_LCB",
  "TOK_RB", "TOK_RP", "TOK_LP", "TOK_TWODOTS", "TOK_EQDEF", "TOK_SELF",
  "TOK_COLON", "TOK_ESAC", "TOK_CASE", "TOK_COMPWFF", "TOK_CTLWFF",
  "TOK_LTLPSL", "TOK_LTLWFF", "TOK_SIMPWFF", "TOK_INCONTEXT", "TOK_WORD",
  "TOK_REAL", "TOK_INTEGER", "TOK_BOOLEAN", "TOK_ARRAY", "TOK_WORD1",
  "TOK_BOOL", "TOK_WAWRITE", "TOK_WAREAD", "TOK_TRUEEXP", "TOK_FALSEEXP",
  "TOK_ATOM", "TOK_NUMBER_WORD", "TOK_NUMBER_EXP", "TOK_NUMBER_REAL",
  "TOK_NUMBER_FRAC", "TOK_NUMBER", "TOK_NOT", "TOK_AND", "TOK_XNOR",
  "TOK_XOR", "TOK_OR", "TOK_IFF", "TOK_IMPLIES", "TOK_COMMA", "TOK_AA",
  "TOK_EE", "TOK_AG", "TOK_EG", "TOK_AF", "TOK_EF", "TOK_AX", "TOK_EX",
  "TOK_RELEASES", "TOK_TRIGGERED", "TOK_UNTIL", "TOK_SINCE", "TOK_MMAX",
  "TOK_MMIN", "TOK_BUNTIL", "TOK_ABG", "TOK_ABF", "TOK_EBG", "TOK_EBF",
  "TOK_OP_FUTURE", "TOK_OP_GLOBAL", "TOK_OP_NEXT", "TOK_OP_ONCE",
  "TOK_OP_HISTORICAL", "TOK_OP_NOTPRECNOT", "TOK_OP_PREC", "TOK_GE",
  "TOK_LE", "TOK_GT", "TOK_LT", "TOK_NOTEQUAL", "TOK_EQUAL", "TOK_RROTATE",
  "TOK_LROTATE", "TOK_RSHIFT", "TOK_LSHIFT", "TOK_SETIN", "TOK_UNION",
  "TOK_DIVIDE", "TOK_TIMES", "TOK_MINUS", "TOK_PLUS", "TOK_MOD",
  "TOK_CONCATENATION", "TOK_SMALLINIT", "TOK_NEXT", "TOK_BIT", "TOK_DOT",
  "TOK_LB", "TOK_PLAYER_2", "TOK_PLAYER_1", "TOK_GAME", "TOK_AVOIDTARGET",
  "TOK_REACHTARGET", "TOK_AVOIDDEADLOCK", "TOK_REACHDEADLOCK",
  "TOK_GENREACTIVITY", "TOK_BUCHIGAME", "TOK_MIRROR", "TOK_PREDSLIST",
  "TOK_PRED", "$accept", "begin", "$@1", "$@2", "$@3", "number", "integer",
  "number_word", "number_frac", "number_real", "number_exp", "subrange",
  "constant", "primary_expr", "case_element_list_expr",
  "case_element_expr", "concatination_expr", "multiplicative_expr",
  "additive_expr", "remainder_expr", "shift_expr", "set_expr",
  "set_list_expr", "union_expr", "in_expr", "relational_expr", "ctl_expr",
  "pure_ctl_expr", "ctl_and_expr", "ctl_or_expr", "ctl_iff_expr",
  "ctl_implies_expr", "ctl_basic_expr", "ltl_unary_expr",
  "pure_ltl_unary_expr", "ltl_binary_expr", "and_expr", "or_expr",
  "iff_expr", "implies_expr", "basic_expr", "simple_expression",
  "next_expression", "ctl_expression", "ltl_expression", "itype", "type",
  "type_value_list", "type_value", "complex_atom", "module_type",
  "simple_list_expression", "module_list", "module", "module_sign",
  "atom_list", "declarations", "declaration", "game_definition",
  "game_body", "game_body_element", "player_body", "player_body_element",
  "var", "var_decl_list", "var_decl", "input_var", "ivar_decl_list",
  "ivar_decl", "define", "define_list", "assign", "assign_list",
  "one_assign", "init", "invar", "trans", "fairness", "justice",
  "compassion", "invarspec", "ctlspec", "ltlspec", "pslspec", "constants",
  "constants_expression", "predicate_list", "predicate", "mirror",
  "player_num", "reachtarget", "avoidtarget", "reachdeadlock",
  "avoiddeadlock", "buchigame", "genreactivity", "compute",
  "compute_expression", "isa", "optsemi", "decl_var_id", "var_id",
  "command", "command_case", "context", "trace", "state", "$@4", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   138,   140,   139,   141,   139,   142,   139,   143,   143,
     144,   144,   144,   145,   146,   147,   148,   149,   150,   150,
     150,   150,   150,   150,   150,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   152,   152,   153,   154,   154,   155,   155,   155,   156,
     156,   156,   157,   157,   158,   158,   158,   159,   159,   159,
     160,   160,   161,   161,   162,   162,   163,   163,   163,   163,
     163,   163,   163,   164,   164,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     166,   166,   167,   167,   167,   167,   168,   168,   169,   169,
     170,   171,   171,   172,   172,   172,   172,   172,   172,   172,
     172,   173,   173,   173,   173,   173,   174,   174,   175,   175,
     175,   175,   176,   176,   177,   177,   178,   179,   180,   181,
     182,   183,   183,   183,   183,   183,   183,   183,   183,   184,
     184,   184,   185,   185,   186,   186,   186,   186,   187,   187,
     188,   188,   188,   188,   189,   189,   190,   190,   190,   190,
     191,   192,   192,   192,   193,   193,   194,   194,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   196,   197,   197,
     198,   198,   198,   198,   198,   198,   198,   198,   199,   199,
     200,   200,   200,   200,   200,   200,   201,   201,   202,   202,
     203,   204,   204,   205,   205,   206,   207,   208,   208,   209,
     210,   210,   211,   211,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   219,   220,   221,   222,   223,   223,   223,
     224,   224,   225,   225,   226,   227,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   235,   236,   237,   237,   238,
     238,   238,   238,   239,   239,   239,   239,   239,   240,   240,
     240,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   242,   242,   242,   243,   243,
     245,   244
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     0,     2,     1,     2,
       1,     2,     2,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     3,
       3,     4,     6,     3,     2,     4,     4,     4,     3,     6,
       8,     1,     2,     4,     1,     3,     1,     3,     3,     1,
       3,     3,     1,     3,     1,     3,     3,     1,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     3,     3,
       3,     3,     3,     1,     1,     2,     2,     2,     2,     2,
       2,     6,     6,     7,     7,     3,     3,     3,     3,     2,
       1,     3,     1,     3,     3,     3,     1,     3,     1,     3,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     3,     3,     3,     3,     1,     3,     1,     3,
       3,     3,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     1,     3,    10,     4,     1,
       1,     2,     1,     3,     1,     1,     1,     1,     1,     3,
       1,     3,     4,     4,     1,     3,     1,     1,     2,     2,
       3,     1,     3,     4,     1,     3,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     0,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       4,     1,     2,     1,     2,     4,     2,     0,     5,     2,
       0,     2,     4,     7,     7,     3,     3,     3,     3,     3,
       7,     3,     3,     3,     3,     1,     3,     0,     1,     3,
       1,     2,     3,     6,     3,     1,     1,     4,     4,     3,
       3,     6,    10,     3,     6,     6,     2,     0,     1,     1,
       3,     3,     4,     1,     1,     3,     3,     4,     1,     2,
       1,     3,     3,     3,     3,     3,     3,     5,     3,     5,
       3,     5,     3,     5,     2,     1,     3,     4,     1,     3,
       0,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       6,     0,     0,     0,     0,     1,     0,   188,     3,   156,
     157,   270,     0,   290,     0,     0,     0,     0,     0,     0,
       0,     0,     5,   268,     0,     0,    28,     0,     0,     0,
       0,     0,    19,    18,    27,    13,    16,    15,    14,     8,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    20,     0,    21,    22,    24,    23,    58,
      25,    44,    46,    49,    52,    54,    57,    62,    64,    66,
      73,   101,    74,   111,   102,   116,   118,   122,   124,   126,
     130,     7,   161,   166,   198,   198,     0,     0,     0,     0,
       0,     0,   187,   188,   192,   193,   194,   195,   196,   197,
     158,   159,   269,   127,     0,     0,     0,   128,     0,     0,
       0,     0,     0,     0,   129,     0,     0,     0,     0,   284,
     240,     0,    60,     0,     0,    41,     0,     0,     0,     0,
       0,     8,     0,     0,    34,    89,   110,     0,     0,     0,
      80,    79,    78,    77,    76,    75,    10,     0,     0,     0,
       0,     0,     0,   108,   106,   103,   109,   107,   105,   104,
       8,     0,    26,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   160,   191,   190,   246,   245,
       0,     0,   257,   257,     0,     0,   189,   275,   271,   288,
       0,   274,   272,   273,     0,     0,   282,     0,   278,     0,
     280,     0,   276,     0,     0,   257,   241,    59,     0,    33,
      38,    42,     0,     0,     0,     0,     0,     9,    90,    92,
      96,    98,   100,     0,     0,    12,    11,    88,    86,    87,
      85,     0,    17,    29,    30,     0,    45,    48,    47,    51,
      50,    53,    56,    55,    63,    65,    72,    71,    70,    69,
      68,    67,   114,   115,   112,   113,   117,   121,   120,   119,
     123,   125,   162,   164,     0,     0,     0,     0,     0,     0,
       0,     0,   217,   211,   206,   235,   237,     0,     0,     0,
       0,   220,     0,     0,   167,   169,   170,   175,   171,   172,
     173,   174,   176,   177,   178,   179,   180,   181,   182,   184,
     185,   186,   183,   168,   199,   200,   205,   201,   202,   203,
     204,   257,   257,   258,   250,   249,     0,     0,     0,     0,
       0,   285,     0,     0,     0,     0,     0,   242,    61,     0,
      36,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    37,    31,     0,   163,     0,   257,
     257,   257,   257,   257,   257,   257,   216,   259,   212,   213,
       0,   207,   208,     0,   148,   238,     0,   257,     0,   257,
     257,   219,   256,   257,   248,   247,   154,     0,     0,   289,
       0,     0,   283,     0,     0,   279,   281,   277,     0,    43,
       0,     0,    91,    95,    94,    93,    97,    99,     0,     0,
       0,     0,     0,   165,   253,   234,   233,   232,   226,   227,
     225,     0,   214,     0,     0,     0,   209,     0,     0,   236,
       0,   229,     0,   228,   231,   264,   263,     0,     0,   221,
       0,   244,     0,     0,   257,     0,     0,   286,     0,   257,
       0,    39,    81,     0,    82,     0,    32,     0,     0,     0,
     133,   132,   131,     0,   135,     0,   260,   261,     0,     0,
       0,   150,   139,     0,   140,   149,   239,     0,     0,     0,
       0,     0,     0,     0,   155,   251,   255,   254,   287,   243,
       0,    83,    84,     0,   147,   146,   145,     0,   142,   144,
       0,     0,     0,   215,   262,     0,   141,     0,     0,   210,
       0,     0,     0,     0,   265,   266,     0,     0,    40,   218,
     136,     0,     0,     0,     0,     0,     0,   151,     0,   257,
       0,     0,   222,   267,     0,   143,   134,     0,   138,     0,
     153,   152,   230,     0,     0,   257,     0,     0,     0,   252,
       0,   223,   224,     0,     0,     0,   137
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     3,     4,    63,    64,    65,    66,    67,
      68,    69,    70,    71,   134,   135,    72,    73,    74,    75,
      76,    77,   131,    78,    79,    80,    81,    82,   249,   250,
     251,   252,   253,    83,    84,    85,    86,    87,    88,    89,
     113,   406,   118,   125,    91,   558,   493,   517,   518,   519,
     560,   407,     8,     9,    93,   294,   205,   314,    10,   102,
     103,   206,   334,   335,   391,   392,   316,   388,   389,   336,
     386,   337,   401,   459,   338,   339,   340,   322,   323,   324,
     325,   326,   327,   328,   329,   396,   129,   130,   331,   210,
     104,   105,   106,   107,   108,   109,   332,   123,   333,   344,
     390,   460,    22,    23,   352,   220,   115,   116
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -433
static const yytype_int16 yypact[] =
{
      45,    29,     6,    83,   613,  -433,    -2,   104,     6,  -433,
    -433,    46,   613,  -433,   613,   613,   613,   115,   613,   613,
     613,   -29,  -433,  -433,   613,   613,  -433,   613,   130,   152,
     190,   201,  -433,  -433,  -433,  -433,  -433,  -433,  -433,   139,
     726,    93,   118,   680,   680,   680,   680,   680,   680,    96,
      96,    96,    96,   613,   613,   613,   613,   613,   613,   613,
     275,   178,   227,  -433,   229,  -433,  -433,  -433,  -433,  -433,
    -433,    69,   171,   200,   231,   174,   254,  -433,   182,   189,
     170,  -433,  -433,  -433,  -433,   360,   243,   208,   301,  -433,
    -433,  -433,   279,  -433,  -433,  -433,   261,   261,   261,   261,
     261,   261,  -433,   104,  -433,  -433,  -433,  -433,  -433,  -433,
    -433,  -433,  -433,  -433,   299,   314,   292,  -433,   331,   340,
     345,   245,   278,    11,  -433,    58,   103,   138,   477,   -29,
    -433,     5,  -433,   362,   361,   613,   372,   613,   613,   613,
     613,  -433,   410,   348,    69,  -433,  -433,   680,   680,   772,
    -433,  -433,  -433,  -433,  -433,  -433,  -433,   351,   373,   680,
     680,   680,   680,  -433,  -433,  -433,  -433,  -433,  -433,  -433,
     400,   410,    69,   402,   613,    96,   149,   613,   410,   410,
     410,   410,   410,   410,   410,   410,   365,   365,   365,   365,
     365,   365,   365,   365,   613,   613,   613,   613,   613,   613,
     613,   613,   613,   613,    51,   234,   202,   202,  -433,  -433,
     613,   613,   418,   418,   414,   415,  -433,  -433,  -433,  -433,
     332,  -433,  -433,  -433,   613,   613,  -433,   396,  -433,   396,
    -433,   396,  -433,   396,   392,   418,  -433,  -433,   613,  -433,
    -433,  -433,   613,   423,   424,   387,   390,  -433,  -433,   411,
     250,   316,  -433,    89,   124,  -433,  -433,  -433,  -433,  -433,
    -433,   442,  -433,  -433,  -433,     8,    69,   171,   171,   200,
     200,   231,   174,   174,  -433,   182,   189,   189,   189,   189,
     189,   189,  -433,  -433,  -433,  -433,   360,   243,   243,   243,
     208,  -433,  -433,  -433,     4,   115,   613,   613,   613,   613,
     613,   613,  -433,   421,   421,  -433,   426,   613,   447,   613,
     613,  -433,   428,   421,  -433,  -433,  -433,  -433,  -433,  -433,
    -433,  -433,  -433,  -433,  -433,  -433,  -433,  -433,  -433,  -433,
    -433,  -433,  -433,  -433,  -433,  -433,  -433,  -433,  -433,  -433,
    -433,   418,   418,  -433,  -433,  -433,   613,   613,   425,   416,
     419,  -433,   -18,    -1,    18,    22,   457,  -433,  -433,   464,
    -433,  -433,   613,   613,   680,   680,   680,   680,   680,   680,
     680,    96,   680,    96,  -433,  -433,   613,  -433,   434,   418,
     418,   418,   418,   418,   418,   418,   421,  -433,   421,  -433,
      31,   421,  -433,    42,  -433,   374,    20,   418,   613,   418,
     418,   -22,  -433,    28,  -433,  -433,  -433,    17,    63,   468,
     613,   613,  -433,   438,   613,  -433,  -433,  -433,   613,  -433,
     427,   466,  -433,   411,   411,   411,   250,  -433,   465,   680,
     469,   680,   470,  -433,  -433,  -433,  -433,  -433,  -433,  -433,
    -433,    34,  -433,   633,   176,    96,  -433,   256,   446,  -433,
     426,  -433,   429,  -433,  -433,  -433,  -433,   471,   472,  -433,
      47,  -433,   437,   613,   418,   478,   480,  -433,   484,   418,
     613,  -433,  -433,   486,  -433,   487,  -433,   613,    57,   389,
    -433,  -433,  -433,    -5,  -433,   493,  -433,  -433,   489,    54,
      -5,   490,  -433,   498,  -433,  -433,   374,   613,    35,    35,
     613,   179,   613,   494,  -433,  -433,  -433,  -433,  -433,  -433,
     509,  -433,  -433,   516,  -433,  -433,  -433,    21,  -433,   374,
     613,   439,   515,  -433,  -433,    96,  -433,   518,   546,  -433,
     512,   -20,    38,   521,  -433,  -433,   526,   613,  -433,  -433,
    -433,    57,   528,   613,   633,   532,   326,  -433,    84,   418,
     527,   539,  -433,  -433,   140,  -433,  -433,   544,  -433,    54,
    -433,  -433,  -433,   613,   613,   418,   549,   554,   555,  -433,
     537,  -433,  -433,   467,   613,   553,  -433
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -433,  -433,  -433,  -433,  -433,  -433,  -174,  -433,  -433,  -433,
    -433,   -26,  -433,    19,   458,  -433,   216,   217,   413,   224,
    -433,   412,  -433,   430,   137,  -433,   -11,   -28,   -60,   232,
    -433,   228,  -146,    13,   561,   420,   184,   417,  -433,   431,
       3,   -12,  -294,  -215,   -14,  -153,  -433,  -433,    75,  -293,
    -432,  -336,  -433,   612,  -433,  -433,  -433,  -433,   614,   529,
    -433,   536,  -433,   432,  -433,   230,  -433,  -433,   247,   445,
    -433,   448,  -433,  -433,   449,   451,   452,  -433,  -433,  -433,
    -433,  -433,  -433,  -433,  -433,  -433,  -433,   -90,  -433,   338,
    -433,  -433,  -433,  -433,  -433,  -433,  -433,   341,  -433,  -197,
    -260,   -87,  -433,  -433,    55,  -433,  -433,  -433
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -292
static const yytype_int16 yytable[] =
{
     114,   262,   254,   119,   120,   126,   384,    90,   127,   349,
     350,   408,   145,   395,   412,   494,   345,   117,   550,     6,
     455,   124,    90,   159,   160,   161,   162,   132,   133,     5,
     136,   415,   150,   151,   152,   153,   154,   155,   357,   236,
     237,   456,   377,   226,   393,   375,    -4,   521,    -4,    -4,
     416,   376,   449,   403,   417,   462,   540,   526,    -2,   144,
     343,    92,   227,   156,    -4,    -4,   163,   164,   165,   166,
     167,   168,   169,    -4,   443,   477,   551,   455,   112,   172,
     378,   238,   381,   382,    11,   447,    12,    13,   500,   292,
     228,    -4,    -4,   463,    -4,    -4,   450,   541,   456,   457,
     458,   464,    14,    15,   501,   502,   413,   414,   128,   229,
     525,    16,   157,   158,   293,   330,   235,   491,   514,   515,
     394,   145,   561,   413,   414,   156,   441,   245,   246,    17,
      18,   393,    19,    20,     7,   230,   248,   248,   136,   463,
     243,   244,   413,   414,   404,   405,   413,   414,   257,   258,
     259,   260,   444,   445,   231,   444,   445,   496,   444,   445,
     463,   172,   501,   502,   156,   265,   444,   445,   144,   137,
     232,   501,   502,    -2,   157,   158,   370,   261,   565,   -10,
     371,    -4,   434,   435,   436,   437,   438,   439,   440,   233,
     144,   138,   548,   176,   177,   465,   466,   266,   341,   342,
     451,   554,   453,   454,   121,   122,   461,   282,   283,   284,
     285,   372,   263,   157,   158,   373,   463,   264,   147,    21,
     299,   300,   301,   302,   428,   304,   430,   124,   124,   139,
      94,    95,   311,    96,    97,    98,    99,   100,   101,   486,
     140,   358,   534,   148,   487,   359,   173,   535,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   174,   505,   489,   175,
     568,   488,   509,   188,   189,   190,   191,   192,   193,   199,
     200,   201,   380,   473,   353,   475,   354,   383,   355,   385,
     485,   178,   478,   183,   492,   397,   186,   399,   400,    90,
     124,   124,   187,   117,   516,   423,   424,   425,   479,   480,
     481,   482,   490,   198,    25,   179,   180,    26,   204,   491,
      27,   365,   366,   367,   156,   276,   277,   278,   279,   280,
     281,   217,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,   170,   171,   429,   218,   431,   181,   182,
     420,   421,   562,   422,   248,   248,   248,   248,   248,   248,
     219,   248,   478,   221,   432,   184,   185,   516,   569,   313,
     224,   128,   222,   157,   158,   202,   203,   223,   479,   480,
     481,   482,   490,   287,   288,   289,   452,   208,   209,   491,
     368,   369,   142,   143,   156,   267,   268,    62,   269,   270,
     239,    24,   468,   225,    25,   240,   469,    26,   272,   273,
      27,   531,   532,   124,   124,   242,   247,   484,   248,   255,
     248,   484,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,   171,   211,   212,   213,   214,   215,
     -12,   256,   -11,   157,   158,   194,   195,   196,   197,    25,
     343,   504,    26,   346,   347,    27,   348,   522,   510,   351,
     356,   360,   361,   362,   527,   513,   363,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,   141,   171,
     374,   364,    60,    61,   387,   530,   398,    62,   533,   394,
     536,   402,   410,   409,   418,   411,   419,   433,   448,   545,
    -291,   467,   472,   470,   471,   497,   474,   476,   542,   495,
     498,   499,   503,    24,   520,   506,    25,   507,   484,    26,
     484,   508,    27,   511,   512,   523,   524,   142,   143,   528,
     529,   557,    62,   537,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,   538,   539,   544,
     549,   567,   546,   552,    41,    42,    43,    44,    45,    46,
      47,    48,   575,   553,   543,   556,   559,   117,   563,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
     564,   566,    24,   570,   547,    25,   571,   572,    26,   573,
     576,    27,   574,   241,    60,    61,   271,   427,   274,    62,
     426,   146,   234,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,   555,   275,   286,   290,
     110,   446,   111,    41,    42,    43,    44,    45,    46,    47,
      48,   207,   216,     0,   291,   442,   379,   315,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    24,
     317,     0,    25,   318,   319,    26,   320,   321,    27,     0,
       0,     0,     0,    60,    61,     0,     0,     0,    62,   478,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,     0,   479,   480,   481,   482,   483,
      41,    42,    43,    44,    45,    46,    47,    48,     0,     0,
       0,   156,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    24,     0,     0,    25,
       0,     0,    26,     0,     0,    27,     0,     0,     0,     0,
      60,    61,     0,     0,     0,    62,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,   149,
     157,   158,     0,     0,     0,     0,     0,    41,    42,    43,
      44,    45,    46,    47,    48,    25,     0,     0,    26,     0,
       0,    27,    49,    50,    51,    52,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,   141,    40,     0,    60,    61,     0,
       0,     0,    62,    41,    42,    43,    44,    45,    46,    47,
      48,    25,     0,     0,    26,     0,     0,    27,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
     141,   149,     0,   142,   143,     0,     0,     0,    62,    41,
      42,    43,    44,    45,    46,    47,    48,     0,     0,     0,
       0,     0,     0,     0,    49,    50,    51,    52,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   142,
     143,     0,     0,     0,    62
};

static const yytype_int16 yycheck[] =
{
      12,   175,   148,    15,    16,    19,   300,     4,    20,   224,
     225,   347,    40,   306,    32,   447,   213,    14,    38,    13,
      42,    18,    19,    49,    50,    51,    52,    24,    25,     0,
      27,    32,    43,    44,    45,    46,    47,    48,   235,   129,
      35,    63,    38,    32,   304,    37,     1,    52,     3,     4,
      32,    43,    32,   313,    32,    38,    35,   489,    13,    40,
      32,    63,    51,    68,    19,    20,    53,    54,    55,    56,
      57,    58,    59,    28,    43,    41,    38,    42,    32,    60,
      76,    76,   297,   298,     1,    43,     3,     4,    41,    38,
      32,    46,    47,    76,    49,    50,    76,    76,    63,   121,
     122,    38,    19,    20,   124,   125,   124,   125,   137,    51,
      56,    28,   117,   118,    63,   205,   128,    63,    61,    62,
      63,   149,    38,   124,   125,    68,   386,   139,   140,    46,
      47,   391,    49,    50,   128,    32,   147,   148,   135,    76,
     137,   138,   124,   125,   341,   342,   124,   125,   159,   160,
     161,   162,   124,   125,    51,   124,   125,   450,   124,   125,
      76,   142,   124,   125,    68,   177,   124,   125,   149,    39,
      32,   124,   125,   128,   117,   118,    87,   174,    38,    40,
      91,   136,   379,   380,   381,   382,   383,   384,   385,    51,
     171,    39,   528,   124,   125,   410,   411,   178,   210,   211,
     397,   537,   399,   400,    89,    90,   403,   194,   195,   196,
     197,    87,    63,   117,   118,    91,    76,    68,   125,   136,
      18,    19,    20,    21,   370,    23,   372,   224,   225,    39,
     126,   127,    30,   129,   130,   131,   132,   133,   134,    63,
      39,   238,    63,   125,    68,   242,    68,    68,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    39,   464,    12,    40,
     564,   445,   469,   103,   104,   105,   106,   107,   108,    71,
      72,    73,   296,   429,   229,   431,   231,   299,   233,   301,
     443,   120,    36,   119,   447,   307,   114,   309,   310,   296,
     297,   298,   113,   300,   478,   365,   366,   367,    52,    53,
      54,    55,    56,    70,    39,   115,   116,    42,    39,    63,
      45,    71,    72,    73,    68,   188,   189,   190,   191,   192,
     193,    32,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,   371,    32,   373,   117,   118,
     362,   363,   549,   364,   365,   366,   367,   368,   369,   370,
      68,   372,    36,    32,   376,   111,   112,   541,   565,   135,
     125,   137,    32,   117,   118,    74,    75,    32,    52,    53,
      54,    55,    56,   199,   200,   201,   398,   126,   127,    63,
      74,    75,   117,   118,    68,   179,   180,   122,   181,   182,
      38,    36,   414,   125,    39,    44,   418,    42,   184,   185,
      45,   498,   499,   410,   411,    43,    68,   443,   429,    68,
     431,   447,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    97,    98,    99,   100,   101,
      40,    68,    40,   117,   118,    85,    86,    87,    88,    39,
      32,   463,    42,    39,    39,    45,   124,   483,   470,    63,
      68,    38,    38,    76,   490,   477,    76,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      38,    70,   117,   118,    63,   497,    39,   122,   500,    63,
     502,    63,    76,    68,    37,    76,    32,    63,   124,   525,
      32,    63,    37,    76,    38,    76,    37,    37,   520,    63,
      39,    39,    75,    36,   125,    37,    39,    37,   544,    42,
     546,    37,    45,    37,    37,    32,    37,   117,   118,    39,
      32,   543,   122,    39,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    38,    32,    34,
      38,   563,    34,    32,    77,    78,    79,    80,    81,    82,
      83,    84,   574,    37,   125,    37,    34,   564,    41,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      41,    37,    36,    34,    38,    39,    32,    32,    42,    52,
      37,    45,   125,   135,   117,   118,   183,   369,   186,   122,
     368,    40,   125,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,   541,   187,   198,   202,
       8,   391,     8,    77,    78,    79,    80,    81,    82,    83,
      84,    95,   103,    -1,   203,   388,   295,   205,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,    36,
     205,    -1,    39,   205,   205,    42,   205,   205,    45,    -1,
      -1,    -1,    -1,   117,   118,    -1,    -1,    -1,   122,    36,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    -1,    -1,    52,    53,    54,    55,    56,
      77,    78,    79,    80,    81,    82,    83,    84,    -1,    -1,
      -1,    68,    -1,    -1,    -1,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,    36,    -1,    -1,    39,
      -1,    -1,    42,    -1,    -1,    45,    -1,    -1,    -1,    -1,
     117,   118,    -1,    -1,    -1,   122,    -1,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
     117,   118,    -1,    -1,    -1,    -1,    -1,    77,    78,    79,
      80,    81,    82,    83,    84,    39,    -1,    -1,    42,    -1,
      -1,    45,    92,    93,    94,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    -1,   117,   118,    -1,
      -1,    -1,   122,    77,    78,    79,    80,    81,    82,    83,
      84,    39,    -1,    -1,    42,    -1,    -1,    45,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    -1,   117,   118,    -1,    -1,    -1,   122,    77,
      78,    79,    80,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    92,    93,    94,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
     118,    -1,    -1,    -1,   122
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   139,   140,   141,   142,     0,    13,   128,   190,   191,
     196,     1,     3,     4,    19,    20,    28,    46,    47,    49,
      50,   136,   240,   241,    36,    39,    42,    45,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    77,    78,    79,    80,    81,    82,    83,    84,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     117,   118,   122,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   154,   155,   156,   157,   158,   159,   161,   162,
     163,   164,   165,   171,   172,   173,   174,   175,   176,   177,
     178,   182,    63,   192,   126,   127,   129,   130,   131,   132,
     133,   134,   197,   198,   228,   229,   230,   231,   232,   233,
     191,   196,    32,   178,   179,   244,   245,   178,   180,   179,
     179,    89,    90,   235,   178,   181,   182,   179,   137,   224,
     225,   160,   178,   178,   152,   153,   178,    39,    39,    39,
      39,    68,   117,   118,   151,   165,   172,   125,   125,    69,
     164,   164,   164,   164,   164,   164,    68,   117,   118,   149,
     149,   149,   149,   171,   171,   171,   171,   171,   171,   171,
      68,    69,   151,    68,    39,    40,   124,   125,   120,   115,
     116,   117,   118,   119,   111,   112,   114,   113,   103,   104,
     105,   106,   107,   108,    85,    86,    87,    88,    70,    71,
      72,    73,    74,    75,    39,   194,   199,   199,   126,   127,
     227,   227,   227,   227,   227,   227,   197,    32,    32,    68,
     243,    32,    32,    32,   125,   125,    32,    51,    32,    51,
      32,    51,    32,    51,   125,   179,   225,    35,    76,    38,
      44,   152,    43,   178,   178,   179,   179,    68,   164,   166,
     167,   168,   169,   170,   170,    68,    68,   164,   164,   164,
     164,   178,   144,    63,    68,   179,   151,   154,   154,   155,
     155,   156,   157,   157,   159,   161,   162,   162,   162,   162,
     162,   162,   171,   171,   171,   171,   173,   174,   174,   174,
     175,   177,    38,    63,   193,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,   135,   195,   201,   204,   207,   209,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     225,   226,   234,   236,   200,   201,   207,   209,   212,   213,
     214,   179,   179,    32,   237,   237,    39,    39,   124,   181,
     181,    63,   242,   242,   242,   242,    68,   237,   178,   178,
      38,    38,    76,    76,    70,    71,    72,    73,    74,    75,
      87,    91,    87,    91,    38,    37,    43,    38,    76,   235,
     182,   181,   181,   179,   180,   179,   208,    63,   205,   206,
     238,   202,   203,   238,    63,   187,   223,   179,    39,   179,
     179,   210,    63,   238,   237,   237,   179,   189,   189,    68,
      76,    76,    32,   124,   125,    32,    32,    32,    37,    32,
     179,   179,   164,   166,   166,   166,   167,   169,   170,   149,
     170,   149,   179,    63,   237,   237,   237,   237,   237,   237,
     237,   238,   206,    43,   124,   125,   203,    43,   124,    32,
      76,   237,   179,   237,   237,    42,    63,   121,   122,   211,
     239,   237,    38,    76,    38,   181,   181,    63,   179,   179,
      76,    38,    37,   170,    37,   170,    37,    41,    36,    52,
      53,    54,    55,    56,   149,   183,    63,    68,   144,    12,
      56,    63,   183,   184,   188,    63,   187,    76,    39,    39,
      41,   124,   125,    75,   179,   237,    37,    37,    37,   237,
     179,    37,    37,   179,    61,    62,   144,   185,   186,   187,
     125,    52,   149,    32,    37,    56,   188,   149,    39,    32,
     179,   239,   239,   179,    63,    68,   179,    39,    38,    32,
      35,    76,   179,   125,    34,   149,    34,    38,   189,    38,
      38,    38,    32,    37,   189,   186,    37,   179,   183,    34,
     188,    38,   237,    41,    41,    38,    37,   179,   180,   237,
      34,    32,    32,    52,   125,   179,    37
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

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
# if YYLTYPE_IS_TRIVIAL
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

/* Line 1455 of yacc.c  */
#line 213 "grammar.y"
    { 
  if (PARSE_MODULES != parse_mode_flag) {
    yyerror("unexpected MODULE definition encountered during parsing");
    YYABORT;
  }
#if HAVE_MBP
  /* By default unset the flag that this is a game declaration */
  unset_mbp_game(options);
#endif
}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 224 "grammar.y"
    {
                  if (!opt_mbp_game(options)) {/*this is a usual SMV file*/
                    parsed_tree = (yyvsp[(2) - (2)].node);
                  }
                  else {
                    /* this is a game (realizability problem) .
                       Return a Game with spec list on the left and a
                       module list on the right. This module list
                       contains two special modules (with names
                       PLAYER_1 and PLAYER_2) created from player
                       declarations.
                    */
                    parsed_tree = new_node(GAME,
                                           mbp_parser_spec_list,
                                           cons(mbp_parser_player_1,
                                                cons(mbp_parser_player_2, 
                                                     (yyvsp[(2) - (2)].node))));
                  }
                }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 243 "grammar.y"
    {
                  if (PARSE_COMMAND != parse_mode_flag) {
		    yyerror("unexpected command encountered during parsing");
		    YYABORT;
		  }
                }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 249 "grammar.y"
    {parsed_tree = (yyvsp[(2) - (2)].node);}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 250 "grammar.y"
    {
		  if (PARSE_LTL_EXPR != parse_mode_flag){
		    yyerror("unexpected expression encountered during parsing");
		    YYABORT;
		  }
                }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 256 "grammar.y"
    {parsed_tree = (yyvsp[(2) - (2)].node);}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 266 "grammar.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 270 "grammar.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 272 "grammar.y"
    {node_int_setcar((yyvsp[(2) - (2)].node), -(node_get_int((yyvsp[(2) - (2)].node)))); (yyval.node) = (yyvsp[(2) - (2)].node);}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 286 "grammar.y"
    {(yyval.node) = new_lined_node(TWODOTS, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno));}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 294 "grammar.y"
    {
#if HAVE_MATHSAT 
                 (yyval.node) = (yyvsp[(1) - (1)].node);
#else
                 yyerror("fractional constants are not supported.");
                 YYABORT;
#endif
               }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 303 "grammar.y"
    {
#if HAVE_MATHSAT 
                 (yyval.node) = (yyvsp[(1) - (1)].node);
#else
                 yyerror("exponential constants are not supported.");
                 YYABORT;
#endif
               }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 312 "grammar.y"
    {
#if HAVE_MATHSAT 
                 (yyval.node) = (yyvsp[(1) - (1)].node);
#else
                 yyerror("real constants are not supported.");
                 YYABORT;
#endif
               }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 330 "grammar.y"
    { (yyval.node) = new_lined_node(UMINUS, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 332 "grammar.y"
    {(yyval.node) = new_node(SELF,Nil,Nil);}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 334 "grammar.y"
    {
		      if (ATOM != node_get_type((yyvsp[(1) - (3)].node)) && DOT != node_get_type((yyvsp[(1) - (3)].node)) && ARRAY != node_get_type((yyvsp[(1) - (3)].node)) && SELF != node_get_type((yyvsp[(1) - (3)].node))) {
			yyerror_lined("incorrect DOT expression", (yyvsp[(2) - (3)].lineno));
		        YYABORT;
		      }
		      (yyval.node) = new_lined_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)) ;
		    }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 342 "grammar.y"
    { 
		      if (ATOM != node_get_type((yyvsp[(1) - (3)].node)) && DOT != node_get_type((yyvsp[(1) - (3)].node)) && ARRAY != node_get_type((yyvsp[(1) - (3)].node)) && SELF != node_get_type((yyvsp[(1) - (3)].node))) {
			yyerror_lined("incorrect DOT expression", (yyvsp[(2) - (3)].lineno));
			YYABORT;
		      }
		      (yyval.node) = new_lined_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), 2) ;
		    }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 350 "grammar.y"
    { 
		       if (ATOM != node_get_type((yyvsp[(1) - (4)].node)) && DOT != node_get_type((yyvsp[(1) - (4)].node)) && ARRAY != node_get_type((yyvsp[(1) - (4)].node)) && SELF != node_get_type((yyvsp[(1) - (4)].node))) {
			yyerror_lined("incorrect ARRAY expression", (yyvsp[(2) - (4)].lineno));
			YYABORT;
		       }
		       (yyval.node) = new_lined_node(ARRAY, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), (yyvsp[(2) - (4)].lineno));
		     }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 358 "grammar.y"
    { 
			(yyval.node) = new_lined_node(BIT_SELECTION, (yyvsp[(1) - (6)].node), 
			               new_lined_node(COLON, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].lineno)), (yyvsp[(2) - (6)].lineno));
		       }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 362 "grammar.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 363 "grammar.y"
    { (yyval.node) = new_lined_node(NOT, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 364 "grammar.y"
    { (yyval.node) = new_lined_node(CAST_BOOL, (yyvsp[(3) - (4)].node), Nil, (yyvsp[(1) - (4)].lineno)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 365 "grammar.y"
    { (yyval.node) = new_lined_node(CAST_WORD1, (yyvsp[(3) - (4)].node), Nil, (yyvsp[(1) - (4)].lineno)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 366 "grammar.y"
    { (yyval.node) = new_lined_node(NEXT, (yyvsp[(3) - (4)].node), Nil, (yyvsp[(1) - (4)].lineno)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 367 "grammar.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 371 "grammar.y"
    { (yyval.node) = new_lined_node(WAREAD, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(1) - (6)].lineno)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 374 "grammar.y"
    { (yyval.node) = new_lined_node(WAWRITE, (yyvsp[(3) - (8)].node), new_node(WAWRITE, (yyvsp[(5) - (8)].node), (yyvsp[(7) - (8)].node)), (yyvsp[(2) - (8)].lineno)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 380 "grammar.y"
    { (yyval.node) = new_node(CASE, (yyvsp[(1) - (1)].node), failure_make("case conditions are not exhaustive", FAILURE_CASE_NOT_EXHAUSTIVE, yylineno));}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 381 "grammar.y"
    { (yyval.node) = new_node(CASE, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 386 "grammar.y"
    {(yyval.node) = new_lined_node(COLON, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), (yyvsp[(2) - (4)].lineno));}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 391 "grammar.y"
    { (yyval.node) = new_lined_node(CONCATENATION, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 396 "grammar.y"
    { (yyval.node) = new_lined_node(TIMES, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 397 "grammar.y"
    { (yyval.node) = new_lined_node(DIVIDE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 402 "grammar.y"
    { (yyval.node) = new_lined_node(PLUS, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 403 "grammar.y"
    { (yyval.node) = new_lined_node(MINUS, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 408 "grammar.y"
    { (yyval.node) = new_lined_node(MOD, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 412 "grammar.y"
    { (yyval.node) = new_lined_node(LSHIFT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 413 "grammar.y"
    { (yyval.node) = new_lined_node(RSHIFT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 421 "grammar.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 425 "grammar.y"
    {(yyval.node) = new_lined_node(UNION, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno));}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 430 "grammar.y"
    { (yyval.node) = new_lined_node(UNION, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 434 "grammar.y"
    { (yyval.node) = new_lined_node(SETIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 439 "grammar.y"
    { (yyval.node) = new_lined_node(EQUAL, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 440 "grammar.y"
    { (yyval.node) = new_lined_node(NOTEQUAL, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 441 "grammar.y"
    { (yyval.node) = new_lined_node(LT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 442 "grammar.y"
    { (yyval.node) = new_lined_node(GT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 443 "grammar.y"
    { (yyval.node) = new_lined_node(LE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 444 "grammar.y"
    { (yyval.node) = new_lined_node(GE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 452 "grammar.y"
    { (yyval.node) = new_lined_node(EX, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 453 "grammar.y"
    { (yyval.node) = new_lined_node(AX, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 454 "grammar.y"
    { (yyval.node) = new_lined_node(EF, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 455 "grammar.y"
    { (yyval.node) = new_lined_node(AF, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 456 "grammar.y"
    { (yyval.node) = new_lined_node(EG, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 457 "grammar.y"
    { (yyval.node) = new_lined_node(AG, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 459 "grammar.y"
    { (yyval.node) = new_lined_node(AU, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(1) - (6)].lineno)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 461 "grammar.y"
    { (yyval.node) = new_lined_node(EU, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(1) - (6)].lineno)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 463 "grammar.y"
    { (yyval.node) = new_lined_node(ABU, new_lined_node(AU, (yyvsp[(3) - (7)].node), (yyvsp[(6) - (7)].node), (yyvsp[(1) - (7)].lineno)), (yyvsp[(5) - (7)].node), (yyvsp[(1) - (7)].lineno)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 465 "grammar.y"
    { (yyval.node) = new_lined_node(EBU, new_lined_node(EU, (yyvsp[(3) - (7)].node), (yyvsp[(6) - (7)].node), (yyvsp[(1) - (7)].lineno)), (yyvsp[(5) - (7)].node), (yyvsp[(1) - (7)].lineno)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 466 "grammar.y"
    { (yyval.node) = new_lined_node(EBF, (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].node), (yyvsp[(1) - (3)].lineno)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 467 "grammar.y"
    { (yyval.node) = new_lined_node(ABF, (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].node), (yyvsp[(1) - (3)].lineno)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 468 "grammar.y"
    { (yyval.node) = new_lined_node(EBG, (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].node), (yyvsp[(1) - (3)].lineno)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 469 "grammar.y"
    { (yyval.node) = new_lined_node(ABG, (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].node), (yyvsp[(1) - (3)].lineno)); }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 472 "grammar.y"
    { (yyval.node) = new_lined_node(NOT, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 479 "grammar.y"
    { (yyval.node) = new_lined_node(AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 483 "grammar.y"
    { (yyval.node) = new_lined_node(OR,(yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 484 "grammar.y"
    { (yyval.node) = new_lined_node(XOR,(yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 485 "grammar.y"
    { (yyval.node) = new_lined_node(XNOR,(yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 489 "grammar.y"
    { (yyval.node) = new_lined_node(IFF, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 494 "grammar.y"
    { (yyval.node) = new_lined_node(IMPLIES, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 507 "grammar.y"
    {(yyval.node) = new_lined_node(OP_NEXT, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 508 "grammar.y"
    {(yyval.node) = new_lined_node(OP_PREC, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 509 "grammar.y"
    {(yyval.node) = new_lined_node(OP_NOTPRECNOT, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 510 "grammar.y"
    {(yyval.node) = new_lined_node(OP_GLOBAL, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 511 "grammar.y"
    {(yyval.node) = new_lined_node(OP_HISTORICAL, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 512 "grammar.y"
    {(yyval.node) = new_lined_node(OP_FUTURE, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 513 "grammar.y"
    {(yyval.node) = new_lined_node(OP_ONCE, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 515 "grammar.y"
    { (yyval.node) = new_lined_node(NOT, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 522 "grammar.y"
    {(yyval.node) = new_lined_node(UNTIL, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno));}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 524 "grammar.y"
    {(yyval.node) = new_lined_node(SINCE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno));}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 526 "grammar.y"
    {(yyval.node) = new_lined_node(NOT, 
                           new_lined_node(UNTIL,
			     new_lined_node(NOT, (yyvsp[(1) - (3)].node), Nil, node_get_lineno((yyvsp[(1) - (3)].node))), 
			     new_lined_node(NOT, (yyvsp[(3) - (3)].node), Nil, node_get_lineno((yyvsp[(3) - (3)].node))),
			     (yyvsp[(2) - (3)].lineno)), Nil, (yyvsp[(2) - (3)].lineno));
		  }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 533 "grammar.y"
    {(yyval.node) = new_lined_node(NOT,
			  new_lined_node(SINCE,
			      new_lined_node(NOT, (yyvsp[(1) - (3)].node), Nil, node_get_lineno((yyvsp[(1) - (3)].node))),
			      new_lined_node(NOT, (yyvsp[(3) - (3)].node), Nil, node_get_lineno((yyvsp[(3) - (3)].node))),
			      (yyvsp[(2) - (3)].lineno)), Nil, (yyvsp[(2) - (3)].lineno));
		  }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 543 "grammar.y"
    { (yyval.node) = new_lined_node(AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 548 "grammar.y"
    { (yyval.node) = new_lined_node(OR,(yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 549 "grammar.y"
    { (yyval.node) = new_lined_node(XOR,(yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 550 "grammar.y"
    { (yyval.node) = new_lined_node(XNOR,(yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 555 "grammar.y"
    { (yyval.node) = new_lined_node(IFF, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 560 "grammar.y"
    { (yyval.node) = new_lined_node(IMPLIES, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno)); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 569 "grammar.y"
    {if (!isCorrectExp((yyval.node), EXP_SIMPLE)) YYABORT;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 572 "grammar.y"
    {if (!isCorrectExp((yyval.node), EXP_NEXT)) YYABORT;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 575 "grammar.y"
    {if (!isCorrectExp((yyval.node), EXP_CTL)) YYABORT;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 578 "grammar.y"
    {if (!isCorrectExp((yyval.node), EXP_LTL)) YYABORT;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 588 "grammar.y"
    {(yyval.node) = new_node(BOOLEAN, Nil, Nil);}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 589 "grammar.y"
    {
#if HAVE_MATHSAT 
                (yyval.node) = new_node(INTEGER, Nil, Nil);
#else
                yyerror("given token is not supported.");
                YYABORT;
#endif
              }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 597 "grammar.y"
    {
#if HAVE_MATHSAT 
                (yyval.node) = new_node(REAL, Nil, Nil);
#else
                yyerror("given token is not supported.");
                YYABORT;
#endif 
}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 606 "grammar.y"
    {(yyval.node) = new_lined_node(WORD, (yyvsp[(3) - (4)].node), Nil, (yyvsp[(1) - (4)].lineno));}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 609 "grammar.y"
    {(yyval.node) = new_lined_node(SCALAR, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 611 "grammar.y"
    {(yyval.node) = new_lined_node(WORDARRAY, (yyvsp[(4) - (10)].node), (yyvsp[(9) - (10)].node), (yyvsp[(1) - (10)].lineno));}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 613 "grammar.y"
    {(yyval.node) = new_lined_node(ARRAY, (yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node), (yyvsp[(1) - (4)].lineno));}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 620 "grammar.y"
    {(yyval.node) = new_lined_node(PROCESS, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 623 "grammar.y"
    {(yyval.node) = cons(find_atom((yyvsp[(1) - (1)].node)), Nil);}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 624 "grammar.y"
    {(yyval.node) = cons(find_atom((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node));}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 634 "grammar.y"
    {(yyval.node) = new_lined_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(2) - (3)].lineno));}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 637 "grammar.y"
    {(yyval.node) = new_node(MODTYPE, (yyvsp[(1) - (1)].node), Nil);}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 638 "grammar.y"
    {(yyval.node) = new_node(MODTYPE, (yyvsp[(1) - (3)].node), Nil);}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 640 "grammar.y"
    {(yyval.node) = new_lined_node(MODTYPE, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), node_get_lineno((yyvsp[(1) - (4)].node)));}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 642 "grammar.y"
    {(yyval.node) = new_lined_node(ARRAY, (yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node), (yyvsp[(1) - (4)].lineno));}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 646 "grammar.y"
    {(yyval.node) = cons((yyvsp[(1) - (1)].node),Nil);}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 647 "grammar.y"
    {(yyval.node) = cons((yyvsp[(3) - (3)].node), (yyvsp[(1) - (3)].node));}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 662 "grammar.y"
    {(yyval.node) = cons((yyvsp[(1) - (1)].node), Nil);}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 663 "grammar.y"
    {(yyval.node) = Nil;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 664 "grammar.y"
    {(yyval.node) = cons((yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].node));}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 665 "grammar.y"
    {(yyval.node) = (yyvsp[(1) - (2)].node);}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 669 "grammar.y"
    {(yyval.node) = new_lined_node(MODULE, (yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].node), (yyvsp[(1) - (3)].lineno));}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 671 "grammar.y"
    {(yyval.node) = new_node(MODTYPE, (yyvsp[(1) - (1)].node), Nil);}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 672 "grammar.y"
    {(yyval.node) = new_node(MODTYPE, (yyvsp[(1) - (3)].node), Nil);}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 674 "grammar.y"
    {(yyval.node) = new_node(MODTYPE, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 676 "grammar.y"
    {(yyval.node) = cons(find_atom((yyvsp[(1) - (1)].node)), Nil);}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 677 "grammar.y"
    {(yyval.node) = cons(find_atom((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node));}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 682 "grammar.y"
    {(yyval.node) = Nil;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 683 "grammar.y"
    {(yyval.node) = cons((yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].node));}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 714 "grammar.y"
    {
		       /* check that the game is not redeclared */
		       if (opt_mbp_game(options)) {
			 yyerror_lined("redefinition of a GAME", (yyvsp[(1) - (2)].lineno));
		       }
		       /* set that this input file is a game declaration */
		       else {
#if !HAVE_MBP
             yyerror_lined("GAME declaration cannot be processes "
                           "because MBP package is not set up\n"
                           "Check --enable-mbp option of "
                           "the configure script\n", (yyvsp[(1) - (2)].lineno));
             YYABORT;
#else
             set_mbp_game(options);
#endif
		       }

		       mbp_parser_spec_list = (yyvsp[(2) - (2)].node);
		       (yyval.node) = Nil;
		     }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 738 "grammar.y"
    {(yyval.node)=Nil;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 740 "grammar.y"
    {if (Nil != (yyvsp[(1) - (2)].node)) (yyval.node) = cons((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)); else (yyval.node) = (yyvsp[(2) - (2)].node);}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 748 "grammar.y"
    {
		       /* a player definition is converted to a module
			  definitino with a special name. This is done
			  to simplify the further flattening
		       */
		       if (mbp_parser_player_1 != Nil) {
			 yyerror_lined("redefinition of a PLAYER_1", (yyvsp[(1) - (2)].lineno));
			 YYABORT;
		       }
		       mbp_parser_player_1 = new_lined_node(MODULE,
			   new_node(MODTYPE, 
			     new_node(ATOM,(node_ptr)find_string("PLAYER_1"),
				      Nil), Nil),  (yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].lineno));
		       (yyval.node)=Nil;
		     }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 764 "grammar.y"
    {
		       /* a player definition is converted to a module
			  definitino with a special name. This is done
			  to simplify the further flattening
		       */
		       if (mbp_parser_player_2 != Nil) {
			 yyerror_lined("redefinition of a PLAYER_2", (yyvsp[(1) - (2)].lineno));
			 YYABORT;
		       }
		       mbp_parser_player_2 = new_lined_node(MODULE,
			   new_node(MODTYPE, 
			     new_node(ATOM,(node_ptr)find_string("PLAYER_2"),
				      Nil), Nil), (yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].lineno));
		       (yyval.node)=Nil;
		     }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 795 "grammar.y"
    { (yyval.node) = Nil; }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 796 "grammar.y"
    { (yyval.node) = cons((yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].node)); }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 818 "grammar.y"
    {(yyval.node) = new_lined_node(VAR, Nil, Nil, (yyvsp[(1) - (1)].lineno));}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 819 "grammar.y"
    {(yyval.node) = new_lined_node(VAR, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 821 "grammar.y"
    {(yyval.node) = cons((yyvsp[(1) - (1)].node), Nil);}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 822 "grammar.y"
    {(yyval.node) = cons((yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].node));}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 824 "grammar.y"
    {(yyval.node) = new_lined_node(COLON, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), (yyvsp[(2) - (4)].lineno));}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 826 "grammar.y"
    {(yyval.node) = new_lined_node(IVAR, Nil, Nil, (yyvsp[(1) - (1)].lineno));}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 827 "grammar.y"
    {(yyval.node) = new_lined_node(IVAR, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 829 "grammar.y"
    {(yyval.node) = cons((yyvsp[(1) - (1)].node), Nil);}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 830 "grammar.y"
    {(yyval.node) = cons((yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].node));}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 832 "grammar.y"
    {(yyval.node) = new_lined_node(COLON, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), (yyvsp[(2) - (4)].lineno));}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 836 "grammar.y"
    {(yyval.node) = new_lined_node(DEFINE, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 838 "grammar.y"
    {(yyval.node) = Nil;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 839 "grammar.y"
    {(yyval.node) = cons(new_lined_node(EQDEF, (yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(3) - (5)].lineno)), (yyvsp[(1) - (5)].node));}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 843 "grammar.y"
    {(yyval.node) = new_lined_node(ASSIGN, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 845 "grammar.y"
    {(yyval.node) = Nil;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 846 "grammar.y"
    {(yyval.node) = new_node(AND, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 849 "grammar.y"
    {(yyval.node) = new_lined_node(EQDEF, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), (yyvsp[(2) - (4)].lineno));}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 851 "grammar.y"
    { (yyval.node) = new_lined_node(EQDEF,
					new_lined_node(SMALLINIT, (yyvsp[(3) - (7)].node), Nil, (yyvsp[(1) - (7)].lineno)),
					(yyvsp[(6) - (7)].node), (yyvsp[(5) - (7)].lineno));
		  }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 856 "grammar.y"
    { (yyval.node) = new_lined_node(EQDEF,
					new_lined_node(NEXT, (yyvsp[(3) - (7)].node), Nil, (yyvsp[(1) - (7)].lineno)),
					(yyvsp[(6) - (7)].node), (yyvsp[(5) - (7)].lineno));
		  }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 863 "grammar.y"
    {(yyval.node) = new_lined_node(INIT, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 865 "grammar.y"
    {(yyval.node) = new_lined_node(INVAR, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 867 "grammar.y"
    {(yyval.node) = new_lined_node(TRANS, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 871 "grammar.y"
    {(yyval.node) = new_lined_node(JUSTICE, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 874 "grammar.y"
    {(yyval.node) = new_lined_node(JUSTICE, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 879 "grammar.y"
    {(yyval.node) = new_lined_node(COMPASSION, cons((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node)), Nil, (yyvsp[(1) - (7)].lineno));}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 882 "grammar.y"
    {(yyval.node) = new_lined_node(INVARSPEC, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 885 "grammar.y"
    {(yyval.node) = new_lined_node(SPEC, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 886 "grammar.y"
    {(yyval.node) = new_lined_node(SPEC, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 889 "grammar.y"
    {(yyval.node) = new_lined_node(LTLSPEC, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 893 "grammar.y"
    { 
  if (nusmv_parse_psl() != 0) {
    YYABORT;
  }
  (yyval.node) = new_lined_node(PSLSPEC, psl_parsed_tree, Nil, (yyvsp[(1) - (1)].lineno)); 
}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 902 "grammar.y"
    {(yyval.node) = new_lined_node(CONSTANTS, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 906 "grammar.y"
    {(yyval.node) = Nil;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 907 "grammar.y"
    { (yyval.node) = cons((yyvsp[(1) - (1)].node), Nil);}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 908 "grammar.y"
    {(yyval.node) = cons((yyvsp[(3) - (3)].node), (yyvsp[(1) - (3)].node));}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 912 "grammar.y"
    { (yyval.node) = cons((yyvsp[(1) - (1)].node), Nil);}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 913 "grammar.y"
    { (yyval.node) = cons((yyvsp[(2) - (2)].node), (yyvsp[(1) - (2)].node));}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 917 "grammar.y"
    {
#if HAVE_MATHSAT 
                   (yyval.node) = new_lined_node(PRED, (yyvsp[(2) - (3)].node), 
                                       new_lined_node(NUMBER, 
                                                      (node_ptr) 0, Nil, (yyvsp[(1) - (3)].lineno)), (yyvsp[(1) - (3)].lineno));
#else
                   yyerror("given token is not supported.");
                   YYABORT;
#endif                 
                 }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 928 "grammar.y"
    {
#if HAVE_MATHSAT 
                   (yyval.node) = new_lined_node(PRED, (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(1) - (6)].lineno));
#else
                   yyerror("given token is not supported.");
                   YYABORT;
#endif                 
                 }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 939 "grammar.y"
    {
#if HAVE_MATHSAT 
                    (yyval.node) = new_lined_node(MIRROR, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));
#else
                    yyerror("given token is not supported.");
                    YYABORT;
#endif                 
                  }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 949 "grammar.y"
    {(yyval.lineno)=1;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 950 "grammar.y"
    {(yyval.lineno)=2;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 953 "grammar.y"
    {(yyval.node) = new_lined_node(REACHTARGET, NODE_FROM_INT((yyvsp[(2) - (4)].lineno)), (yyvsp[(3) - (4)].node), (yyvsp[(1) - (4)].lineno));}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 956 "grammar.y"
    {(yyval.node) = new_lined_node(AVOIDTARGET, NODE_FROM_INT((yyvsp[(2) - (4)].lineno)), (yyvsp[(3) - (4)].node), (yyvsp[(1) - (4)].lineno));}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 959 "grammar.y"
    {(yyval.node) = new_lined_node(REACHDEADLOCK, NODE_FROM_INT((yyvsp[(2) - (3)].lineno)), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 962 "grammar.y"
    {(yyval.node) = new_lined_node(AVOIDDEADLOCK, NODE_FROM_INT((yyvsp[(2) - (3)].lineno)), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 967 "grammar.y"
    {(yyval.node) = new_lined_node(BUCHIGAME, NODE_FROM_INT((yyvsp[(2) - (6)].lineno)),
					    new_lined_node(MBP_EXP_LIST,
							 reverse((yyvsp[(4) - (6)].node)), Nil, (yyvsp[(3) - (6)].lineno)),
					    (yyvsp[(1) - (6)].lineno));}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 975 "grammar.y"
    {(yyval.node) = new_lined_node(GENREACTIVITY, NODE_FROM_INT((yyvsp[(2) - (10)].lineno)),
				          new_lined_node(MBP_TWO_EXP_LISTS,
						reverse((yyvsp[(4) - (10)].node)), reverse((yyvsp[(8) - (10)].node)), (yyvsp[(6) - (10)].lineno)),
					    (yyvsp[(1) - (10)].lineno));}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 981 "grammar.y"
    {(yyval.node) = new_lined_node(COMPUTE, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 985 "grammar.y"
    { (yyval.node) = new_lined_node(MINU, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(1) - (6)].lineno)); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 987 "grammar.y"
    { (yyval.node) = new_lined_node(MAXU, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(1) - (6)].lineno)); }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 991 "grammar.y"
    {(yyval.node) = new_node(ISA, (yyvsp[(2) - (2)].node), Nil);}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 995 "grammar.y"
    {}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1004 "grammar.y"
    {(yyval.node) = new_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1005 "grammar.y"
    {(yyval.node) = new_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1006 "grammar.y"
    {(yyval.node) = new_node(ARRAY, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 1010 "grammar.y"
    {(yyval.node) = new_node(SELF,Nil,Nil);}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 1011 "grammar.y"
    {(yyval.node) = new_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 1012 "grammar.y"
    {(yyval.node) = new_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 1013 "grammar.y"
    {(yyval.node) = new_node(ARRAY, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 1021 "grammar.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 1022 "grammar.y"
    {return(1);}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 1023 "grammar.y"
    {return(1);}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 1027 "grammar.y"
    {(yyval.node) = new_lined_node(GOTO, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 1029 "grammar.y"
    {(yyval.node) = new_lined_node(INIT, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 1031 "grammar.y"
    {(yyval.node) = new_lined_node(JUSTICE, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 1033 "grammar.y"
    {(yyval.node) = new_lined_node(TRANS, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 1035 "grammar.y"
    {(yyval.node) = new_lined_node(CONSTRAINT, (yyvsp[(2) - (3)].node), Nil, (yyvsp[(1) - (3)].lineno));}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 1037 "grammar.y"
    {(yyval.node) = new_lined_node(SIMPWFF,
                             new_node(CONTEXT, Nil, (yyvsp[(2) - (3)].node)), Nil, (yyvsp[(1) - (3)].lineno));
                 }
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 1041 "grammar.y"
    {(yyval.node) = new_lined_node(SIMPWFF,
                       new_node(CONTEXT, context2maincontext((yyvsp[(4) - (5)].node)), (yyvsp[(2) - (5)].node)), Nil, (yyvsp[(1) - (5)].lineno));
                 }
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 1045 "grammar.y"
    {(yyval.node) = new_lined_node(CTLWFF,
                             new_node(CONTEXT, Nil, (yyvsp[(2) - (3)].node)), Nil, (yyvsp[(1) - (3)].lineno));
                 }
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 1049 "grammar.y"
    {(yyval.node) = new_lined_node(CTLWFF,
                       new_node(CONTEXT, context2maincontext((yyvsp[(4) - (5)].node)), (yyvsp[(2) - (5)].node)), Nil, (yyvsp[(1) - (5)].lineno));
                 }
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 1053 "grammar.y"
    {(yyval.node) = new_lined_node(LTLWFF,
                             new_node(CONTEXT, Nil, (yyvsp[(2) - (3)].node)), Nil, (yyvsp[(1) - (3)].lineno));
                 }
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 1057 "grammar.y"
    {(yyval.node) = new_lined_node(LTLWFF,
                       new_node(CONTEXT, context2maincontext((yyvsp[(4) - (5)].node)), (yyvsp[(2) - (5)].node)), Nil, (yyvsp[(1) - (5)].lineno));
                 }
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 1061 "grammar.y"
    {(yyval.node) = new_lined_node(COMPWFF,
                             new_node(CONTEXT, Nil, (yyvsp[(2) - (3)].node)), Nil, (yyvsp[(1) - (3)].lineno));
                 }
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 1065 "grammar.y"
    {(yyval.node) = new_lined_node(COMPWFF,
                       new_node(CONTEXT, context2maincontext((yyvsp[(4) - (5)].node)), (yyvsp[(2) - (5)].node)), Nil, (yyvsp[(1) - (5)].lineno));
                 }
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 1069 "grammar.y"
    {
#if HAVE_MATHSAT 
                  (yyval.node) = new_lined_node(PREDS_LIST, (yyvsp[(2) - (2)].node), Nil, (yyvsp[(1) - (2)].lineno));
#else
                  yyerror("given token is not supported.");
                  YYABORT;
#endif
                }
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 1080 "grammar.y"
    {(yyval.node) = find_atom((yyvsp[(1) - (1)].node));}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 1081 "grammar.y"
    {(yyval.node) = find_node(DOT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 1082 "grammar.y"
    {(yyval.node) = find_node(ARRAY, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 1085 "grammar.y"
    {(yyval.node) = (node_ptr)find_atom((yyvsp[(1) - (1)].node));}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 1086 "grammar.y"
    {(yyval.node) = find_node(DOT, (yyvsp[(1) - (3)].node), find_atom((yyvsp[(3) - (3)].node)));}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 1089 "grammar.y"
    {parser_parse_real_unset(); }
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 1091 "grammar.y"
    {
                     parser_parse_real_set(); 
                     (yyval.node) = find_node(DOT, (yyvsp[(2) - (4)].node), find_atom((yyvsp[(4) - (4)].node)));
                   }
    break;



/* Line 1455 of yacc.c  */
#line 4035 "grammar.c"
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



/* Line 1675 of yacc.c  */
#line 1098 "grammar.y"



/* Additional source code */
/* outputs the current token with the provided string and then may terminate */
void yyerror(char *s)
{
    extern options_ptr options;
    extern char yytext[];
    
    start_parsing_err();
    fprintf(nusmv_stderr, "at token \"%s\": %s\n", yytext, s);
    if (opt_batch(options)) finish_parsing_err();
}

/* the same as yyerror, except at first it sets the line number and does
 not output the current token 
*/
void yyerror_lined(const char *s, int line)
{
    extern options_ptr options;
    extern int yylineno;

    /*set the line number */
    yylineno = line;

    start_parsing_err();
    fprintf(nusmv_stderr, ": %s\n", s);
    if (opt_batch(options)) finish_parsing_err();
}

int yywrap()
{
  return(1);
}

/* This function is used to build the internal structure of the
   context (e.g. module instance name) from the parse tree. The
   function is needed since with the grammar it is not possible/simple
   to build directly the desired structure. */
static node_ptr context2maincontext(node_ptr context) {
  switch(node_get_type(context)) {
  case ATOM:
    return find_node(DOT, Nil, find_atom(context));
  case DOT: {
    node_ptr t = context2maincontext(car(context));
    return find_node(DOT, t, find_atom(cdr(context)));
  }
  case ARRAY: {
    node_ptr t = context2maincontext(car(context));
    return find_node(ARRAY, t, find_atom(cdr(context)));
  }
  default:
    fprintf(nusmv_stderr, "context2maincontext: undefined token \"%d\"\n",
	    node_get_type(context));
    nusmv_assert(false);
  }
}

/* this functions checks that the expression is formed syntactically correct.
   Takes the expression to be checked, the expected kind of the 
   expression. Returns true if the expression is formed correctly, and 
   false otherwise.
   See enum EXP_KIND for more info about syntactic well-formedness.
*/
static boolean isCorrectExp(node_ptr exp, enum EXP_KIND expectedKind)
{
  switch(node_get_type(exp))
    {
      /* atomic expression (or thier operands have been checked earlier) */
    case FAILURE:
    case FALSEEXP:
    case TRUEEXP:
    case NUMBER:
    case NUMBER_WORD:
    case NUMBER_FRAC: 
    case NUMBER_REAL:
    case NUMBER_EXP:
    case TWODOTS:
    case DOT:
    case ATOM:
    case SELF:
    case ARRAY:
    case BIT_SELECTION:
      return true;

      /* unary operators incompatible with LTL and CTL operator */
    case CAST_BOOL:
    case CAST_WORD1:
      if (EXP_LTL == expectedKind || EXP_CTL == expectedKind) {
	return isCorrectExp(car(exp), EXP_SIMPLE);
      } 
      /* unary operators compatible with LTL and CTL operator */
    case NOT: 
    case UMINUS:
      return isCorrectExp(car(exp), expectedKind);
      
      /* binary opertors incompatible with LTL and CTL operator */
    case CASE: case COLON:
    case CONCATENATION: 
    case TIMES: case DIVIDE: case PLUS :case MINUS: case MOD: 
    case LSHIFT: case RSHIFT: case LROTATE: case RROTATE: 
    case WAREAD: case WAWRITE: /* AC ADDED THESE */
    case UNION: case SETIN: 
    case EQUAL: case NOTEQUAL: case LT: case GT: case LE: case GE: 
      if (EXP_LTL == expectedKind || EXP_CTL == expectedKind) {
	return isCorrectExp(car(exp), EXP_SIMPLE)
	  && isCorrectExp(cdr(exp), EXP_SIMPLE);
      } 
      /* binary opertors compatible LTL and CTL operator */
    case AND: case OR: case XOR: case XNOR: case IFF: case IMPLIES: 
      return isCorrectExp(car(exp), expectedKind)
	&& isCorrectExp(cdr(exp), expectedKind);

      /* next expression */
    case NEXT: 
      if (EXP_NEXT != expectedKind) {
	yyerror_lined("unexpected 'next' operator", node_get_lineno(exp));
	return false;
      }
      return isCorrectExp(car(exp), EXP_SIMPLE); /* NEXT cannot contain NEXT */

      /* CTL unary expressions */
    case EX: case AX: case EF: case AF: case EG: case AG: 
    case ABU: case EBU:
    case EBF: case ABF: case EBG: case ABG: 
      if (EXP_CTL != expectedKind) {
	yyerror_lined("unexpected CTL operator", node_get_lineno(exp));
	return false;
      }
      return isCorrectExp(car(exp), EXP_CTL); /* continue to check CTL */
      
      /* CTL binary expressions */
    case AU: case EU: 
      if (EXP_CTL != expectedKind) {
	yyerror_lined("unexpected CTL operator", node_get_lineno(exp));
	return false;
      }
      return isCorrectExp(car(exp), EXP_CTL)
	&& isCorrectExp(cdr(exp), EXP_CTL); /* continue to check CTL */


      /* LTL unary expressions */
    case OP_NEXT: case OP_PREC: case OP_NOTPRECNOT: case OP_GLOBAL:
    case OP_HISTORICAL: case OP_FUTURE: case OP_ONCE:
      if (EXP_LTL != expectedKind) {
	yyerror_lined("unexpected LTL operator", node_get_lineno(exp));
	return false;
      }
      return isCorrectExp(car(exp), EXP_LTL); /* continue to check LTL */


      /* LTL binary expressions */
    case UNTIL: case SINCE: 
      if (EXP_LTL != expectedKind) {
	yyerror_lined("unexpected LTL operator", node_get_lineno(exp));
	return false;
      }
      return isCorrectExp(car(exp), EXP_LTL)
	&& isCorrectExp(cdr(exp), EXP_LTL); /* continue to check LTL */

    default: nusmv_assert(false); /* unknown expression */
    }
  return false; /* should never be invoked */
}


static int nusmv_parse_psl() 
{
  int res;
  res = psl_yyparse();
  return res;  
}


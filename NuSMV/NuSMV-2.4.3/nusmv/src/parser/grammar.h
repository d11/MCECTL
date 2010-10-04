
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
#line 113 "grammar.y"

  node_ptr node;
  int lineno;



/* Line 1676 of yacc.c  */
#line 333 "grammar.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;



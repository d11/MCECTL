
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
     TKEOF = 258,
     TKSTRING = 259,
     TKERROR = 260,
     TKSTRUCT = 261,
     TKNUMBER = 262,
     TKREALNUMBER = 263,
     TKBASENUMBER = 264,
     TKTRUE = 265,
     TKFALSE = 266,
     TKINCONTEXT = 267,
     TKSERE = 268,
     TKSERECONCAT = 269,
     TKSEREFUSION = 270,
     TKSERECOMPOUND = 271,
     TKSEREREPEATED = 272,
     TKCONS = 273,
     TKCONCATENATION = 274,
     TKREPLPROP = 275,
     TKARRAY = 276,
     TKCONTEXT = 277,
     TKATOM = 278,
     TKFAILURE = 279,
     TKITE = 280,
     TKVUNIT = 281,
     TKVMODE = 282,
     TKVPROP = 283,
     TKSTRONG = 284,
     TKDEFPARAM = 285,
     TKINHERIT = 286,
     TKFAIRNESS = 287,
     TKCONST = 288,
     TKBEGIN = 289,
     TKEND = 290,
     TKPARAMETER = 291,
     TKTASK = 292,
     TKENDTASK = 293,
     TKFORK = 294,
     TKJOIN = 295,
     TKSUPPLY0 = 296,
     TKSUPPLY1 = 297,
     TKSTRONG0 = 298,
     TKPULL0 = 299,
     TKWEAK0 = 300,
     TKHIGHZ0 = 301,
     TKSTRONG1 = 302,
     TKPULL1 = 303,
     TKWEAK1 = 304,
     TKHIGHZ1 = 305,
     TKINPUT = 306,
     TKOUTPUT = 307,
     TKINOUT = 308,
     TKDEFAULT_CLOCK = 309,
     TKDEFAULT_COLON = 310,
     TKDEASSIGN = 311,
     TKDISABLE = 312,
     TKENDSPECIFY = 313,
     TKFOR = 314,
     TKINITIAL = 315,
     TKSPECIFY = 316,
     TKWAIT = 317,
     TKFOREVER = 318,
     TKREPEAT = 319,
     TKWHILE = 320,
     TKENDMODULE = 321,
     TKENDFUNCTION = 322,
     TKWIRE = 323,
     TKTRI = 324,
     TKTRI1 = 325,
     TKWAND = 326,
     TKTRIAND = 327,
     TKTRI0 = 328,
     TKWOR = 329,
     TKTRIOR = 330,
     TKTRIREG = 331,
     TKREG = 332,
     TKINTEGER = 333,
     TKINF = 334,
     TKDOT = 335,
     TKENDPOINT = 336,
     TKASSIGN = 337,
     TKFORCE = 338,
     TKRELEASE = 339,
     TKPROPERTY = 340,
     TKSEQUENCE = 341,
     TKMODULE = 342,
     TKFUNCTION = 343,
     TKRESTRICT = 344,
     TKRESTRICT_GUARANTEE = 345,
     TKFORALL = 346,
     TKFORANY = 347,
     TKASSERT = 348,
     TKASSUME = 349,
     TKASSUME_GUARANTEE = 350,
     TKCOVER = 351,
     TKBOOLEAN = 352,
     TKCASE = 353,
     TKCASEX = 354,
     TKCASEZ = 355,
     TKELSE = 356,
     TKENDCASE = 357,
     TKIF = 358,
     TKNONDET = 359,
     TKNONDET_VECTOR = 360,
     TKNONDET_RANGE = 361,
     TKWNONDET = 362,
     TKBASE = 363,
     TKDOTDOT = 364,
     TKPIPEMINUSGT = 365,
     TKPIPEEQGT = 366,
     TKIDENTIFIER = 367,
     TKHIERARCHICALID = 368,
     TKLP = 369,
     TKRP = 370,
     TKLC = 371,
     TKRC = 372,
     TKLB = 373,
     TKRB = 375,
     TKCOMMA = 376,
     TKDIEZ = 377,
     TKTRANS = 378,
     TKHINT = 379,
     TKTEST_PINS = 380,
     TKALWAYS = 381,
     TKNEVER = 382,
     TKEVENTUALLYBANG = 383,
     TKWITHINBANG = 384,
     TKWITHIN = 385,
     TKWITHINBANG_ = 386,
     TKWITHIN_ = 387,
     TKWHILENOTBANG = 388,
     TKWHILENOT = 389,
     TKWHILENOTBANG_ = 390,
     TKWHILENOT_ = 391,
     TKNEXT_EVENT_ABANG = 392,
     TKNEXT_EVENT_A = 393,
     TKNEXT_EVENT_EBANG = 394,
     TKNEXT_EVENT_E = 395,
     TKNEXT_EVENTBANG = 396,
     TKNEXT_EVENT = 397,
     TKNEXT_ABANG = 398,
     TKNEXT_EBANG = 399,
     TKNEXT_A = 400,
     TKNEXT_E = 401,
     TKNEXTBANG = 402,
     TKNEXT = 403,
     TKNEXTfunc = 404,
     TKBEFOREBANG = 405,
     TKBEFORE = 406,
     TKBEFOREBANG_ = 407,
     TKBEFORE_ = 408,
     TKUNTILBANG = 409,
     TKUNTIL = 410,
     TKUNTILBANG_ = 411,
     TKUNTIL_ = 412,
     TKABORT = 413,
     TKROSE = 414,
     TKFELL = 415,
     TKPREV = 416,
     TKG = 417,
     TKXBANG = 418,
     TKX = 419,
     TKF = 420,
     TKU = 421,
     TKW = 422,
     TKEG = 423,
     TKEX = 424,
     TKEF = 425,
     TKAG = 426,
     TKAX = 427,
     TKAF = 428,
     TKA = 429,
     TKE = 430,
     TKIN = 431,
     TKUNION = 432,
     TKQUESTIONMARK = 433,
     TKCOLON = 434,
     TKSEMI = 435,
     TKPIPEPIPE = 436,
     TKAMPERSANDAMPERSAND = 437,
     TKMINUSGT = 438,
     TKLTMINUSGT = 439,
     TKPIPE = 440,
     TKTILDEPIPE = 441,
     TKOR = 442,
     TKPOSEDGE = 443,
     TKNEGEDGE = 444,
     TKCARET = 445,
     TKXOR = 446,
     TKCARETTILDE = 447,
     TKTILDECARET = 448,
     TKAMPERSAND = 449,
     TKTILDEAMPERSAND = 450,
     TKEQEQ = 451,
     TKBANGEQ = 452,
     TKEQEQEQ = 453,
     TKBANGEQEQ = 454,
     TKEQ = 455,
     TKGT = 456,
     TKGE = 457,
     TKLT = 458,
     TKLE = 459,
     TKLTLT = 460,
     TKGTGT = 461,
     TKGTGTGT = 462,
     TKLTLTLT = 463,
     TKPLUS = 464,
     TKMINUS = 465,
     TKSPLAT = 466,
     TKSLASH = 467,
     TKPERCENT = 468,
     TKSPLATSPLAT = 469,
     TKBANG = 470,
     TKTILDE = 471,
     TKLBSPLAT = 472,
     TKLBEQ = 473,
     TKLBMINUSGT = 474,
     TKLBPLUSRB = 475,
     TKSTRUDLE = 476
   };
#endif
/* Tokens.  */
#define TKEOF 258
#define TKSTRING 259
#define TKERROR 260
#define TKSTRUCT 261
#define TKNUMBER 262
#define TKREALNUMBER 263
#define TKBASENUMBER 264
#define TKTRUE 265
#define TKFALSE 266
#define TKINCONTEXT 267
#define TKSERE 268
#define TKSERECONCAT 269
#define TKSEREFUSION 270
#define TKSERECOMPOUND 271
#define TKSEREREPEATED 272
#define TKCONS 273
#define TKCONCATENATION 274
#define TKREPLPROP 275
#define TKARRAY 276
#define TKCONTEXT 277
#define TKATOM 278
#define TKFAILURE 279
#define TKITE 280
#define TKVUNIT 281
#define TKVMODE 282
#define TKVPROP 283
#define TKSTRONG 284
#define TKDEFPARAM 285
#define TKINHERIT 286
#define TKFAIRNESS 287
#define TKCONST 288
#define TKBEGIN 289
#define TKEND 290
#define TKPARAMETER 291
#define TKTASK 292
#define TKENDTASK 293
#define TKFORK 294
#define TKJOIN 295
#define TKSUPPLY0 296
#define TKSUPPLY1 297
#define TKSTRONG0 298
#define TKPULL0 299
#define TKWEAK0 300
#define TKHIGHZ0 301
#define TKSTRONG1 302
#define TKPULL1 303
#define TKWEAK1 304
#define TKHIGHZ1 305
#define TKINPUT 306
#define TKOUTPUT 307
#define TKINOUT 308
#define TKDEFAULT_CLOCK 309
#define TKDEFAULT_COLON 310
#define TKDEASSIGN 311
#define TKDISABLE 312
#define TKENDSPECIFY 313
#define TKFOR 314
#define TKINITIAL 315
#define TKSPECIFY 316
#define TKWAIT 317
#define TKFOREVER 318
#define TKREPEAT 319
#define TKWHILE 320
#define TKENDMODULE 321
#define TKENDFUNCTION 322
#define TKWIRE 323
#define TKTRI 324
#define TKTRI1 325
#define TKWAND 326
#define TKTRIAND 327
#define TKTRI0 328
#define TKWOR 329
#define TKTRIOR 330
#define TKTRIREG 331
#define TKREG 332
#define TKINTEGER 333
#define TKINF 334
#define TKDOT 335
#define TKENDPOINT 336
#define TKASSIGN 337
#define TKFORCE 338
#define TKRELEASE 339
#define TKPROPERTY 340
#define TKSEQUENCE 341
#define TKMODULE 342
#define TKFUNCTION 343
#define TKRESTRICT 344
#define TKRESTRICT_GUARANTEE 345
#define TKFORALL 346
#define TKFORANY 347
#define TKASSERT 348
#define TKASSUME 349
#define TKASSUME_GUARANTEE 350
#define TKCOVER 351
#define TKBOOLEAN 352
#define TKCASE 353
#define TKCASEX 354
#define TKCASEZ 355
#define TKELSE 356
#define TKENDCASE 357
#define TKIF 358
#define TKNONDET 359
#define TKNONDET_VECTOR 360
#define TKNONDET_RANGE 361
#define TKWNONDET 362
#define TKBASE 363
#define TKDOTDOT 364
#define TKPIPEMINUSGT 365
#define TKPIPEEQGT 366
#define TKIDENTIFIER 367
#define TKHIERARCHICALID 368
#define TKLP 369
#define TKRP 370
#define TKLC 371
#define TKRC 372
#define TKLB 373
#define TKRB 375
#define TKCOMMA 376
#define TKDIEZ 377
#define TKTRANS 378
#define TKHINT 379
#define TKTEST_PINS 380
#define TKALWAYS 381
#define TKNEVER 382
#define TKEVENTUALLYBANG 383
#define TKWITHINBANG 384
#define TKWITHIN 385
#define TKWITHINBANG_ 386
#define TKWITHIN_ 387
#define TKWHILENOTBANG 388
#define TKWHILENOT 389
#define TKWHILENOTBANG_ 390
#define TKWHILENOT_ 391
#define TKNEXT_EVENT_ABANG 392
#define TKNEXT_EVENT_A 393
#define TKNEXT_EVENT_EBANG 394
#define TKNEXT_EVENT_E 395
#define TKNEXT_EVENTBANG 396
#define TKNEXT_EVENT 397
#define TKNEXT_ABANG 398
#define TKNEXT_EBANG 399
#define TKNEXT_A 400
#define TKNEXT_E 401
#define TKNEXTBANG 402
#define TKNEXT 403
#define TKNEXTfunc 404
#define TKBEFOREBANG 405
#define TKBEFORE 406
#define TKBEFOREBANG_ 407
#define TKBEFORE_ 408
#define TKUNTILBANG 409
#define TKUNTIL 410
#define TKUNTILBANG_ 411
#define TKUNTIL_ 412
#define TKABORT 413
#define TKROSE 414
#define TKFELL 415
#define TKPREV 416
#define TKG 417
#define TKXBANG 418
#define TKX 419
#define TKF 420
#define TKU 421
#define TKW 422
#define TKEG 423
#define TKEX 424
#define TKEF 425
#define TKAG 426
#define TKAX 427
#define TKAF 428
#define TKA 429
#define TKE 430
#define TKIN 431
#define TKUNION 432
#define TKQUESTIONMARK 433
#define TKCOLON 434
#define TKSEMI 435
#define TKPIPEPIPE 436
#define TKAMPERSANDAMPERSAND 437
#define TKMINUSGT 438
#define TKLTMINUSGT 439
#define TKPIPE 440
#define TKTILDEPIPE 441
#define TKOR 442
#define TKPOSEDGE 443
#define TKNEGEDGE 444
#define TKCARET 445
#define TKXOR 446
#define TKCARETTILDE 447
#define TKTILDECARET 448
#define TKAMPERSAND 449
#define TKTILDEAMPERSAND 450
#define TKEQEQ 451
#define TKBANGEQ 452
#define TKEQEQEQ 453
#define TKBANGEQEQ 454
#define TKEQ 455
#define TKGT 456
#define TKGE 457
#define TKLT 458
#define TKLE 459
#define TKLTLT 460
#define TKGTGT 461
#define TKGTGTGT 462
#define TKLTLTLT 463
#define TKPLUS 464
#define TKMINUS 465
#define TKSPLAT 466
#define TKSLASH 467
#define TKPERCENT 468
#define TKSPLATSPLAT 469
#define TKBANG 470
#define TKTILDE 471
#define TKLBSPLAT 472
#define TKLBEQ 473
#define TKLBMINUSGT 474
#define TKLBPLUSRB 475
#define TKSTRUDLE 476




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 60 "psl_grammar.y"

  node_ptr node; 
  int lineno;

  /* these are news */
  int ival;
  char* fval;
  char* baseval;
  char* idname;
  PslExpr psl_expr;
  PslOp operator;



/* Line 1676 of yacc.c  */
#line 507 "psl_grammar.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE psl_yylval;



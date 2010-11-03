%{
    open Lexing;;

    (* File from a skeleton provided by Gerard Basler *)
    let parse_error msg = 
        print_string ("Parse error: " ^ msg ^ "\n");
        let lex_pos = Parsing.symbol_start_pos () in
        print_string ("At line: " ^ (string_of_int lex_pos.pos_lnum));
        print_newline ();;



    let id_starts_lowercase s = 
        if ((String.length s) = 0) then (
           parse_error "Empty identifier encountered in mu mu_formula.";
           failwith "";
        ) else 
           ((Char.lowercase s.[0]) = s.[0]);;


    let make_pds_mucalc pds int_confs fmla props = 
        let do_config chars (p, w) =
            Words.CharacterSet.union chars (Words.word_get_characters w) in
        let int_alpha = List.fold_left do_config Words.CharacterSet.empty int_confs in
        pds#expand_alphabet int_alpha;
        new Pds_mucalc.pds_mucalc pds int_confs fmla props;;

    let make_parity_game pds props int_confs = 
        let do_config chars (p, w) =
            Words.CharacterSet.union chars (Words.word_get_characters w) in
        let int_alpha = List.fold_left do_config Words.CharacterSet.empty int_confs in
        pds#expand_alphabet int_alpha;
        new Parity_game.parity_game pds props int_confs;;
%}
%token ARROW ABELARD ELOISE RULES_START STATE_PROPS_START EMPTY_WORD  
%token MU_PROP_START PROP_VAL_START BP_START INTERESTING_CONFIGS
%token <string> IDENTIFIER 
%token <string> TICKED_IDENTIFIER 
%token <int> INTEGER 
%token BOX 
%token DIAMOND
%token BACKBOX BACKDIAMOND
%token LCONSTRAINBOX LCONSTRAINDIAMOND LNEGCONSTRAINBOX LNEGCONSTRAINDIAMOND
%token LCONSTRAINBACKBOX LCONSTRAINBACKDIAMOND LNEGCONSTRAINBACKBOX LNEGCONSTRAINBACKDIAMOND
%token NONDET 
%token ASSIGN 
%token EQ_OP 
%token NE_OP 
%token IMPLIES 
%token DECL 
%token ENFORCE 
%token TERNARY 
%token SEMI_COLON
%token APOS
%token COMMA
%token COLON
%token LPAREN
%token RPAREN
%token LSQUARE
%token RSQUARE
%token BANG
%token TILDE
%token CARET
%token OR
%token AND 
%token LT
%token GT
%token IF 
%token THEN 
%token ELSE 
%token ELSIF 
%token FI 
%token WHILE 
%token DO 
%token OD 
%token RETURN 
%token SKIP 
%token GOTO 
%token BEG 
%token END 
%token BOOL 
%token VOID 
%token ASSERT
%token ASSUME 
%token PRINT 
%token CHOOSE 
%token CONSTRAIN 
%token DEAD 
%token ABORTIF 
%token DFS 
%token DECL 
%token MU NU PUNCT
%token EOF EOL
%token FIXEDPOINT


%right FIXEDPOINT
%left IMPLIES
%left OR
%left AND
%right BANG BOX DIAMOND BACKBOX BACKDIAMOND RSQUARE GT

%start parity_game boolean_program mu_formula pds_mucalc bp_mucalc
%type <Parity_game.parity_game> parity_game
%type <Bp.boolean_program> boolean_program
%type <Mucalc.mu_formula> mu_formula
%type <Pds_mucalc.pds_mucalc> pds_mucalc
%type <Bp_mucalc.bp_mucalc> bp_mucalc
%%

/******************************************************************************************/
/************ PARITY GAMES */

parity_game:
    game_def     { $1 }
;
game_def:
    rules props EOF                     { make_parity_game $1 $2 [] }
  | props rules EOF                     { make_parity_game $2 $1 [] }
  | rules props interesting_configs EOF { make_parity_game $1 $2 $3 }

;
rules:
    RULES_START rule_list  { let pds = new Pds.pds in
                             let do_rule r = pds#add_rule r in
                             List.iter do_rule $2;
                             pds }
;
rule_list:
    rule_list rule { $2::$1 }
  | rule           { [$1] }
;
rule:
    IDENTIFIER IDENTIFIER ARROW IDENTIFIER word SEMI_COLON { 
        Pds.make_rule (Pds.make_control_state $1)
                      (Words.make_character $2)
                      (Pds.make_control_state $4)
                      (Words.make_word $5) 
    }
;
word:
    word IDENTIFIER  { List.append $1 [Words.make_character $2] }
  | IDENTIFIER       { [Words.make_character $1] }
  | EMPTY_WORD { [] }
;
props:
    STATE_PROPS_START prop_list { new Parity_game.property_map $2 }
;
prop_list:
    prop_list property { $2::$1 }
  | property { [$1] }
;
property:
    IDENTIFIER ABELARD INTEGER SEMI_COLON { (Pds.make_control_state $1, Parity_game.A, $3) }
  | IDENTIFIER ELOISE INTEGER SEMI_COLON { (Pds.make_control_state $1, Parity_game.E, $3) }
;
interesting_configs:
    INTERESTING_CONFIGS config_list { $2 }
;
config_list:
    config_list config { $2::$1 }
  | config { [$1] }
;
config:
    IDENTIFIER word SEMI_COLON { (Pds.make_control_state $1, Words.make_word $2) }
;













/******************************************************************************************/
/************ BOOLEAN PROGRAMS */


boolean_program:
    program { $1 }
;


program:
	  g_decl_list fun_list  { ($1, $2) }
	   ;


/***** decls *****/

g_decl_list
	: DECL g_identifier_list SEMI_COLON g_decl_list
	  {
	  	List.append $2 $4
	  }
	| { [] };
	
g_identifier_list
	: g_decl_id
	  {
	  	[$1]
	  } 
    | g_identifier_list COMMA g_decl_id
      {
        $3::$1
      } ;

g_decl_id
	: IDENTIFIER			
	  {
	  	$1
	  } ;
	
decl_list
	: DECL identifier_list SEMI_COLON decl_list
	  {
	  	List.append $2 $4
	  }
	| { [] } ;

identifier_list
	: decl_id
	  {
	  	[$1]
	  }
	| identifier_list COMMA decl_id
	  {
	  	$3::$1
	  } ;

decl_id	: IDENTIFIER			
	  {
	  	$1
	  } ;
	  
/***** functions *****/

fun_list
	: function_definition { [$1] }
	| function_definition fun_list { $1::$2 }
	; 

function_definition
	: function_heading parms BEG decl_list enforce abortif statement_list END
	  {
        let (ret_type, name) = $1 in
        (ret_type, name, $2, $4, $5, $6, $7)
	  }
	 ;

function_heading
	: fun_type IDENTIFIER
	  {
	  	($1, $2)
	  }
	;

fun_type
    : fun_search_order BOOL	{ Bp.Bool(1)  }
	| fun_search_order BOOL LT INTEGER GT { Bp.Bool($4) }
    | fun_search_order VOID	{ Bp.Void }
    ;

fun_search_order
	: DFS { (* don't know what this is, Bob *) }
	| { (* . . . *) }
	;

enforce
	: ENFORCE expression SEMI_COLON
	  {
	  	$2
	  }
	|
	  {
	  	Bp.True
	  } ;

abortif
	: ABORTIF expression SEMI_COLON 
	  {
	  	$2
	  }
	|
	  {
	  	Bp.False
	  } ;

parms	: LPAREN parameter_list RPAREN
	  {
	  	$2
	  }
	  ;

parameter_list
	: identifier_list { $1 }
	| 
	  {
	  	[]
	  } ;

/***** statements *****/

statement_list
    : labelled_statement
      {
	  	[$1]
	  }
	| statement_list labelled_statement
	  {
	  	List.append $1 [$2]
	  }
    | statement_list SEMI_COLON
      {
	  	$1
      }
    ;

labelled_statement
    : label_list statement 
	  {
	  	($1, $2)
	  }
    | statement
	  {
	  	([], $1)
	  }
    ;

label_list
		: label_list IDENTIFIER COLON
		  {
	  		$2::$1
		  }
		| IDENTIFIER COLON 
		  {
	  		[$1]
		  }
		;

target_label_list
		: target_label_list COMMA IDENTIFIER
		  {
		  	List.append $1 [$3]
		  }
		| IDENTIFIER
		  {
		  	[$1]
		  }
		;

statement
    : dead_statement SEMI_COLON { $1 }
    | parallel_assign SEMI_COLON { $1 }
    | assert_stmt SEMI_COLON { $1 }
    | assume SEMI_COLON { $1 }
    | print SEMI_COLON { $1 }
    | call SEMI_COLON { let (name, args) = $1 in Bp.Call([], name, args) }
    | selection_statement { $1 }
    | iteration_statement { $1 }
    | jump_statement SEMI_COLON { $1 }
    ;

dead_statement
	: DEAD assign_id_list
      {
	  	Bp.UnsupportedStatement
      }
	;
	   
parallel_assign 
       	: assign_id_list ASSIGN assign_expression_list assign_constrain_opt
	  {
	  	Bp.Assign($1, $3, $4)
	  }
       	| assign_id_list ASSIGN call
	  { 
        let (id, args) = $3 in
	  	Bp.Call($1, id, args)
	  } ;

assign_constrain_opt
	: {  Bp.True }
	| CONSTRAIN expression
	  {  $2 }
	;

assign_expression_list
	: choose_expression
	  {
	  	[$1]
	  }
	| assign_expression_list COMMA choose_expression
	  {
	  	List.append $1 [$3]
	  } ;
  
assign_id_list
	: asgn_id
	  {
	  	[$1]
	  }
	| assign_id_list COMMA asgn_id
	  {
	  	List.append $1 [$3]
	  } ;

asgn_id	
	: IDENTIFIER			
	  {
	  	$1
	  } ;

assert_stmt	
	: ASSERT expression
	  {
	  	Bp.Assert($2)
	  };
	  
assume	
	: ASSUME expression
	  {
	  	Bp.Assume($2)
	  } ;

print	
	: PRINT LPAREN expression_list RPAREN 
	  {
	  	Bp.UnsupportedStatement
	  } ;

call
	: IDENTIFIER LPAREN expression_list RPAREN 
	  {
	  	($1, $3)
	  }
	| IDENTIFIER LPAREN RPAREN
	  {
	  	($1, [])
	  } ;

selection_statement
	: IF choose_expression THEN statement_list elsif_list
	  {
	  	Bp.If($2, $4, $5)
	  } ;

elsif_list
	: FI
	  {	
	  	[]
	  }
	| ELSE statement_list FI
	  {	
	  	$2
	  }
	| ELSIF choose_expression THEN statement_list elsif_list
	  {	
	  	[([], Bp.If($2, $4, $5))]
	  } ;

iteration_statement
	: WHILE expression DO statement_list OD
	  {
	  	Bp.While($2, $4)
	  } ;

jump_statement
	: RETURN
	  {
	  	Bp.Return([])
	  }
	| RETURN expression_list
	  {
	  	Bp.Return($2)
	  }
	| SKIP
	  {
	  	Bp.Skip
	  }
	| GOTO target_label_list
	  {
	  	Bp.Goto($2)
	  } ;


/***** expressions *****/

primary_expression
	: LPAREN expression RPAREN
	  {
	  	$2
  	  }
    | NONDET
      {
	  	Bp.Nondet
      }
    | CHOOSE LSQUARE expression COMMA expression RSQUARE
	  {
	  	Bp.Choose($3, $5)
	  }
	| IDENTIFIER 	
	  { 
	  	Bp.Id($1)
	  }
	| APOS IDENTIFIER
	  {
	  	Bp.AposId($2)
	  }
	| INTEGER 
	  { 
	  	if $1 = 0 then Bp.False else Bp.True
	  }
	;
	
unary_expression
	: primary_expression { $1 }
	| unary_operator primary_expression 
	  { 
	  	Bp.Un($1, $2)
	  }
	;

unary_operator
	: TILDE { Bp.Not }
	| BANG  { Bp.Not } ;

equality_expression
	: unary_expression  { $1 }
	| equality_expression EQ_OP unary_expression
	  { 
	  	Bp.Bin($1, Bp.Equal, $3)
	  }
	| equality_expression NE_OP unary_expression
	  { 
	  	Bp.Bin($1, Bp.NEqual, $3)
	  }
	;

and_expression
	: equality_expression { $1 }
	| and_expression AND equality_expression
	  { 
	  	Bp.Bin($1, Bp.And, $3)
	  }
	;

exclusive_or_expression
	: and_expression { $1 }
	| exclusive_or_expression CARET and_expression
	  { 
	  	Bp.Bin($1, Bp.XOr, $3)
	  }
	;

or_expression
	: exclusive_or_expression { $1 }
	| or_expression OR exclusive_or_expression
	  { 
	  	Bp.Bin($1, Bp.Or, $3)
	  }
	| or_expression IMPLIES exclusive_or_expression
	  { 
	  	Bp.Bin($1, Bp.Implies, $3)
	  }
	;
	
expression
	: or_expression { $1 }
	| expression TERNARY expression COLON or_expression 
	  { 
	  	Bp.Ternary($1, $3, $5)
	  }
	;

choose_expression
	: expression { $1 }
	;

expression_list
	: choose_expression
	  { 
	  	[$1]
	  }
	| expression_list COMMA choose_expression
	  { 
	  	List.append $1 [$3]
	  }
	;






/*******************************************************************************/
/******************** Mu-Calculus */

mu_formula:
      IDENTIFIER { 
            if (id_starts_lowercase $1) then
                Mucalc.Prop($1)
            else
                Mucalc.Var($1) 
        }
    | BANG mu_formula { Mucalc.Un(Mucalc.Not, $2) }
    | LPAREN mu_formula RPAREN { $2 }
    | BOX mu_formula { Mucalc.Un(Mucalc.Box, $2) }
    | DIAMOND mu_formula { Mucalc.Un(Mucalc.Diamond, $2) }
    | BACKBOX mu_formula { Mucalc.Un(Mucalc.BackBox, $2) }
    | BACKDIAMOND mu_formula { Mucalc.Un(Mucalc.BackDiamond, $2) }
    | LCONSTRAINBOX proposition_list RSQUARE mu_formula { 
            Mucalc.ConstrainUn(Mucalc.ConstrainBox, $2, $4) 
        }
    | LCONSTRAINDIAMOND proposition_list GT mu_formula { 
            Mucalc.ConstrainUn(Mucalc.ConstrainDiamond, $2, $4) 
        }
    | LNEGCONSTRAINBOX proposition_list RSQUARE mu_formula { 
            Mucalc.ConstrainUn(Mucalc.NegConstrainBox, $2, $4) 
        }
    | LNEGCONSTRAINDIAMOND proposition_list GT mu_formula { 
            Mucalc.ConstrainUn(Mucalc.NegConstrainDiamond, $2, $4) 
        }
    | LCONSTRAINBACKBOX proposition_list RSQUARE mu_formula { 
            Mucalc.ConstrainUn(Mucalc.ConstrainBackBox, $2, $4) 
        }
    | LCONSTRAINBACKDIAMOND proposition_list GT mu_formula { 
            Mucalc.ConstrainUn(Mucalc.ConstrainBackDiamond, $2, $4) 
        }
    | LNEGCONSTRAINBACKBOX proposition_list RSQUARE mu_formula { 
            Mucalc.ConstrainUn(Mucalc.NegConstrainBackBox, $2, $4) 
        }
    | LNEGCONSTRAINBACKDIAMOND proposition_list GT mu_formula { 
            Mucalc.ConstrainUn(Mucalc.NegConstrainBackDiamond, $2, $4) 
        }
    | mu_formula AND mu_formula { Mucalc.Bin($1,Mucalc.And,$3) }
    | mu_formula OR mu_formula { Mucalc.Bin($1,Mucalc.Or,$3) }
    | mu_formula IMPLIES mu_formula { Mucalc.Bin($1,Mucalc.Implies,$3) }
    | %prec FIXEDPOINT MU IDENTIFIER PUNCT mu_formula { 
            if (id_starts_lowercase $2) then (
                parse_error ("Variable name '" ^ $2 ^ "' must start with an uppercase character.");
                Mucalc.Mu("parse error",$4)
            ) else
                Mucalc.Mu($2,$4)
        }
    | %prec FIXEDPOINT NU IDENTIFIER PUNCT mu_formula { 
            if (id_starts_lowercase $2) then (
                parse_error ("Variable name '" ^ $2 ^ "' must start with an uppercase character.");
                Mucalc.Nu("parse error",$4)
            ) else
                Mucalc.Nu($2,$4)
        }
;






/****************************************************************************************/
/***************** PDS / MUCALC FORMULA */

pds_mucalc:

    rules mu_property props_val EOF { make_pds_mucalc $1 [] $2 $3 }
  | rules props_val mu_property EOF { make_pds_mucalc $1 [] $3 $2 }
  | mu_property rules props_val EOF { make_pds_mucalc $2 [] $1 $3 }
  | mu_property props_val rules EOF { make_pds_mucalc $3 [] $1 $2 }
  | props_val rules mu_property EOF { make_pds_mucalc $2 [] $3 $1 }
  | props_val mu_property rules EOF { make_pds_mucalc $3 [] $2 $1 }
  | rules interesting_configs mu_property props_val EOF { make_pds_mucalc $1 $2 $3 $4 }
;
mu_property:
    MU_PROP_START mu_formula { $2 }
;
props_val:
    PROP_VAL_START head_val_list { new Pds_mucalc.proposition_map $2 }
;
head_val_list:
    head_val_list head_props { $2::$1 }
  | head_props            { [$1] }
;
head_props:
    IDENTIFIER IDENTIFIER proposition_list SEMI_COLON { 
        (Pds.make_control_state $1, 
         Words.make_character $2,
         $3) 
    }
;
proposition_list:
    proposition_list IDENTIFIER { 
        if (id_starts_lowercase $2) then
            $2::$1
        else (
            parse_error ("In proposition list, proposition '" ^ $2 ^ "' does not start lowercase.");
            failwith "";
        )
    }
  | IDENTIFIER {
         if (id_starts_lowercase $1) then
            [$1]
        else (
            parse_error ("In proposition list, proposition '" ^ $1 ^ "' does not start lowercase.");
            failwith "";
        )
    }
;


/**********************************************************************************************/
/*************  Boolean Program with a Mucalc property  */


bp_mucalc:
    mu_property bp EOF { new Bp_mucalc.bp_mucalc $2 $1 }
  | bp mu_property EOF { new Bp_mucalc.bp_mucalc $1 $2 }
;
bp:
    BP_START boolean_program { $2 }
;
 








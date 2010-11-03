type token =
  | ARROW
  | ABELARD
  | ELOISE
  | RULES_START
  | STATE_PROPS_START
  | EMPTY_WORD
  | MU_PROP_START
  | PROP_VAL_START
  | BP_START
  | INTERESTING_CONFIGS
  | IDENTIFIER of (string)
  | TICKED_IDENTIFIER of (string)
  | INTEGER of (int)
  | BOX
  | DIAMOND
  | BACKBOX
  | BACKDIAMOND
  | LCONSTRAINBOX
  | LCONSTRAINDIAMOND
  | LNEGCONSTRAINBOX
  | LNEGCONSTRAINDIAMOND
  | LCONSTRAINBACKBOX
  | LCONSTRAINBACKDIAMOND
  | LNEGCONSTRAINBACKBOX
  | LNEGCONSTRAINBACKDIAMOND
  | NONDET
  | ASSIGN
  | EQ_OP
  | NE_OP
  | IMPLIES
  | DECL
  | ENFORCE
  | TERNARY
  | SEMI_COLON
  | APOS
  | COMMA
  | COLON
  | LPAREN
  | RPAREN
  | LSQUARE
  | RSQUARE
  | BANG
  | TILDE
  | CARET
  | OR
  | AND
  | LT
  | GT
  | IF
  | THEN
  | ELSE
  | ELSIF
  | FI
  | WHILE
  | DO
  | OD
  | RETURN
  | SKIP
  | GOTO
  | BEG
  | END
  | BOOL
  | VOID
  | ASSERT
  | ASSUME
  | PRINT
  | CHOOSE
  | CONSTRAIN
  | DEAD
  | ABORTIF
  | DFS
  | MU
  | NU
  | PUNCT
  | EOF
  | EOL
  | FIXEDPOINT

val parity_game :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Parity_game.parity_game
val boolean_program :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Bp.boolean_program
val mu_formula :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Mucalc.mu_formula
val pds_mucalc :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Pds_mucalc.pds_mucalc
val bp_mucalc :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Bp_mucalc.bp_mucalc

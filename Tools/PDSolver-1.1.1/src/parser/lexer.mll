{
    open Parser        (* The type token is defined in parser.mli *)

    let update_position lex_buf = 
        let pos = lex_buf.Lexing.lex_curr_p in
        lex_buf.Lexing.lex_curr_p <- { pos with
        Lexing.pos_lnum = pos.Lexing.pos_lnum + 1;
        Lexing.pos_bol = pos.Lexing.pos_cnum };;

}

let d = ['0'-'9']
let l = ['a'-'z''A'-'Z''_''\\''$''#''$''\'']
let h = ['a'-'f''A'-'F''0'-'9']
let e = ['E''e']['+''-']?d+
let fs = ('f'|'F'|'l'|'L')
let is = ('u'|'U'|'l'|'L')*
let rb = '}'



rule token = parse
    | "_"                           { EMPTY_WORD }
    | "->"                          { ARROW }
    | "Abelard"                     { ABELARD }
    | "Eloise"                      { ELOISE }
    | "Rules:"                      { RULES_START }
    | "Control States:"             { STATE_PROPS_START }
    | "Propositions:"               { PROP_VAL_START }
    | "Boolean Program:"            { BP_START }
    | "Mu Property:"                { MU_PROP_START }
    | "Interesting Configurations:" { INTERESTING_CONFIGS }
    | "//"[^'\n']*'\n'              { update_position lexbuf; token lexbuf }
    | "return" 			            { RETURN }
    | "goto" 			            { GOTO }
    | "skip" 			            { SKIP }
    | "dead" 			            { DEAD }
    | "dfs" 			            { DFS }
    | "do" 	    		            { DO }
    | "od" 	    		            { OD }
    | "else" 			            { ELSE }
    | "elsif" 			            { ELSIF }
    | "if" 	    		            { IF }
    | "then" 			            { THEN }
    | "fi" 	    		            { FI }
    | "decl" 			            { DECL }
    | "while"			            { WHILE } 
    | "begin" 			            { BEG }
    | "end" 			            { END }
    | "bool" 			            { BOOL }
    | "void" 			            { VOID }
    | "assert" 			            { ASSERT }
    | "assume" 			            { ASSUME }
    | "print" 			            { PRINT }
    | "enforce" 		            { ENFORCE }
    | "constrain" 		            { CONSTRAIN }
    | "schoose" 		            { CHOOSE }
    | "abortif" 		            { ABORTIF }
    | "mu"                          { MU }
    | "nu"                          { NU }
    | "*" 			                { NONDET }
    | "?" 			                { TERNARY }
    | 'T'                           { INTEGER(1) }
    | 'F'                           { INTEGER(0) }
    | "_"		    	            { IDENTIFIER("temporary id") }
    | (d)+ as num                   { INTEGER(int_of_string num) }
    | l(l|d)* as id                 { IDENTIFIER(id) }
    | ":="                          { ASSIGN }
    | "="                           { EQ_OP }
    | "!="                          { NE_OP }
    | "=>"                          { IMPLIES }
    | ";"                           { SEMI_COLON }
    | "'"                           { APOS }
    | ","                           { COMMA }
    | ":"                           { COLON }
    | "("                           { LPAREN }
    | ")"                           { RPAREN }
    | "["                           { LSQUARE }
    | "]"                           { RSQUARE }
    | "&"                           { AND }
    | "&&"                          { AND }
    | "!"                           { BANG }
    | "~"                           { TILDE }
    | "^"                           { CARET }
    | "|"                           { OR }
    | "||"                          { OR }
    | "<"                           { LT }
    | ">"                           { GT }
    | "."                           { PUNCT }
    | "[]"                          { BOX }
    | "<>"                          { DIAMOND }
    | "~[]"                         { BACKBOX }
    | "~<>"                         { BACKDIAMOND }
    | "[@"                          { LCONSTRAINBOX }
    | "<@"                          { LCONSTRAINDIAMOND }
    | "[\\"                         { LNEGCONSTRAINBOX }
    | "<\\"                         { LNEGCONSTRAINDIAMOND }
    | "~[@"                         { LCONSTRAINBACKBOX }
    | "~<@"                         { LCONSTRAINBACKDIAMOND }
    | "~[\\"                        { LNEGCONSTRAINBACKBOX }
    | "~<\\"                        { LNEGCONSTRAINBACKDIAMOND }
    | '\t'	           	            { token lexbuf }
    | '\n'                          { update_position lexbuf; token lexbuf } 
    | _		        	            { token lexbuf (* ignore bad characters *) }
    | eof                           { EOF }


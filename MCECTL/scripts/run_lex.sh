lex -o build/CTL.yy.c src/CTL.l 
cc build/CTL.yy.c -o build/ctl_lexer -lfl

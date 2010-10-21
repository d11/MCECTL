lex -o build/CTL.yy.c src/CTL.l 
yacc -o build/CTL.tab.c src/CTL.y
cc build/CTL.yy.c build/CTL.tab.c -o build/ctl_parser -lfl

lex -o build/CTL.yy.c src/CTL.l 
yacc --verbose --debug -o build/CTL.tab.cpp src/CTL.ypp
cc -c build/CTL.yy.c -o build/CTL.yy.o
g++ build/CTL.yy.o build/CTL.tab.cpp -o build/ctl_parser -lfl

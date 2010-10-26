default: build/ctl_parser

clean:
	rm -vf build/*.o build/ctl_parser src/CTL.yy.c src/CTL.tab.cpp src/y.tab.h

src/CTL.yy.c: src/CTL.l src/y.tab.h
	lex -o src/CTL.yy.c src/CTL.l 

src/CTL.tab.cpp src/y.tab.h: src/CTL.ypp
	yacc --defines=src/y.tab.h -o src/CTL.tab.cpp src/CTL.ypp

build/CTL.yy.o: src/CTL.yy.c
	cc -c src/CTL.yy.c -o build/CTL.yy.o

build/ctl_parser: build/CTL.yy.o src/CTL.tab.cpp src/ctl_ast.h
	g++ build/CTL.yy.o src/CTL.tab.cpp -o build/ctl_parser -lfl

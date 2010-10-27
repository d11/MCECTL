BUILD=build
SRC=src

TARGET=build/ctl_parser
LEX=lex
YACC=yacc
CC=gcc
CXX=g++

default: $(TARGET)

clean:
	rm -vf build/*.o build/ctl_parser src/CTL.yy.c src/CTL.tab.cpp src/y.tab.h

tags: src/*.cpp src/*.h
	scripts/build_tags.sh

src/CTL.yy.c: src/CTL.l src/y.tab.h
	$(LEX) -o src/CTL.yy.c src/CTL.l 

src/CTL.tab.cpp src/y.tab.h: src/CTL.ypp
	$(YACC) --defines=src/y.tab.h -o src/CTL.tab.cpp src/CTL.ypp

build/CTL.yy.o: src/CTL.yy.c
	$(CC) -c src/CTL.yy.c -o build/CTL.yy.o

build/ctl_parser: build/CTL.yy.o src/CTL.tab.cpp src/ctl_ast.h
	$(CXX) build/CTL.yy.o src/CTL.tab.cpp -o build/ctl_parser -lfl

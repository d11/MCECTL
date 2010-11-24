#
# General declarations
#
CC=g++
CFLAGS=-O3 -DINLINING -Icommon -Ispare -Ispare/ac -Ispare/bf -Ispare/bm -Ispare/cw -Ispare/kmp
CFLAGSLIB=$(CFLAGS) -c
COMPILELIB=$(CC) $(CFLAGSLIB)
CFLAGSAPP=$(CFLAGS)
COMPILEAPP=$(CC) $(CFLAGSAPP)

all: acgrep bmgrep cwgrep kmpgrep test

#
# grep
#
ACLIB=aceftrie.o acgamma.o acmfail.o acmkmpfl.o acmopt.o acout.o
ACDEPEND=commonstravfwd.o $(ACLIB)
BMLIB=bmslfst1.o bmslfst2.o bmslnone.o bmslsfc.o
BMDEPEND=commonstravfwd.o commonstravrev.o $(BMLIB)
CWLIB=cwchar.o cwcharbm.o cwcharrl.o cwd1.o cwd2.o cwdopt.o cwout.o cwshnaiv.o cwshnla.o cwshnorm.o cwshopt.o cwshrla.o cwshwbm.o
CWDEPEND=commonstravrev.o $(CWLIB) 
KMPLIB=pm-kmp.o
KMPDEPEND=commonfailidx.o $(KMPLIB)
ALL=$(ACLIB) $(BMLIB) $(CWLIB) $(KMPLIB) commonstravfwd.o commonstravrev.o commonfailidx.o
acgrep: $(ACDEPEND) spare/acgrep.cpp
	$(COMPILEAPP) -o acgrep $(ACDEPEND) spare/acgrep.cpp
bmgrep: $(BMDEPEND) spare/bmgrep.cpp
	$(COMPILEAPP) -o bmgrep $(BMDEPEND) spare/bmgrep.cpp
cwgrep: $(CWDEPEND) spare/cwgrep.cpp
	$(COMPILEAPP) -o cwgrep $(CWDEPEND) spare/cwgrep.cpp
kmpgrep: $(KMPDEPEND) spare/kmpgrep.cpp
	$(COMPILEAPP) -o kmpgrep $(KMPDEPEND) spare/kmpgrep.cpp
test: $(ALL) spare/test.cpp
	$(COMPILEAPP) -o test $(ALL) spare/test.cpp


#
# Aho-Corasick
#
ac%.o: spare/ac/ac%.cpp
	$(COMPILELIB) -o $@ $<


#
# Boyer-Moore
#
bm%.o: spare/bm/bm%.cpp
	$(COMPILELIB) -o $@ $<


#
# Commentz-Walter
#
cw%.o: spare/cw/cw%.cpp
	$(COMPILELIB) -o $@ $<

#
# Knuth-Morris-Pratt
#
%kmp.o: spare/kmp/%kmp.cpp
	$(COMPILELIB) -o $@ $<


#
# Common
#
common%.o: common/%.cpp
	$(COMPILELIB) -o $@ $<


#
# Clean up
#
clean:
	-rm *.o

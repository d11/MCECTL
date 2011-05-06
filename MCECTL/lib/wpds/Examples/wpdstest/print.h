#include "wpds.h"

#ifndef PRINT_H
#define PRINT_H

extern void print_automaton (wFA*,char*);
extern void print_rule (wRule*);
extern void print_trace (wFA*, wPath*);

#endif

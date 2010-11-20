/**************************************************************************/

/* Internal definitions of the intPds module. */

#ifndef INTINT_H
#define INTINT_H

#include <wpds.h>
#include "bdd.h"

struct intPds;
struct intExpr;

DdNode* intBuild (struct intPds*,struct intExpr*);

#endif

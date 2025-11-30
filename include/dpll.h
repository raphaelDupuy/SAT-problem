#ifndef DPLL_H
#define DPLL_H

#include "satStructure.h"

static struct probleme *copyProbleme(const struct probleme *P);
struct result *satisfyDPLL(struct probleme *P);


#endif
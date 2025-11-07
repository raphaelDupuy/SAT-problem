#ifndef TIMER_h
#define TIMER_H

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

double meanTime(AlgoFunc, struct probleme **P, int count, int *satCount);
double runAndTime(AlgoFunc, struct probleme *P, int *sat);

#endif
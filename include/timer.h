#ifndef TIMER_h
#define TIMER_H

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

double meanTime(AlgoFunc, struct probleme **P, int count);
double runAndTime(AlgoFunc, struct probleme *P);

#endif
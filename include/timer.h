#ifndef TIMER_h
#define TIMER_H

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

double meanTime(int (*algo)(struct probleme *), struct probleme **P, int *count);
AlgoFunc parseAlgo(const char *algoName);
double runAndTime(int (*algo)(struct probleme *), struct probleme *P);

#endif
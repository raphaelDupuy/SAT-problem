#ifndef TIMER_h
#define TIMER_H

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

double runAndTime(int (*algo)(struct probleme *), struct probleme *P);
int (*parseAlgo(const char *algoName))(struct probleme *);

#endif
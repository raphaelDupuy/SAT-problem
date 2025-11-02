#ifndef PARSER_H
#define PARSER_H

#include "satStructure.h"

struct probleme* parseProbleme(const char *F);
struct probleme** parseDbProbleme(const char *F, int *count);

#endif
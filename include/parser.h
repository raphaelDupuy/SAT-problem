#ifndef PARSER_H
#define PARSER_H

#include "satStructure.h"

typedef enum {
    PARSE_FILE,
    PARSE_DIR
}ParseType;

struct parsedPath {
    ParseType type;
    int count;
    struct probleme *single;
    struct probleme **multiple;
};

struct probleme* parseProbleme(const char *F);
struct probleme** parseDbProbleme(const char *F, int *count);
struct parsedPath *parseAll(const char *dirOrFile);
AlgoFunc parseAlgo(const char *algoName);

#endif
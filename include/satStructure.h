#ifndef SATSTRUCTURE_H
#define SATSTRUCTURE_H

typedef int litteral;

struct solution {
    int k;
    int *variables;
};

struct result {
    int exists;
    struct solution sol;
};

struct clause {
    int taille;
    litteral *litteraux;
};

struct probleme {
    int k;
    int variableCount;
    int clauseCount;
    struct clause *fonction;
};

typedef int (*AlgoFunc)(struct probleme *);

char *printSol(struct solution S);
char *printClause(struct clause C);
char *printPb(struct probleme P);
int check(struct probleme *P, struct solution *S);
void freeProbleme(struct probleme *P);
void freeSolution(struct solution *S);
void freeResult(struct result *R);

#endif
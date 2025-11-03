#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "satStructure.h"

char *printSol(struct solution S) {
    int taille = (S.k * 2) + 3;
    char *res = malloc(taille);
    if (!res) return NULL;

    char *ptr = res;
    *ptr++ = '[';

    for (int i = 0; i < S.k; i++) {
        ptr += sprintf(ptr, "%d,", S.variables[i]);
    }

    *(ptr - 1) = ']';
    *ptr = '\0';

    return res;
}


char *printClause(struct clause C) {
    char *res = malloc((2 + 3 * C.taille) * sizeof(char));
    if (!res) return NULL;

    strcpy(res, "(");
    for (int indexLitteral = 0; indexLitteral < C.taille; indexLitteral++) {
        char litt[5];
        sprintf(litt, "%dv", C.litteraux[indexLitteral]);
        strcat(res, litt);
    }

    res[strlen(res)-1] = ')';
    res[strlen(res)] = '\0';
    return res;
}

char *printPb(struct probleme P) {
    char *res = malloc((25 + P.k * sizeof(struct clause)) * sizeof(char));
    strcpy(res, "Fonction à satisfaire: [");
    for (int indexClause = 0; indexClause < P.clauseCount; indexClause++) {
        char *s = printClause(P.fonction[indexClause]);
        strcat(res, s);
        strcat(res, "^");
        free(s);
    }
    res[strlen(res)-1] = ']';
    res[strlen(res)] = '\0';
    return res;
}

void freeProbleme(struct probleme *P) {
    for (int index = 0; index < P->clauseCount; index++) {
        free(P->fonction[index].litteraux);
    }
    free(P);
}

void freeSolution(struct solution *S) {
    free(S->variables);
    free(S);

}

void freeResult(struct result *R) {
    free(R->sol.variables);
    free(R);

}

int check(struct probleme *P, struct solution *S) {
    int resultat = 1;
    for (int indexClause = 0; indexClause < P->clauseCount; indexClause++) {
        int clauseEval = 0;

        struct clause clauseCourante = P->fonction[indexClause];
        for (int indexLitteral = 0; indexLitteral < clauseCourante.taille; indexLitteral++) {
            int variable = clauseCourante.litteraux[indexLitteral];
            if (variable > 0) {
                clauseEval = clauseEval || S->variables[variable - 1];
            } else if (variable < 0) {
                clauseEval = clauseEval || !S->variables[(-variable) - 1];
            }

            if (clauseEval) {
                break;
            }
        }
        resultat = resultat && clauseEval;
        if (!resultat) {
            return 0;
        }
    }
    return 1;
}


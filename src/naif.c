#include <stdlib.h>
#include <stdio.h>
#include "satStructure.h"

int satisfyNaif(struct probleme *P) {
    int nbBits = P->variableCount;
    struct solution *S = malloc(sizeof(struct solution));
    if (!S) { perror("malloc S"); return 0; }

    S->k = nbBits;
    S->variables = malloc(nbBits * sizeof(int));
    if (!S->variables) { perror("malloc variables"); free(S); return 0; }

    for (int i = 0; i < (1 << nbBits); i++) {
        
        for (int j = 0; j < nbBits; j++) {
            S->variables[j] = (i >> j) & 1;
        }

        fflush(stdout);

        if (check(P, S)) {
            free(S->variables);
            free(S);
            return 1;
        }
    }

    printf("\nAucune solution\n");
    free(S->variables);
    free(S);
    return 0;
}


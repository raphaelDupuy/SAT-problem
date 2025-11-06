#include <stdlib.h>
#include <stdio.h>
#include "satStructure.h"

struct result *satisfyNaif(struct probleme *P) {
    
    if (!P) {
        fprintf(stderr, "Erreur: pointeur de problème NULL\n");
        return NULL;
    }
    int nbBits = P->variableCount;

    struct solution *S = malloc(sizeof(struct solution));
    struct result *res = malloc(sizeof(struct result));

    if (!S || !res) {
        perror("malloc");
        free(S);
        free(res);
        exit(EXIT_FAILURE);
    }

    S->k = nbBits;
    S->variables = malloc(nbBits * sizeof(int));
    if (!S->variables) {
        perror("malloc variables");
        free(S);
        free(res);
        exit(EXIT_FAILURE);
    }

    res->exists = 0;
    res->sol.k = 0;
    res->sol.variables = NULL;

    for (int i = 0; i < (1 << nbBits); i++) {
        for (int j = 0; j < nbBits; j++) {
            S->variables[j] = (i >> j) & 1;
        }

        if (check(P, S)) {
            res->exists = 1;

            // Copie propre de la solution trouvée
            res->sol.k = nbBits;
            res->sol.variables = malloc(nbBits * sizeof(int));
            if (!res->sol.variables) {
                perror("malloc res->sol.variables");
                free(S->variables);
                free(S);
                free(res);
                exit(EXIT_FAILURE);
            }
            for (int j = 0; j < nbBits; j++) {
                res->sol.variables[j] = S->variables[j];
            }

            // Libère la solution temporaire
            free(S->variables);
            free(S);
            return res;
        }
    }

    printf("\nAucune solution\n");
    free(S->variables);
    free(S);
    return res;
}
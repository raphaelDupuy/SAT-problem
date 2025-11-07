#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "satStructure.h"

struct result *satisfyNaif(struct probleme *P) {
    if (!P) {
        fprintf(stderr, "Erreur: pointeur de problème NULL\n");
        return NULL;
    }

    int nbBits = P->variableCount;
    if (nbBits < 0) {
        fprintf(stderr, "Erreur: nombre de variables négatif\n");
        return NULL;
    }

    if (nbBits > 30) {
        fprintf(stderr, "Impossible d'énumérer toutes les combinaisons : nbBits = %d > 30\n", nbBits);
        return NULL;
    }

    // allocations
    struct solution *S = malloc(sizeof(struct solution));
    struct result *res = malloc(sizeof(struct result));
    if (!S || !res) {
        perror("malloc");
        free(S); free(res);
        exit(EXIT_FAILURE);
    }

    S->k = nbBits;
    S->variables = malloc((size_t)nbBits * sizeof(int));
    if (!S->variables) {
        perror("malloc variables");
        free(S); free(res);
        exit(EXIT_FAILURE);
    }

    res->exists = 0;
    res->sol.k = 0;
    res->sol.variables = NULL;

    // nb total de combinaisons (valide pour nbBits <= 63)
    uint64_t total = (nbBits == 0) ? 1ULL : (1ULL << nbBits);

    for (uint64_t i = 0; i < total; ++i) {
        for (int j = 0; j < nbBits; ++j) {
            S->variables[j] = (int)((i >> (uint64_t)j) & 1ULL);
        }

        //printf("Trying sol : %s\n", printSol(S));

        if (check(P, S)) {
            
            res->exists = 1;
            res->sol.k = nbBits;
            res->sol.variables = malloc((size_t)nbBits * sizeof(int));
            if (!res->sol.variables) {
                perror("malloc res->sol.variables");
                free(S->variables); free(S); free(res);
                exit(EXIT_FAILURE);
            }
            for (int j = 0; j < nbBits; ++j) res->sol.variables[j] = S->variables[j];

            free(S->variables);
            free(S);
            return res;
        }
    }

    // aucune solution après avoir tout parcouru
    res->exists = 0;
    free(S->variables);
    free(S);
    return res;
}
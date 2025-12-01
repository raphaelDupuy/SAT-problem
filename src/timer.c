#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

double runAndTime(AlgoFunc algo, struct probleme *P, int *sat) {
    if (P->fonction == NULL) {
        fprintf(stderr, "Erreur: P->fonction est NULL !\n");
        *sat = 0;
        return -1;
    }

    clock_t start = clock();
    
    struct result *res = algo(P);
    
    clock_t end = clock();

    // Gérer le cas où res == NULL
    if (res == NULL) {
        *sat = 0; // UNSAT ou erreur
    } else {
        *sat = res->exists;
        free(res->sol.variables);
        free(res);
    }
    
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double meanTime(AlgoFunc algo, struct probleme **P, int count, int *satCount) {
    double sum = 0;
    *satCount = 0;
    
    for (int index = 0; index < count; index++) {
        printf("\rcurrent pb : %d/%d", index + 1, count);
        fflush(stdout);
        
        int sat = 0;
        sum += runAndTime(algo, P[index], &sat);
        *satCount += sat;
    }

    printf("\n");
    return sum / count;
}
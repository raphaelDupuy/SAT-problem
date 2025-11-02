#include <stdlib.h>
#include <stdio.h>

#include "satStructure.h"

int satisfyNaif(struct probleme *P) {
    int nbBits = P->variableCount;
    for (int i = 0; i < (1 << nbBits); i++) {
        int tab[nbBits];
    
        // Extraire chaque bit
        for(int j = 0; j < nbBits; j++) {
            tab[j] = (i >> j) & 1;
        }
        
        struct solution S = {nbBits, tab};

        if (check(P, &S)) {
            printf("1 %s\n", printSol(S));
            return 1;
        }
    }
    return 0;
}

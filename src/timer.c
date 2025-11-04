#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

double runAndTime(AlgoFunc algo, struct probleme *P) {
    clock_t start = clock();
    
    algo(P);
    
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double meanTime(AlgoFunc algo, struct probleme **P, int count) {

    double sum = 0;
    for (int index = 0; index < count; index++) {
        printf("\rcurrent pb : %d/%d", index + 1, count);
        fflush(stdout);
        sum += runAndTime(algo, P[index]);
    }

    return sum / count;
}

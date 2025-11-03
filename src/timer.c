#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

int (*parseAlgo(const char *algoName)) (struct probleme *) {

    if (strcmp(algoName, "naif") == 0) {
        return &satisfyNaif;
    }
       printf("No algorithm named : %s\n", algoName);
       perror("AlgoName parsing");
       return NULL;

}

double runAndTime(int (*algo)(struct probleme *), struct probleme *P) {
    clock_t start = clock();
    
    algo(P);
    
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

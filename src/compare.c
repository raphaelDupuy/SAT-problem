#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "satStructure.h"
#include "timer.h"


// compare {set of algorithms to compare} {path to file or database}
int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s {set of algorithms, e.g. 'nA'} {path}\n", argv[0]);
        return 1;
    }

    const char *algoNames = argv[1];
    const char *targetPath = argv[2];
    struct parsedPath *target = parseAllPaths(targetPath);

    int nbAlgo = 0;
    AlgoFunc *algos = parseAlgoSet(algoNames, &nbAlgo);

    printf("%d problème(s)\n", target->count);

    if (algos == NULL) {
        printf("No valid algorithms given\n");
    }

    if (target->type == PARSE_FILE) {

        if (!target->single) { printf("No problem fpund\n"); } 

        printf("Target is File, %d algo to run\n", nbAlgo);
        for (int index = 0; index < nbAlgo; index++) {
            int sat = 0;
            double execTime = runAndTime(*algos[index], target->single, &sat);
            printf("Satisfaisable : %d\n", sat);
            printf("\nCPU time for %c : %f\n", algoNames[index], execTime);
        }

    } else if (target->type == PARSE_DIR) {
        
        if (!target->multiple) { printf("No problem found\n") ;}

        printf("Target is Dir, nbAlgo : %d\n", nbAlgo);
        for (int index = 0; index < nbAlgo; index++) {
            int satCount = 0;
            double meanExecTime = meanTime(*algos[index], target->multiple, target->count, &satCount);
            printf("\n%d / %d satisfaisables\n", satCount, target->count);
            printf("Average CPU time for %c : %f\n", algoNames[index], meanExecTime);
        }
    }

    free(algos);
    freeParsedPath(*target);
    return 0;

};

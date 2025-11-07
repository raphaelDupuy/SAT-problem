#include <stdio.h>

#include "timer.h"
#include "parser.h"
#include "satStructure.h"

// chrono algo {Path to file or database}
int main(int argc, char *argv[]) {

    (void) argc;
    struct parsedPath *target = parseAllPaths(argv[2]);

    const char *algoName = argv[1];

    int nbAlgo = 0;
    AlgoFunc *algo = parseAlgoSet(algoName, &nbAlgo);
    printf("\n");

    if (!algo) {
        printf("Algorithm parsing not succesfull\n");
    }

    if (target->type == PARSE_FILE) {

        int sat = 0;
        double temps = runAndTime(algo[0], target->single, &sat);
        printf("Sat : %d\n", sat);
        printf("CPU time with %s : %f\n", argv[1], temps);

    } else if (target->type == PARSE_DIR) {
        
        int satCount = 0;
        struct probleme **Pbs = target->multiple;
        double tempsMoyen = meanTime(algo[0], Pbs, target->count, &satCount);
        printf("\n%d / %d satisfaisables\n", satCount, target->count);
        printf("Mean time with %s : %f\n", argv[1], tempsMoyen);

    }

}

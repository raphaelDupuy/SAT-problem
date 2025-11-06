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

    if (!algo) {
        printf("Algorithm parsing not succesfull\n");
    }

    if (target->type == PARSE_FILE) {

        double temps = runAndTime(algo[0], target->single);
        printf("CPU time with %s : %f\n", argv[1], temps);

    } else if (target->type == PARSE_DIR) {
        
        struct probleme **Pbs = target->multiple;
        double tempsMoyen = meanTime(algo[0], Pbs, target->count);
        printf("Mean time with %s : %f", argv[1], meanTime(algo[0], Pbs, tempsMoyen));

    }

}

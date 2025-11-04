#include <stdio.h>

#include "timer.h"
#include "parser.h"
#include "satStructure.h"

int main(int argc, char *argv[]) {

    (void) argc;
    struct parsedPath *path = parseAll(argv[2]);
    AlgoFunc algo = parseAlgo(argv[1]);
    if (path->type == PARSE_FILE) {

        struct probleme *P = path->single;

        printf("Temps avec algo %s : %f", argv[1], runAndTime(algo, P));

    } else if (path->type == PARSE_DIR) {
        
        struct probleme **Pbs = path->multiple;

        int *count = 0;
        printf("Temps moyen avec algo %s : %f", argv[1], meanTime(algo, Pbs, count));

    }

}

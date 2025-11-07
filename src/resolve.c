#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "satStructure.h"
#include "timer.h"

// resolve algo {path to file}
// if no algorithm is given, default is : naïve
int main(int argc, char *argv[]) {

    if (argc != 2 && argc != 3) {
        printf(" resolve algo {path to file} \n if no algorithm is given, default is : naïve\n");
        return 1;
    }

    char *path;
    char *algoName = "n";

    if (argc == 2) {
        path = argv[1]; 

    } else if (argc == 3) {

        algoName = argv[1];
        path = argv[2];
    }

    size_t len = strlen(path);
    if (len < 4 || strcmp(path + len - 4, ".cnf") != 0) {
        printf("Path %s does not lead to a file\n", path); exit(9);
    }

    int nbAlgo = 0;
    AlgoFunc *algo = parseAlgoSet(algoName, &nbAlgo);
    struct probleme *Pb = parseProbleme(path);
    struct result *res = algo[0](Pb);

    if (res->exists) {
        printf("Solution avec algo %s : %s\n", algoName, printSol(&res->sol));
    } else {
        printf("Aucune solution trouvé\n");
    }
}



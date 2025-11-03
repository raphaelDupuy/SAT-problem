#include <stdio.h>

#include "parser.h"
#include "satStructure.h"
#include "timer.h"

int main(int argc, char *argv[]) {
    
    (void) argc;

    char *algoName = argv[1];
    char *target = argv[2];

    printf("Compare Db %s with algo %s\n", target, algoName);
    int count = 0;
    struct probleme **problemes = parseDbProbleme(target, &count);
    printf("%d problèmes\n", count);

    double sum = 0;
    for (int index = 0; index < count; index++) {
        printf("\rcurrent pb : %d/%d", index + 1, count);
        fflush(stdout);
        sum += runAndTime(parseAlgo(algoName), problemes[index]);
    }
    printf("\nmean time : %f\n", sum / count);
};

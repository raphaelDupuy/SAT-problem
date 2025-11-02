#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "naif.h"
#include "parser.h"
#include "satStructure.h"

int (*parseAlgo(const char *algoName))(struct probleme *) {

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

int main(int argc, char **argv) {

    // argv[1] : chrono / compare / resolve
    // argv[2] : algo
    // argv[3] : Db ou path du pb
    
    char *commande = argv[1];
    char *algoName = argv[2];
    char *target = argv[3];
    
    if (strcmp(commande, "chrono") == 0) {
        printf("chrono algo : %s, path : %s\n", algoName, target);

    } else if (strcmp(commande, "compare") == 0) {
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

    } else if (strcmp(commande, "resolve") == 0) {
        printf("resolve algo : %s, path : %s\n", algoName, target);

    } else {
        perror("Commande inconnue");
    }

    //printf("Temps : %f\n", runAndTime(satisfyNaif, P));

    //freeProbleme(P);
    
    return 0;

}

#include <stdio.h>

#include "timer.h"
#include "parser.h"

int main(int argc, char *argv[]) {

    (void) argc;

    printf("Temps avec %s: %f\n", argv[1], runAndTime(parseAlgo(argv[1]), parseProbleme(argv[2])));
}

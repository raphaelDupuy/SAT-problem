#include <stdio.h>

#include "parser.h"
#include "satStructure.h"
#include "timer.h"

int main(int argc, char *argv[]) {
    (void) argc;
    printf("Solution avec algo %s : %d\n", argv[1], (satisfyNaif(parseProbleme(argv[2])) == 0));
}



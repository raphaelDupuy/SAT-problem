#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "satStructure.h"

#ifdef printf
#undef printf
#endif

// Évaluer une clause avec une assignation
// 1 = satisfaite, 0 = non satisfaite
static int evalClause(const struct clause *C, const int *assign) {
    for (int j = 0; j < C->taille; j++) {
        int litt = C->litteraux[j];
        int var = abs(litt);
        int val = assign[var - 1];
        if ((litt > 0 && val == 1) || (litt < 0 && val == 0))
            return 1;
    }
    return 0;
}

// Vérifie si toutes les clauses sont satisfaites
static bool checkFormula(const struct probleme *P, const int *assign) {
    for (int i = 0; i < P->clauseCount; i++) {
        if (!evalClause(&P->fonction[i], assign))
            return false;
    }
    return true;
}

// Trouve une clause violée au hasard
// Retourne l'indice de la clause, ou -1 si aucune n'est violée
static int findViolatedClause(const struct probleme *P, const int *assign) {
    int *violated = malloc(P->clauseCount * sizeof(int));
    int count = 0;

    for (int i = 0; i < P->clauseCount; i++) {
        if (!evalClause(&P->fonction[i], assign)) {
            violated[count++] = i;
        }
    }

    if (count == 0) {
        free(violated);
        return -1;
    }

    int idx = violated[rand() % count];
    free(violated);
    return idx;
}

// Copie un tableau d'assignation
static void copyAssign(int *dest, const int *src, int size) {
    for (int i = 0; i < size; i++) dest[i] = src[i];
}

// Une tentative de Schöning avec une assignation initiale et un nombre de steps
static bool trySchoning(const struct probleme *P, int *assign, int maxSteps) {
    int *current = malloc(P->variableCount * sizeof(int));
    if (!current) { perror("malloc"); return false; }

    copyAssign(current, assign, P->variableCount);

    for (int step = 0; step < maxSteps; step++) {
        // Vérifier si la formule est satisfaite
        if (checkFormula(P, current)) {
            copyAssign(assign, current, P->variableCount);
            free(current);
            return true;
        }

        // Trouver une clause violée au hasard
        int clauseIdx = findViolatedClause(P, current);
        if (clauseIdx == -1) {
            // Toutes les clauses sont satisfaites
            copyAssign(assign, current, P->variableCount);
            free(current);
            return true;
        }

        // Choisir un littéral aléatoire dans la clause violée
        struct clause *C = &P->fonction[clauseIdx];
        if (C->taille > 0) {
            int randLit = C->litteraux[rand() % C->taille];
            int var = abs(randLit);

            // Inverser la valeur de la variable
            if (var > 0 && var <= P->variableCount) {
                current[var - 1] = 1 - current[var - 1];
            }
        }
    }

    free(current);
    return false;
}

struct result *satisfySchoning(struct probleme *P) {
    //printf("Starting Schöning algorithm\n");
    if (!P) return NULL;

    struct result *res = malloc(sizeof(struct result));
    if (!res) { perror("malloc"); return NULL; }

    res->exists = 0;
    res->sol.k = P->variableCount;
    res->sol.variables = malloc(P->variableCount * sizeof(int));

    if (!res->sol.variables) {
        perror("malloc");
        free(res);
        return NULL;
    }

    int *assign = malloc(P->variableCount * sizeof(int));
    if (!assign) {
        perror("malloc");
        free(res->sol.variables);
        free(res);
        return NULL;
    }

    srand((unsigned int)time(NULL));

    //printf("varCount: %d\n", P->variableCount);
    //printf("clauseCount: %d\n", P->clauseCount);

    // Pour 3-SAT, le nombre optimal de steps est environ (4/3)^n
    // Calcul sans libm : maxSteps = 3 * n * n (heuristique simple)
    // ou pour plus de steps : maxSteps = 10 * n * n
    int maxSteps = 10 * P->variableCount * P->variableCount;
    int maxAttempts = 100; // Nombre de tentatives

    //printf("maxSteps per attempt: %d\n", maxSteps);
    //printf("maxAttempts: %d\n", maxAttempts);

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        // Générer une assignation aléatoire initiale
        for (int i = 0; i < P->variableCount; i++) {
            assign[i] = rand() % 2;
        }

        //printf("Attempt %d\n", attempt);

        // Tentative avec marche aléatoire
        if (trySchoning(P, assign, maxSteps)) {
        //    printf("Schöning found solution at attempt %d\n", attempt);
            res->exists = 1;
            for (int i = 0; i < P->variableCount; i++)
                res->sol.variables[i] = assign[i];
            free(assign);
            return res;
        }
    }

    printf("Schöning done - no solution found\n");
    free(assign);
    free(res->sol.variables);
    free(res);
    return NULL;
}
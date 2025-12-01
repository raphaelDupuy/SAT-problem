#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "satStructure.h"

#ifdef printf
#undef printf
#endif

// Évaluer une clause avec une assignation
// 1 = satisfaite, 0 = non déterminée, -1 = violée
static int evalClause(const struct clause *C, const int *assign) {
    bool hasUnassigned = false;
    for (int j = 0; j < C->taille; j++) {
        int litt = C->litteraux[j];
        int var = abs(litt);
        int val = assign[var - 1];
        if (val == -1) hasUnassigned = true;
        else if ((litt > 0 && val == 1) || (litt < 0 && val == 0))
            return 1; // Clause satisfaite
    }
    return hasUnassigned ? 0 : -1; // Non déterminée ou violée
}

// Vérifie si toutes les clauses sont satisfaites
static bool checkFormula(const struct probleme *P, const int *assign) {
    for (int i = 0; i < P->clauseCount; i++) {
        int status = evalClause(&P->fonction[i], assign);
        if (status == -1) return false; // Clause violée
    }
    return true;
}

// Trouve une clause unitaire et renvoie son littéral
static int findUnitClause(const struct probleme *P, const int *assign) {
    for (int i = 0; i < P->clauseCount; i++) {
        const struct clause *C = &P->fonction[i];
        int unassignedLit = 0;
        int unassignedCount = 0;
        bool satisfied = false;

        for (int j = 0; j < C->taille; j++) {
            int litt = C->litteraux[j];
            int var = abs(litt);
            int val = assign[var - 1];
            if (val == -1) { unassignedLit = litt; unassignedCount++; }
            else if ((litt > 0 && val == 1) || (litt < 0 && val == 0)) { satisfied = true; break; }
        }

        if (!satisfied && unassignedCount == 1) return unassignedLit;
    }
    return 0;
}

// Propagation unitaire
static bool unitPropagation(const struct probleme *P, int *assign) {
    bool changed = true;
    while (changed) {
        changed = false;
        int unitLit = findUnitClause(P, assign);
        if (unitLit != 0) {
            int var = abs(unitLit);
            int val = (unitLit > 0) ? 1 : 0;
            if (assign[var - 1] != -1 && assign[var - 1] != val) return false;
            if (assign[var - 1] == -1) {
                assign[var - 1] = val;
                changed = true;
                if (checkFormula(P, assign) == false) return false;
            }
        }
    }
    return true;
}

// Trouve une clause violée au hasard
static int findViolatedClause(const struct probleme *P, const int *assign) {
    int *violated = malloc(P->clauseCount * sizeof(int));
    int count = 0;

    for (int i = 0; i < P->clauseCount; i++) {
        int status = evalClause(&P->fonction[i], assign);
        if (status == -1) { // Clause violée
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

// Une tentative de Schöning amélioré avec unit propagation
static bool trySchoningOptimized(const struct probleme *P, int *assign, int maxSteps) {
    int *current = malloc(P->variableCount * sizeof(int));
    if (!current) { perror("malloc"); return false; }

    copyAssign(current, assign, P->variableCount);

    for (int step = 0; step < maxSteps; step++) {
        // Appliquer la propagation unitaire
        int *backup = malloc(P->variableCount * sizeof(int));
        copyAssign(backup, current, P->variableCount);
        
        if (!unitPropagation(P, current)) {
            // Propagation a échoué, restaurer et faire un flip aléatoire
            copyAssign(current, backup, P->variableCount);
            
            // Trouver une clause violée et faire un flip aléatoire
            int clauseIdx = findViolatedClause(P, current);
            if (clauseIdx == -1) {
                // Plus de clause violée
                free(backup);
                copyAssign(assign, current, P->variableCount);
                free(current);
                return true;
            }
            
            struct clause *C = &P->fonction[clauseIdx];
            if (C->taille > 0) {
                int randLit = C->litteraux[rand() % C->taille];
                int var = abs(randLit);
                if (var > 0 && var <= P->variableCount) {
                    current[var - 1] = 1 - current[var - 1];
                }
            }
        }

        free(backup);

        // Vérifier si la formule est satisfaite
        if (checkFormula(P, current)) {
            copyAssign(assign, current, P->variableCount);
            free(current);
            return true;
        }

        // Trouver une clause violée
        int clauseIdx = findViolatedClause(P, current);
        if (clauseIdx == -1) {
            // Toutes les clauses sont satisfaites
            copyAssign(assign, current, P->variableCount);
            free(current);
            return true;
        }

        // Choisir un littéral aléatoire dans la clause violée et faire un flip
        struct clause *C = &P->fonction[clauseIdx];
        if (C->taille > 0) {
            int randLit = C->litteraux[rand() % C->taille];
            int var = abs(randLit);
            if (var > 0 && var <= P->variableCount) {
                current[var - 1] = 1 - current[var - 1];
            }
        }
    }

    free(current);
    return false;
}

struct result *satisfySchoningU(struct probleme *P) {
    //printf("Starting Schöning algorithm (optimized with unit propagation)\n");
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

    // Nombre de steps et tentatives
    int maxSteps = 10 * P->variableCount * P->variableCount;
    int maxAttempts = 100;

    //printf("maxSteps per attempt: %d\n", maxSteps);
    //printf("maxAttempts: %d\n", maxAttempts);

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        // Générer une assignation aléatoire initiale
        for (int i = 0; i < P->variableCount; i++) {
            assign[i] = rand() % 2;
        }

        //printf("Attempt %d\n", attempt);

        // Tentative avec marche aléatoire et propagation unitaire
        if (trySchoningOptimized(P, assign, maxSteps)) {
            //printf("Schöning found solution at attempt %d\n", attempt);
            res->exists = 1;
            for (int i = 0; i < P->variableCount; i++)
                res->sol.variables[i] = assign[i];
            free(assign);
            return res;
        }
    }

    //printf("Schöning done - no solution found\n");
    free(assign);
    free(res->sol.variables);
    free(res);
    return NULL;
}
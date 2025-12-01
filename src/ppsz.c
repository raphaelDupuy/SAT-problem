#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "satStructure.h"

#ifdef printf
#undef printf
#endif

// Évaluer une clause avec une assignation partielle
// 1 = satisfaite, 0 = non déterminée, -1 = violée
static int evalClause(const struct clause *C, const int *assign) {
    bool hasUnassigned = false;
    for (int j = 0; j < C->taille; j++) {
        int litt = C->litteraux[j];
        int var = abs(litt);
        int val = assign[var - 1];
        if (val == -1) hasUnassigned = true;
        else if ((litt > 0 && val == 1) || (litt < 0 && val == 0))
            return 1;
    }
    return hasUnassigned ? 0 : -1;
}

// Vérifie si le problème est SAT, UNSAT ou indéterminé
static int checkFormula(const struct probleme *P, const int *assign) {
    bool allSatisfied = true;
    for (int i = 0; i < P->clauseCount; i++) {
        int status = evalClause(&P->fonction[i], assign);
        if (status == -1) return -1;
        if (status == 0) allSatisfied = false;
    }
    return allSatisfied ? 1 : 0;
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
            if (val == -1) {
                unassignedLit = litt;
                unassignedCount++;
            } else if ((litt > 0 && val == 1) || (litt < 0 && val == 0)) {
                satisfied = true;
                break;
            }
        }

        if (!satisfied && unassignedCount == 1) return unassignedLit;
    }
    return 0;
}

// Propagation unitaire complète (propage toutes les clauses unitaires)
static bool unitPropagation(const struct probleme *P, int *assign) {
    bool changed = true;
    while (changed) {
        changed = false;
        int unitLit = findUnitClause(P, assign);
        if (unitLit != 0) {
            int var = abs(unitLit);
            int val = (unitLit > 0) ? 1 : 0;
            
            // Contradiction détectée
            if (assign[var - 1] != -1 && assign[var - 1] != val) 
                return false;
            
            if (assign[var - 1] == -1) {
                assign[var - 1] = val;
                changed = true;
                
                // Vérifier si contradiction après assignation
                if (checkFormula(P, assign) == -1) 
                    return false;
            }
        }
    }
    return true;
}

// Trouve un littéral pur
static int findPureLiteral(const struct probleme *P, const int *assign) {
    int *polarity = calloc(P->variableCount, sizeof(int));
    if (!polarity) { perror("calloc"); exit(1); }

    for (int i = 0; i < P->clauseCount; i++) {
        if (evalClause(&P->fonction[i], assign) == 1) continue;
        const struct clause *C = &P->fonction[i];
        for (int j = 0; j < C->taille; j++) {
            int litt = C->litteraux[j];
            int var = abs(litt);
            if (assign[var - 1] != -1) continue;
            int idx = var - 1;
            if (polarity[idx] == 0) 
                polarity[idx] = (litt > 0) ? 1 : -1;
            else if ((polarity[idx] == 1 && litt < 0) || 
                     (polarity[idx] == -1 && litt > 0))
                polarity[idx] = 2;
        }
    }

    for (int v = 0; v < P->variableCount; v++) {
        if (assign[v] == -1 && (polarity[v] == 1 || polarity[v] == -1)) {
            int result = (v + 1) * polarity[v];
            free(polarity);
            return result;
        }
    }

    free(polarity);
    return 0;
}

// Une exécution de PPSZ
static bool ppszIteration(const struct probleme *P, int *assign) {
    // Initialiser toutes les variables à non assignées
    for (int i = 0; i < P->variableCount; i++) {
        assign[i] = -1;
    }
    
    // Créer une permutation aléatoire
    int *perm = malloc(P->variableCount * sizeof(int));
    for (int i = 0; i < P->variableCount; i++) {
        perm[i] = i;
    }
    
    // Mélange de Fisher-Yates
    for (int i = P->variableCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
    
    // Parcourir les variables dans l'ordre de la permutation
    for (int idx = 0; idx < P->variableCount; idx++) {
        int v = perm[idx];
        
        // Si la variable est déjà assignée (par propagation), passer à la suivante
        if (assign[v] != -1) {
            continue;
        }
        
        // Assignation aléatoire
        assign[v] = rand() % 2;
        
        // Propager toutes les conséquences de cette assignation
        if (!unitPropagation(P, assign)) {
            free(perm);
            return false; // Contradiction détectée
        }
        
        // Appliquer les littéraux purs après propagation
        int pureLit;
        while ((pureLit = findPureLiteral(P, assign)) != 0) {
            int var = abs(pureLit);
            int val = (pureLit > 0) ? 1 : 0;
            assign[var - 1] = val;
            
            // Propager après assignation du littéral pur
            if (!unitPropagation(P, assign)) {
                free(perm);
                return false;
            }
        }
    }
    
    free(perm);
    
    // Vérifier si l'assignation complète satisfait la formule
    return checkFormula(P, assign) == 1;
}

// Implémentation PPSZ avec répétitions
struct result *satisfyPPSZ(struct probleme *P) {
    if (!P) return NULL;
    
    // Initialiser le générateur aléatoire
    srand((unsigned int)time(NULL));
    
    struct result *res = malloc(sizeof(struct result));
    res->exists = 0;
    res->sol.k = P->variableCount;
    res->sol.variables = NULL;
    
    int *assign = malloc(P->variableCount * sizeof(int));
    
    // Calculer le nombre d'itérations (heuristique)
    // Pour PPSZ, on utilise typiquement O(n * 2^((1-ε)n)) itérations
    // Ici on utilise une heuristique plus pratique
    int maxIterations = P->variableCount * 100; // Ajuster selon les besoins
    if (maxIterations > 100000) maxIterations = 100000; // Limite supérieure
    
    //printf("PPSZ: Starting with %d iterations (n=%d, m=%d)\n", 
    //       maxIterations, P->variableCount, P->clauseCount);
    
    // Exécuter PPSZ plusieurs fois
    for (int iter = 0; iter < maxIterations; iter++) {
        if (iter % 1000 == 0 && iter > 0) {
            // printf("PPSZ: Iteration %d/%d\n", iter, maxIterations);
        }
        
        if (ppszIteration(P, assign)) {
            // Solution trouvée !
            res->exists = 1;
            res->sol.variables = malloc(P->variableCount * sizeof(int));
            for (int i = 0; i < P->variableCount; i++) {
                res->sol.variables[i] = assign[i];
            }
            
            // printf("PPSZ: Solution found at iteration %d\n", iter);
            free(assign);
            return res;
        }
    }
    
    printf("PPSZ: No solution found after %d iterations\n", maxIterations);
    
    // Aucune solution trouvée
    free(assign);
    return res;
}

// Version hybride : essayer PPSZ d'abord, puis DPLL si échec
struct result *satisfyPPSZWithFallback(struct probleme *P) {
    printf("Trying PPSZ first...\n");
    struct result *res = satisfyPPSZ(P);
    
    if (res && res->exists == 1) {
        printf("PPSZ succeeded!\n");
        return res;
    }
    
    printf("PPSZ failed, falling back to DPLL...\n");
    if (res) {
        free(res);
    }
    
    // Appeler DPLL (vous devez avoir cette fonction disponible)
    // return satisfyDPLL(P);
    
    return NULL; // Ou appelez votre fonction DPLL ici
}
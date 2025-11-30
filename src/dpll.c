#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "satStructure.h"

// Évaluer une clause avec une assignation partielle
// Retourne: 1 = satisfaite, 0 = non déterminée, -1 = violée
static int evalClause(const struct clause *C, const int *assign) {
    bool hasUnassigned = false;
    
    for (int j = 0; j < C->taille; j++) {
        int litt = C->litteraux[j];
        int var = abs(litt);
        int val = assign[var - 1];
        
        if (val == -1) {
            hasUnassigned = true;
        } else {
            // Vérifier si le littéral est satisfait
            if ((litt > 0 && val == 1) || (litt < 0 && val == 0)) {
                return 1; // Clause satisfaite
            }
        }
    }
    
    return hasUnassigned ? 0 : -1; // 0 = non déterminée, -1 = violée
}

// Vérifier si toutes les clauses sont satisfaites ou s'il y a une violation
// Retourne: 1 = SAT, 0 = non déterminé, -1 = UNSAT
static int checkFormula(const struct probleme *P, const int *assign) {
    bool allSatisfied = true;
    
    for (int i = 0; i < P->clauseCount; i++) {
        int status = evalClause(&P->fonction[i], assign);
        if (status == -1) return -1; // Clause violée
        if (status == 0) allSatisfied = false;
    }
    
    return allSatisfied ? 1 : 0;
}

// Trouver une clause unitaire
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
        
        if (!satisfied && unassignedCount == 1) {
            return unassignedLit;
        }
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
            
            if (assign[var - 1] != -1 && assign[var - 1] != val) {
                return false; // Contradiction
            }
            
            if (assign[var - 1] == -1) {
                assign[var - 1] = val;
                changed = true;
                
                // Vérifier immédiatement si cela crée une contradiction
                if (checkFormula(P, assign) == -1) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

// Trouver un littéral pur
static int findPureLiteral(const struct probleme *P, const int *assign) {
    int *polarity = calloc(P->variableCount, sizeof(int));
    
    for (int i = 0; i < P->clauseCount; i++) {
        const struct clause *C = &P->fonction[i];
        
        // Ignorer les clauses déjà satisfaites
        if (evalClause(C, assign) == 1) continue;
        
        for (int j = 0; j < C->taille; j++) {
            int litt = C->litteraux[j];
            int var = abs(litt);
            
            if (assign[var - 1] != -1) continue; // Déjà assignée
            
            int idx = var - 1;
            if (polarity[idx] == 0) {
                polarity[idx] = (litt > 0) ? 1 : -1;
            } else if ((polarity[idx] == 1 && litt < 0) || 
                       (polarity[idx] == -1 && litt > 0)) {
                polarity[idx] = 2; // Non pur
            }
        }
    }
    
    // Trouver un littéral pur
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

// Choisir un littéral non assigné
static int chooseLiteral(const struct probleme *P, const int *assign) {
    for (int i = 0; i < P->clauseCount; i++) {
        if (evalClause(&P->fonction[i], assign) != 0) continue;
        
        const struct clause *C = &P->fonction[i];
        for (int j = 0; j < C->taille; j++) {
            int litt = C->litteraux[j];
            int var = abs(litt);
            if (assign[var - 1] == -1) {
                return litt;
            }
        }
    }
    return 0;
}

// DPLL récursif
static bool dpll(const struct probleme *P, int *assign) {
    // Vérifier l'état actuel
    int status = checkFormula(P, assign);
    if (status == 1) return true;   // SAT
    if (status == -1) return false; // UNSAT
    
    // Propagation unitaire
    if (!unitPropagation(P, assign)) {
        return false;
    }
    
    // Vérifier à nouveau après propagation
    status = checkFormula(P, assign);
    if (status == 1) return true;
    if (status == -1) return false;
    
    // Littéraux purs
    int pureLit;
    while ((pureLit = findPureLiteral(P, assign)) != 0) {
        int var = abs(pureLit);
        int val = (pureLit > 0) ? 1 : 0;
        assign[var - 1] = val;
        
        status = checkFormula(P, assign);
        if (status == 1) return true;
        if (status == -1) return false;
    }
    
    // Choisir un littéral
    int chosenLit = chooseLiteral(P, assign);
    if (chosenLit == 0) {
        return checkFormula(P, assign) == 1;
    }
    
    int var = abs(chosenLit);
    
    // Essayer d'abord la valeur suggérée par le littéral choisi
    int firstVal = (chosenLit > 0) ? 1 : 0;
    
    for (int attempt = 0; attempt < 2; attempt++) {
        int val = (attempt == 0) ? firstVal : (1 - firstVal);
        
        // Créer une copie de l'assignation
        int *assignCopy = malloc(P->variableCount * sizeof(int));
        memcpy(assignCopy, assign, P->variableCount * sizeof(int));
        
        assignCopy[var - 1] = val;
        
        if (dpll(P, assignCopy)) {
            // Copier la solution trouvée
            memcpy(assign, assignCopy, P->variableCount * sizeof(int));
            free(assignCopy);
            return true;
        }
        
        free(assignCopy);
    }
    
    return false;
}

// Interface publique
struct result *satisfyDPLL(struct probleme *P) {
    if (!P) return NULL;
    
    struct result *res = malloc(sizeof(struct result));
    res->exists = 0;
    res->sol.k = 0;
    res->sol.variables = NULL;
    
    int *assign = malloc(P->variableCount * sizeof(int));
    for (int i = 0; i < P->variableCount; i++) {
        assign[i] = -1;
    }
    
    if (dpll(P, assign)) {
        res->exists = 1;
        res->sol.k = P->variableCount;
        res->sol.variables = malloc(P->variableCount * sizeof(int));
        
        for (int i = 0; i < P->variableCount; i++) {
            res->sol.variables[i] = (assign[i] == -1) ? 0 : assign[i];
        }
    }
    
    free(assign);
    return res;
}
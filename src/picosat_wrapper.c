#include <stdio.h>
#include <stdlib.h>
#include "satStructure.h"
#include "/mnt/c/Users/dupuy/Uvsq/M2/Decidabilite/SAT/SAT-problem/src/picosat/picosat-965/picosat.h"

struct result *satisfyPicosat(struct probleme *P) {
    //printf("Starting Picosat\n");
    
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
    
    //printf("varCount: %d, clauseCount: %d\n", P->variableCount, P->clauseCount);
    
    // Initialiser picosat - NOUVELLE INSTANCE À CHAQUE APPEL
    PicoSAT *pico = picosat_init();
    if (!pico) {
        fprintf(stderr, "Erreur initialisation Picosat\n");
        free(res->sol.variables);
        free(res);
        return NULL;
    }
    
    // Ajouter les clauses à picosat
    for (int i = 0; i < P->clauseCount; i++) {
        struct clause *C = &P->fonction[i];
        
        for (int j = 0; j < C->taille; j++) {
            int lit = C->litteraux[j];
            
            // Ignorer le 0 (terminateur de clause)
            if (lit == 0) continue;
            
            // Vérifier que le littéral est valide
            if (abs(lit) > P->variableCount) {
                fprintf(stderr, "Littéral invalide: %d (max: %d)\n", lit, P->variableCount);
                continue;
            }
            picosat_add(pico, lit);
        }
        picosat_add(pico, 0); // Terminer la clause avec 0
    }
    
    // Résoudre
    int sat = picosat_sat(pico, -1); // -1 = pas de limite d'itérations
    
    //printf("Picosat result code: %d\n", sat);
    
    if (sat == PICOSAT_SATISFIABLE) {
        //printf("Picosat found solution\n");
        res->exists = 1;
        
        // Récupérer la solution
        for (int i = 1; i <= P->variableCount; i++) {
            int val = picosat_deref(pico, i);
            res->sol.variables[i - 1] = (val > 0) ? 1 : 0;
        }
    } else if (sat == PICOSAT_UNSATISFIABLE) {
        //printf("Picosat: UNSAT\n");
        res->exists = 0;
    } else {
        //printf("Picosat: UNKNOWN\n");
        res->exists = 0;
    }
    
    // Cleanup - libérer COMPLÈTEMENT l'instance
    fflush(stdout);
    picosat_reset(pico);
    fflush(stdout);
    
    if (res->exists == 0) {
        //printf("Liberating resources (UNSAT)\n");
        fflush(stdout);
        free(res->sol.variables);
        free(res);
        return NULL;
    }
    
    //printf("Returning result (SAT)\n");
    fflush(stdout);
    return res;
}
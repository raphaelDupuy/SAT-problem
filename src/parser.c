#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "satStructure.h"

struct probleme *parseProbleme(const char* F) {
    FILE *f = fopen(F, "r");
    if (!f) { perror("Erreur ouverture fichier"); exit(100); }

    struct probleme *P = malloc(sizeof(struct probleme));
    if (!P) { perror("malloc"); exit(40); }

    char ligne[512];
    int nbVariables = 0, nbClauses = 0;

    // trouve la ligne "p cnf"
    while (fgets(ligne, sizeof(ligne), f)) {
        if (ligne[0] == 'c') continue; // commentaires
        if (ligne[0] == 'p') {
            if (sscanf(ligne, "p cnf %d %d", &nbVariables, &nbClauses) != 2) {
                fprintf(stderr, "Erreur lecture ligne CNF\n"); exit(1);
            }
            break;
        }
    }

    P->variableCount = nbVariables;
    P->clauseCount = nbClauses;
    P->k = 0; // valeur k

    // Allocation du tableau de clauses
    P->fonction = malloc(nbClauses * sizeof(struct clause));
    if (!P->fonction) { perror("malloc clauses"); exit(2); }

    // Lire chaque clause
    int clauseIndex = 0;
    while (fgets(ligne, sizeof(ligne), f) && clauseIndex < nbClauses) {
        if (ligne[0] == 'c' || ligne[0] == 'p') continue;

        // Compter le nombre de littéraux
        int nLit = 0;
        char *tmp = strdup(ligne); // pour strtok sans altérer ligne
        char *token = strtok(tmp, " \t\n");
        while (token) { nLit++; token = strtok(NULL, " \t\n"); }
        free(tmp);

        if (nLit == 0) continue;

        // Allocation dynamique des littéraux
        int *litteraux = malloc(nLit * sizeof(int));
        if (!litteraux) { perror("malloc clause litteraux"); exit(3); }

        int i = 0;
        token = strtok(ligne, " \t\n");
        while (token && i < nLit) {
            litteraux[i++] = atoi(token);
            token = strtok(NULL, " \t\n");
        }

        // Stocker dans la structure
        P->fonction[clauseIndex].taille = nLit;
        P->fonction[clauseIndex].litteraux = litteraux;

        clauseIndex++;
    }

    fclose(f);
    return P;
}

struct probleme **parseDbProbleme(const char* dirname, int *count) {
    
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirname);
    if (!dir) { perror("Erreur ouverture dossier"); exit(200); return NULL;}

    struct probleme **problemes = NULL;
    *count = 0;

    while ((entry = readdir(dir)) != NULL) {
        //check
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

        if (strstr(entry->d_name, ".cnf") == NULL)
        continue;

        //construction chemin
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
    

        printf("\rLecture : %s", path);
        fflush(stdout);

        //parse fichier du path
        struct probleme *P = parseProbleme(path);
        if (!P) {
            fprintf(stderr, "Erreur parsing %s\n", path);
            continue;
        }

        *count += 1;
        problemes = realloc(problemes, (*count) * sizeof(struct probleme *));
        problemes[*count - 1] = P;

    }

    closedir(dir);
    printf("\nParsing successful\n");
    return problemes;
}


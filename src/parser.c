#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "satStructure.h"

struct probleme *parseProbleme(const char* F) {
    FILE *f = fopen(F, "r");

    if (!f) {
        perror("Erreur ouverture fichier");
        exit(100);
    }

    struct probleme *P = malloc(sizeof(struct probleme));
    if (P == NULL) { perror("malloc"); exit(40); }  

    char ligne[512];
    int valeur = 0;
    int lineCount = 0;
    int nbClauses = 0;
    int indexClause = 0;
    int nbVariables;


    while (fgets(ligne, sizeof(ligne), f)) {
        if (lineCount >= 5) {
            if (lineCount == 5) {
                // Extrait k pour k-sat
                valeur = atoi(ligne + 20);
            }

            if (lineCount == 7) {
                // Extrait nbVariables, nbClauses
                if (sscanf(ligne, "p cnf %d %d", &nbVariables, &nbClauses) != 2) {
                    printf("Erreur de lecture de la ligne CNF\n");
                }
                //printf("%d-SAT, %d variables, %d clauses\n", valeur, nbVariables, nbClauses);
            }

            P->k = valeur;
            P->variableCount = nbVariables;
            P->clauseCount = nbClauses;

            struct clause *fonction = malloc(nbClauses * sizeof(struct clause));
            if (fonction == NULL) { perror("malloc"); exit(50); }
            
            P->fonction = fonction;

            if (lineCount > 7 && lineCount < 8 + nbClauses) {
                int litteraux[valeur];

                char *token = strtok(ligne, " \t\n");
                int i = 0;
                while (token && i < valeur) {
                    litteraux[i++] = atoi(token);
                    token = strtok(NULL, " \t\n");
                }

                struct clause C = {valeur, litteraux};
                fonction[indexClause++] = C;
            }
        }
        lineCount++;
    }

    if (fclose(f) != 0) {
        perror("Erreur fermeture fichier");
    }
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


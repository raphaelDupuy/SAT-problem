#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "naif.h"
#include "satStructure.h"

typedef enum {
    PARSE_FILE,
    PARSE_DIR
} ParseType; 

struct parsedPath {
    ParseType type;
    int count;
    struct probleme *single;
    struct probleme **multiple;
};

AlgoFunc parseAlgo(const char *algoName) {

    if (strcmp(algoName, "naif") == 0) {
        return &satisfyNaif;
    }
       printf("No algorithm named : %s\n", algoName);
       perror("AlgoName parsing");
       return NULL;

}

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

    P->fonction = malloc(nbClauses * sizeof(struct clause));
    if (!P->fonction) { perror("malloc clauses"); exit(2); }

    int clauseIndex = 0;
    while (fgets(ligne, sizeof(ligne), f) && clauseIndex < nbClauses) {
        if (ligne[0] == 'c' || ligne[0] == 'p') continue;

        int nLit = 0;
        char *tmp = strdup(ligne); 
        char *token = strtok(tmp, " \t\n");
        while (token) { nLit++; token = strtok(NULL, " \t\n"); }
        free(tmp);

        if (nLit == 0) continue;

        int *litteraux = malloc(nLit * sizeof(int));
        if (!litteraux) { perror("malloc clause litteraux"); exit(3); }

        int i = 0;
        token = strtok(ligne, " \t\n");
        while (token && i < nLit) {
            litteraux[i++] = atoi(token);
            token = strtok(NULL, " \t\n");
        }

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

struct parsedPath *parseAll(const char *dirOrFile) {
    struct parsedPath *result = malloc(sizeof(struct parsedPath));
    if (!result) { perror("malloc"); exit(1); }

    struct stat path_stat;
    if (stat(dirOrFile, &path_stat) != 0) {
        perror("Erreur stat");
        free(result);
        return NULL;
    }

    if (S_ISREG(path_stat.st_mode)) {
        // Cas fichier
        result->type = PARSE_FILE;
        result->count = 1;
        result->single = parseProbleme(dirOrFile);
        result->multiple = NULL;
    } 
    else if (S_ISDIR(path_stat.st_mode)) {
        // Cas dossier
        result->type = PARSE_DIR;
        result->multiple = parseDbProbleme(dirOrFile, &result->count);
        result->single = NULL;
    } 
    else {
        fprintf(stderr, "Erreur: %s n'est ni un fichier ni un dossier\n", dirOrFile);
        free(result);
        return NULL;
    }

    return result;
}


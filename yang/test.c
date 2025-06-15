/**
 * @file test.c
 * @brief Implémentation des fonctions de test pour le système de fichiers.
 */

#include "test.h"
#include <stdio.h>
#include <stdlib.h> // pour malloc, free, realloc, exit
#include <string.h> // pour memset, memcpy, strcpy, strtok, strcspn
#include <unistd.h> // pour chdir

// Définition de la variable globale de statut de partition
PartitionStatus g_partitionStatus;

/**
 * @brief Efface le tampon d'entrée.
 */
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* Ignorer les caractères */ }
}

/**
 * @brief Affiche le répertoire actuel.
 */
void displayCurrentDirectory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Répertoire actuel : %s\n", cwd);
    } else {
        perror("getcwd() a échoué");
    }
}

/**
 * @brief Initialise le statut de la partition à tous libres ('0').
 * 
 * @return Un pointeur vers le statut de partition initialisé.
 */
PartitionStatus* initializePartitionStatus() {
    memset(g_partitionStatus.block_usage, '0', TOTAL_BLOCKS);
    return &g_partitionStatus;
}

/**
 * @brief Visualise le statut de la partition.
 * 
 * @param status Le statut de la partition à visualiser.
 */
void visualizePartitionStatus(PartitionStatus* status) {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        printf("%c", status->block_usage[i] == '0' ? '.' : '*');
        if ((i + 1) % 64 == 0) printf("\n");
    }
}


/**
 * @brief Trouve des blocs libres consécutifs.
 * 
 * @param blocksNeeded Le nombre de blocs libres nécessaires.
 * @return L'index du premier bloc libre trouvé, -1 s'il n'y en a pas assez.
 */
int findFreeBlocks(int blocksNeeded) {
    int freeCount = 0;
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        if (g_partitionStatus.block_usage[i] == '0') {
            ++freeCount;
            if (freeCount == blocksNeeded) {
                return i - blocksNeeded + 1;  // Index du bloc de départ
            }
        } else {
            freeCount = 0;  // Réinitialiser le compteur
        }
    }
    return -1;  // Pas assez de blocs libres trouvés
}

/**
 * @brief Alloue des blocs pour le fichier.
 * 
 * @param f Le pointeur vers la structure de fichier.
 * @param size La taille du fichier à allouer.
 * @return Le nombre de blocs alloués ou -1 s'il n'y a pas assez d'espace.
 */
int allocateBlocks(file* f, int size) {
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int startBlock = findFreeBlocks(blocksNeeded);
    if (startBlock == -1) {
        return -1;  // Pas assez d'espace
    }
    for (int i = startBlock; i < startBlock + blocksNeeded; ++i) {
        g_partitionStatus.block_usage[i] = '1';  // Marquer le bloc comme utilisé
    }
    f->block_start = startBlock;
    f->blocks_count = blocksNeeded;
    return blocksNeeded;
}

/**
 * @brief Libère les blocs alloués au fichier.
 * 
 * @param f Le pointeur vers la structure de fichier.
 */
void freeBlocks(file* f) {
    for (int i = f->block_start; i < f->block_start + f->blocks_count; ++i) {
        g_partitionStatus.block_usage[i] = '0';  // Marquer le bloc comme libre
    }
}

/**
 * @brief Obtient la taille du fichier.
 * 
 * @param f Le pointeur vers la structure de fichier.
 * @return La taille du fichier ou -1 en cas d'erreur.
 */
int getFileSize(const file* f) {
    return f ? f->size : -1;
}

/**
 * @brief Formatte la partition.
 * 
 * @param partitionName Le nom de la partition à formater.
 * @return 0 en cas de réussite, -1 en cas d'échec.
 */
int myFormat(char* partitionName) {
    FILE *fp = fopen(partitionName, "wb+");
    if (!fp) {
        perror("Échec de la création de la partition");
        return -1;
    }

    // Initialiser la partition avec des zéros
    char buffer[PARTITION_SIZE];
    memset(buffer, 0, PARTITION_SIZE);
    if (fwrite(buffer, 1, PARTITION_SIZE, fp) < PARTITION_SIZE) {
        perror("Échec de l'écriture sur la partition");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

/**
 * @brief Ouvre un fichier.
 * 
 * @param fileName Le nom du fichier à ouvrir.
 * @return Un pointeur vers la structure de fichier ou NULL en cas d'échec.
 */
file* myOpen(char* fileName) {
    file *f = (file*)malloc(sizeof(file));
    if (!f) {
        perror("Échec de l'allocation de la structure de fichier");
        return NULL;
    }

    f->name = strdup(fileName);
    f->size = 0;
    f->current_position = 0;
    f->data = NULL;

    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        printf("Le fichier n'existe pas. Voulez-vous le créer ? (o/n) : ");
        char response[3];
        fgets(response, sizeof(response), stdin);
        if (response[0] == 'o' || response[0] == 'O') {
            fp = fopen(fileName, "wb+");
            if (!fp) {
                perror("Échec de la création du fichier");
                free(f->name);
                free(f);
                return NULL;
            }
        } else {
            free(f->name);
            free(f);
            return NULL;
        }
    } else {
        fclose(fp);
        fp = fopen(fileName, "rb+");
    }

    fseek(fp, 0, SEEK_END);
    f->size = ftell(fp);
    rewind(fp);
    
    f->data = (char*)malloc(f->size + 1);
    if (f->data) {
        fread(f->data, f->size, 1, fp);
        f->data[f->size] = '\0';
    }
    
    fclose(fp);
    return f;
}

/**
 * @brief Écrit dans un fichier.
 * 
 * @param f Le pointeur vers la structure de fichier.
 * @param buffer Le tampon contenant les données à écrire.
 * @param nBytes Le nombre d'octets à écrire.
 * @return Le nombre d'octets écrits ou -1 en cas d'erreur.
 */
int myWrite(file* f, void* buffer, int nBytes) {
    if (!f || !buffer || nBytes < 1) {
        fprintf(stderr, "Paramètres non valides pour myWrite.\n");
        return -1;
    }
    
    FILE *fp = fopen(f->name, "rb+");
    if (!fp) {
        perror("Échec de l'ouverture du fichier pour écriture");
        return -1;
    }
    
    fseek(fp, f->current_position, SEEK_SET);
    int bytesWritten = fwrite(buffer, 1, nBytes, fp);
    if (bytesWritten < nBytes) {
        perror("Échec de l'écriture des données dans le fichier");
        fclose(fp);
        return -1;
    }

    f->current_position += bytesWritten;
    if (f->current_position > f->size) {
        f->size = f->current_position;
    }

    fclose(fp);
    return bytesWritten;
}

/**
 * @brief Lit depuis un fichier.
 * 
 * @param f Le pointeur vers la structure de fichier.
 * @param buffer Le tampon où stocker les données lues.
 * @param nBytes Le nombre d'octets à lire.
 * @return Le nombre d'octets lus ou -1 en cas d'erreur.
 */
int myRead(file* f, void* buffer, int nBytes) {
    if (!f || !buffer || nBytes < 1) {
        fprintf(stderr, "Paramètres non valides pour myRead ou à la fin du fichier.\n");
        return -1;
    }

    FILE *fp = fopen(f->name, "rb");
    if (!fp) {
        perror("Échec de l'ouverture du fichier pour lecture");
        return -1;
    }

    memset(buffer, 0, nBytes);

    if (fseek(fp, f->current_position, SEEK_SET) != 0) {
        perror("Échec de la recherche de la position actuelle dans le fichier");
        fclose(fp);
        return -1;
    }

    int bytesRead = fread(buffer, 1, nBytes, fp);
    if (bytesRead <= 0) {
        if (feof(fp)) {
            printf("Fin du fichier atteinte.\n");
        } else {
            perror("Échec de lecture depuis le fichier");
        }
    } else {
        f->current_position += bytesRead;
    }

    fclose(fp);
    return bytesRead;
}

/**
 * @brief Déplace le curseur de lecture/écriture dans le fichier.
 * 
 * @param f Le pointeur vers la structure de fichier.
 * @param offset Le décalage par rapport à la position de base.
 * @param base La position de base à partir de laquelle effectuer le décalage.
 */
void mySeek(file* f, int offset, int base) {
    if (!f) {
        perror("Structure de fichier invalide");
        return;
    }

    // Déterminer la nouvelle position de recherche.
    int new_position;
    switch (base) {
        case SEEK_SET: // depuis le début du fichier
            new_position = offset;
            break;
        case SEEK_CUR: // depuis la position actuelle
            new_position = f->current_position + offset;
            break;
        case SEEK_END: // depuis la fin du fichier
            new_position = f->size + offset;
            break;
        default:
            perror("Base de recherche invalide");
            return;
    }

    // S'assurer que la nouvelle position n'est pas avant le début du fichier.
    if (new_position < 0) {
        perror("La position de recherche est avant le début du fichier");
        new_position = 0;
    }

    // S'assurer également que la nouvelle position n'est pas au-delà de la fin du fichier.
    if (new_position > f->size) {
        perror("La position de recherche est au-delà de la fin du fichier");
        new_position = f->size;
    }

    // Définir la position actuelle du fichier sur la nouvelle position.
    f->current_position = new_position;
}

/**
 * @brief Affiche un message.
 * 
 * @param params Les paramètres passés à la commande echo.
 */
void myEcho(char* params[]) {
    if (params[0] != NULL) {
        printf("%s\n", params[0]);
    } else {
        printf("Aucun message fourni.\n");
    }
}

/**
 * @brief Change le répertoire de travail courant.
 * 
 * @param params Les paramètres passés à la commande cd.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myCd(char **params) {
    if (params[1] == NULL) {
        fprintf(stderr, "cd: opérande manquant\n");
        return -1;
    }
    if (chdir(params[1]) != 0) {
        perror("cd a échoué");
        return -1;
    }
    return 0;
}

/**
 * @brief Supprime un fichier.
 * 
 * @param f Le pointeur vers la structure de fichier à supprimer.
 */
void myDelete(file* f) {
    if (!f) {
        printf("Fichier invalide.\n");
        return;
    }

    // Libérer les blocs de disque occupés par le fichier
    freeBlocks(f);
    
    // Supprimer les fichiers du système de fichiers
    if (remove(f->name) == 0) {
        printf("Fichier supprimé avec succès.\n");
    } else {
        perror("Échec de la suppression du fichier");
    }
    
    // Libérer la mémoire occupée par la structure de fichier
    free(f->name);
    free(f->data);
    free(f);
}

/**
 * @brief Copie un fichier source dans un fichier destination.
 * 
 * @param sourceName Le nom du fichier source.
 * @param destName Le nom du fichier destination.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myCopy(const char* sourceName, const char* destName) {
    FILE *sourceFile = fopen(sourceName, "rb");
    if (!sourceFile) {
        perror("Échec de l'ouverture du fichier source pour la copie");
        return -1;
    }

    FILE *destFile = fopen(destName, "wb");
    if (!destFile) {
        perror("Échec de l'ouverture du fichier destination pour la copie");
        fclose(sourceFile);
        return -1;
    }

    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
        fwrite(buffer, 1, bytesRead, destFile);
    }

    fclose(sourceFile);
    fclose(destFile);
    return 0;
}

/**
 * @brief Renomme un fichier.
 * 
 * @param oldName Le nom du fichier à renommer.
 * @param newName Le nouveau nom du fichier.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myRename(const char* oldName, const char* newName) {
    if (rename(oldName, newName) != 0) {
        perror("Échec du renommage du fichier");
        return -1;
    }
    return 0;
}

/**
 * @brief Déplace un fichier.
 * 
 * @param sourceName Le nom du fichier source.
 * @param destName Le nom du fichier destination.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myMove(const char* sourceName, const char* destName) {
    // Essayer de renommer le fichier directement
    if (rename(sourceName, destName) == 0) {
        printf("Fichier '%s' déplacé vers '%s' avec succès.\n", sourceName, destName);
        return 0;
    } else {
        perror("Le renommage direct a échoué, tentative de copie et de suppression");
    }

    // Si le renommage échoue, copier puis supprimer le fichier d'origine
    if (myCopy(sourceName, destName) == 0) {
        if (remove(sourceName) == 0) {
            printf("Fichier '%s' copié vers '%s' puis l'original a été supprimé avec succès.\n", sourceName, destName);
            return 0;
        } else {
            perror("Échec de la suppression du fichier source après la copie");
        }
    }

    return -1; // Échec du déplacement du fichier
}

/**
 * @brief Termine l'exécution du programme.
 * 
 * @param params Les paramètres passés à la commande exit.
 * @return N/A
 */
int myExit(char **params) {
    printf("Programme terminé.\n");
    exit(0);
}

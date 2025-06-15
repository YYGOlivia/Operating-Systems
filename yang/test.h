/**
 * @file test.h
 * @brief Representation des fonctions , des macros et structures de test pour le système de fichiers.
 */


#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define PARTITION_SIZE 1024 * 1024  /**< Taille de la partition 1 Mo */
#define MAX_LENGTH 1024
#define MAX_PARAMS 20
#define BLOCK_SIZE 512 /**< Taille de chaque bloc de disque */
#define TOTAL_BLOCKS (PARTITION_SIZE / BLOCK_SIZE) /**< Nombre total de blocs */

/**
 * @brief Structure représentant le statut de la partition.
 */
typedef struct {
    char block_usage[TOTAL_BLOCKS]; /**< '0' pour libre, '1' pour utilisé */
} PartitionStatus;

/**
 * @brief Variable globale représentant le statut de la partition.
 */
extern PartitionStatus g_partitionStatus;

/**
 * @brief Structure représentant un fichier.
 */
typedef struct {
    char* name; /**< Nom du fichier */
    int size; /**< Taille du fichier */
    int current_position; /**< Position actuelle dans le fichier */
    char* data; /**< Données du fichier */
    int block_start; /**< Index du bloc de départ */
    int blocks_count; /**< Nombre de blocs utilisés */
} file;

/**
 * @brief Vide le tampon d'entrée.
 */
void clearInputBuffer(void);

/**
 * @brief Affiche le répertoire courant.
 */
void displayCurrentDirectory(void);

/**
 * @brief Initialise le statut de la partition.
 * @return Pointeur vers la structure PartitionStatus initialisée.
 */
PartitionStatus* initializePartitionStatus(void);

/**
 * @brief Visualise le statut des blocs de la partition.
 * @param status Pointeur vers la structure PartitionStatus à visualiser.
 */
void visualizePartitionStatus(PartitionStatus* status);

/**
 * @brief Alloue des blocs de disque pour un fichier.
 * @param f Pointeur vers la structure de fichier.
 * @param size Taille des données à allouer.
 * @return Nombre de blocs alloués avec succès, -1 en cas d'échec.
 */
int allocateBlocks(file* f, int size);

/**
 * @brief Libère les blocs de disque alloués pour un fichier.
 * @param f Pointeur vers la structure de fichier.
 */
void freeBlocks(file* f);

/**
 * @brief Formatte une partition.
 * @param partitionName Nom de la partition à formater.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myFormat(char* partitionName);

/**
 * @brief Ouvre un fichier.
 * @param fileName Nom du fichier à ouvrir.
 * @return Pointeur vers la structure de fichier ou NULL en cas d'échec.
 */
file* myOpen(char* fileName);

/**
 * @brief Écrit des données dans un fichier.
 * @param f Pointeur vers la structure de fichier.
 * @param buffer Tampon contenant les données à écrire.
 * @param nBytes Nombre d'octets à écrire.
 * @return Nombre d'octets écrits avec succès, -1 en cas d'échec.
 */
int myWrite(file* f, void* buffer, int nBytes);

/**
 * @brief Lit des données depuis un fichier.
 * @param f Pointeur vers la structure de fichier.
 * @param buffer Tampon pour stocker les données lues.
 * @param nBytes Nombre d'octets à lire.
 * @return Nombre d'octets lus avec succès, -1 en cas d'échec.
 */
int myRead(file* f, void* buffer, int nBytes);

/**
 * @brief Déplace le curseur de lecture/écriture dans un fichier.
 * @param f Pointeur vers la structure de fichier.
 * @param offset Décalage à appliquer.
 * @param base Position de référence (SEEK_SET, SEEK_CUR, SEEK_END).
 */
void mySeek(file* f, int offset, int base);

/**
 * @brief Obtient la taille d'un fichier.
 * @param f Pointeur vers la structure de fichier.
 * @return Taille du fichier en octets, -1 en cas d'erreur.
 */
int getFileSize(const file* f);

/**
 * @brief Affiche un message.
 * @param params Tableau de chaînes de caractères contenant le message.
 */
void myEcho(char **params);

/**
 * @brief Change le répertoire courant.
 * @param params Tableau de chaînes de caractères contenant le nouveau chemin.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myCd(char **params);

/**
 * @brief Renomme un fichier.
 * @param oldName Ancien nom du fichier.
 * @param newName Nouveau nom du fichier.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myRename(const char* oldName, const char* newName);

/**
 * @brief Déplace un fichier.
 * @param sourceName Nom du fichier source.
 * @param destName Nom du fichier de destination.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myMove(const char* sourceName, const char* destName);

/**
 * @brief Supprime un fichier.
 * @param f Pointeur vers la structure de fichier.
 */
void myDelete(file* f);

/**
 * @brief Copie un fichier.
 * @param sourceName Nom du fichier source.
 * @param destName Nom du fichier de destination.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int myCopy(const char* sourceName, const char* destName);

/**
 * @brief Termine l'exécution du programme.
 * @param params Tableau de chaînes de caractères contenant les paramètres.
 * @return Cette fonction ne retourne pas de valeur.
 */
int myExit(char **params);

#endif // TEST_H

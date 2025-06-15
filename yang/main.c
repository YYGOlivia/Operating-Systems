/**
 * @file main.c
 * @brief Fichier principal du programme de gestion du système de fichiers.
 */

#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Affiche les options du menu.
 */
void displayMenu() {
    printf("\nMenu :\n");
    printf("1. Formater la partition\n");
    printf("2. Ouvrir un fichier\n");
    printf("3. Écrire dans un fichier\n");
    printf("4. Lire à partir d'un fichier\n");
    printf("5. Rechercher dans un fichier\n");
    printf("6. Obtenir la taille du fichier\n");
    printf("7. Supprimer un fichier\n");
    printf("8. Copier un fichier\n");
    printf("9. Renommer un fichier\n");
    printf("10. Déplacer un fichier\n");
    printf("11. Changer de répertoire avec un repertoire existant par exemple tapez : /home \n");
    printf("12. Écho de message\n");
    printf("13. Visualiser l'espace disque\n");
    printf("14. Quitter\n");
    
    printf("Sélectionnez une option : ");
}

/**
 * @brief Fonction principale pour exécuter le programme de gestion du système de fichiers.
 * 
 * @return 0 en cas d'exécution réussie.
 */
int main() {
    int running = 1;
    char choice[3];
    file* f = NULL;

    while (running) {
        displayMenu();
        fgets(choice, sizeof(choice), stdin);
        int option = atoi(choice);

        switch (option) {
            case 1: {
                char partitionName[256];  // Créer un tableau assez grand pour stocker le nom du fichier
                printf("Entrez le nom du fichier de partition à formater : ");
                fgets(partitionName, sizeof(partitionName), stdin);
                partitionName[strcspn(partitionName, "\n")] = 0;  // Supprimer le caractère de nouvelle ligne

                printf("Formatage de la partition \"%s\"...\n", partitionName);
                if (myFormat(partitionName) == 0) {
                    printf("Partition \"%s\" formatée avec succès.\n", partitionName);
                } else {
                    printf("Échec du formatage de la partition \"%s\".\n", partitionName);
                }
                break;
            }
            case 2:
                if (f != NULL) {
                    printf("Fermeture du fichier précédemment ouvert.\n");
                    free(f); // S'assurer que tout fichier précédemment ouvert est fermé
                    f = NULL;
                }
                printf("Entrez le nom du fichier à ouvrir : ");
                char fileName[256];
                fgets(fileName, sizeof(fileName), stdin);
                fileName[strcspn(fileName, "\n")] = 0; // Supprimer le caractère de nouvelle ligne
                f = myOpen(fileName);
                if (f != NULL) {
                    printf("Fichier ouvert avec succès : %s\n", fileName);
                } else {
                    printf("Échec de l'ouverture du fichier : %s\n", fileName);
                }
                break;
            case 3:
                if (f == NULL) {
                    printf("Aucun fichier n'est ouvert.\n");
                    break;
                }
                printf("Entrez le texte à écrire dans le fichier : ");
                char textToWrite[1024];
                fgets(textToWrite, sizeof(textToWrite), stdin);
                textToWrite[strcspn(textToWrite, "\n")] = 0; // Supprimer le caractère de nouvelle ligne
                if (myWrite(f, textToWrite, strlen(textToWrite)) > 0) {
                    printf("Texte écrit dans le fichier avec succès.\n");
                } else {
                    printf("Échec de l'écriture dans le fichier.\n");
                }
                break;
            case 4: // Lire à partir du fichier
                if (f == NULL) {
                    printf("Aucun fichier n'est ouvert.\n");
                } else {
                    // Réinitialiser le pointeur de fichier au début du fichier
                    mySeek(f, 0, SEEK_SET);
                    char readBuffer[1024] = {0};
                    int bytesRead = myRead(f, readBuffer, sizeof(readBuffer) - 1);
                    if (bytesRead > 0) {
                        printf("Lecture de %d octets : %s\n", bytesRead, readBuffer);
                    } else {
                        printf("Échec de la lecture ou fin du fichier.\n");
                    }
                }
                break;
            case 5: // Rechercher dans le fichier
                if (f == NULL) {
                    printf("Aucun fichier n'est ouvert.\n");
                    break;
                }
                printf("Entrez le décalage de recherche : ");
                int offset; // Assurez-vous que les variables sont déclarées dans cette portée
                if (scanf("%d", &offset) == 1) {
                    getchar(); // Consommer le caractère de nouvelle ligne laissé par scanf
                    mySeek(f, offset, SEEK_SET); // Définir la nouvelle position
                    printf("Recherche terminée au décalage %d.\n", offset);
                } else {
                    printf("Entrée invalide. Veuillez entrer un nombre.\n");
                    // Effacer l'entrée incorrecte pour continuer
                    while (getchar() != '\n');
                }
                break;
            case 6: // Obtenir la taille du fichier
                if (f) {
                    printf("Taille du fichier : %d octets\n", getFileSize(f));
                } else {
                    printf("Aucun fichier n'est ouvert.\n");
                }
                break;
            case 7: // Supprimer un fichier
                if (f != NULL) {
                    myDelete(f);  
                    f = NULL;  // Éviter les pointeurs d'erreurs
                } else {
                    printf("Aucun fichier n'est ouvert.\n");
                }
                break;
            case 8: { // Copier un fichier 
                char sourceFileName[256], destFileName[256];

                // Obtenir le nom du fichier source
                printf("Entrez le nom du fichier source : ");
                fgets(sourceFileName, sizeof(sourceFileName), stdin);
                sourceFileName[strcspn(sourceFileName, "\n")] = 0; // Supprimer le caractère de nouvelle ligne

                // Obtenir le nom du fichier de destination
                printf("Entrez le nom du fichier de destination : ");
                fgets(destFileName, sizeof(destFileName), stdin);
                destFileName[strcspn(destFileName, "\n")] = 0; // Supprimer le caractère de nouvelle ligne

                // Appeler myCopy pour la copie de fichier
                if (myCopy(sourceFileName, destFileName) == 0) {
                    printf("Fichier copié avec succès.\n");
                } else {
                    printf("Échec de la copie du fichier.\n");
                }
                break;
            }
            case 9: { // Renommer un fichier
                char oldName[256], newName[256];
                printf("Entrez le nom de fichier actuel : ");
                fgets(oldName, sizeof(oldName), stdin);
                oldName[strcspn(oldName, "\n")] = 0; // Supprimer le caractère de nouvelle ligne

                printf("Entrez le nouveau nom de fichier : ");
                fgets(newName, sizeof(newName), stdin);
                newName[strcspn(newName, "\n")] = 0; // Supprimer le caractère de nouvelle ligne

                if (myRename(oldName, newName) == 0) {
                    printf("Fichier renommé avec succès.\n");
                } else {
                    printf("Échec du renommage du fichier.\n");
                }
                break;
            }
            case 10: { // Déplacer un fichier
                char sourceName[256], destName[256];

                printf("Entrez le nom du fichier source : ");
                clearInputBuffer(); // Effacer le tampon d'entrée avant de lire les noms de fichiers
                fgets(sourceName, sizeof(sourceName), stdin);
                sourceName[strcspn(sourceName, "\n")] = 0;

                printf("Entrez le nom du fichier de destination : ");
                clearInputBuffer(); // Effacer à nouveau le tampon d'entrée
                fgets(destName, sizeof(destName), stdin);
                destName[strcspn(destName, "\n")] = 0;

                if (myMove(sourceName, destName) == 0) {
                    printf("Fichier déplacé avec succès.\n");
                } else {
                    printf("Échec du déplacement du fichier.\n");
                }
                break;
            }
            case 11: // Changer de répertoire (cd)
                printf("Entrez le chemin du répertoire : ");
                clearInputBuffer(); // Nettoyer le tampon d'entrée pour vous assurer qu'il n'y a pas de caractères résiduels
                char dirPath[256];
                fgets(dirPath, sizeof(dirPath), stdin);
                dirPath[strcspn(dirPath, "\n")] = 0; // Supprimer le caractère de nouvelle ligne
                if(strlen(dirPath) > 0) { // Assurez-vous de saisir un chemin valide
                    char *params[] = {NULL, dirPath, NULL}; // Créer un tableau de paramètres
                    if (myCd(params) == 0) { // Tableau de paramètres correctement passé
                        printf("Changement de répertoire vers %s effectué avec succès.\n", dirPath);
                        displayCurrentDirectory(); // Appeler la fonction pour afficher le répertoire actuel
                    } else {
                        printf("Échec du changement de répertoire vers %s.\n", dirPath);
                    }
                } else {
                    printf("Aucun chemin de répertoire saisi.\n");
                }
                break;
            case 12: { // Écho de message
                clearInputBuffer(); // Nettoyer le tampon d'entrée
                printf("Entrez le message à écho : ");
                char message[256];
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = 0; // Supprimer le caractère de nouvelle ligne
                printf("%s\n", message); // Écho du message
                break;
            }
            case 13: // Visualiser l'espace disque
                visualizePartitionStatus(&g_partitionStatus);
                break;
            case 14: // Quitter
                printf("Fermeture...\n");
                running = 0;
                if (f != NULL) {
                    if (f->data != NULL) free(f->data);
                    if (f->name != NULL) free(f->name);
                    free(f);
                }
                exit(0); // Quitter le programme directement
                break;
            default:
                printf("Option invalide.\n");
        }
    }
    // Assurez-vous de libérer toutes les ressources avant de quitter
    if (f != NULL) {
        if (f->data != NULL) free(f->data);
        if (f->name != NULL) free(f->name);
        free(f);
    }

    return 0;
}

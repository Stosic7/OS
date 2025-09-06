#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX 1024

void processdir(char* path1, char* path2) {
    DIR* dp1;
    DIR* dp2;
    struct dirent * dirp1;
    struct dirent* dirp2;
    char fullPath1[MAX];
    char fullPath2[MAX];
    struct stat statbuf1;
    struct stat statbuf2;

    int brIstihDatoteka = 0;

    dp1 = opendir(path1);
    if (dp1 == NULL) exit(-1);

    dp2 = opendir(path2);
    if (dp2 == NULL) exit(-1);

    while((dirp1 = readdir(dp1)) != NULL) {
        if (strcmp(dirp1->d_name, ".") == 0 || strcmp(dirp1->d_name, "..") == 0)
            continue;
        
        strcpy(fullPath1, path1);
        strcat(fullPath1, "/");
        strcat(fullPath1, dirp1->d_name);
        
        stat(fullPath1, &statbuf1);

        if (S_ISREG(statbuf1.st_mode)) {
            rewinddir(dp2);
            printf("Nadjena datoteka u prvom direktorijumu pod nazivom: %s, pretrazujem drugi...\n", fullPath1);
            while((dirp2 = readdir(dp2)) != NULL) {
                if (strcmp(dirp2->d_name, ".") == 0 || strcmp(dirp2->d_name, "..") == 0)
                    continue;

                strcpy(fullPath2, path2);
                strcat(fullPath2, "/");
                strcat(fullPath2, dirp2->d_name);

                stat(fullPath2, &statbuf2);

                if (S_ISREG(statbuf2.st_mode)) {
                    printf("Nadjena datoteka u drugom direktorijumu pod nazivom: %s, uporedjujem...\n", fullPath2);
                    if (strcmp(dirp1->d_name, dirp2->d_name) == 0 && statbuf1.st_size == statbuf2.st_size) {
                        printf("Nadjene dve datoteke sa istim imenom.\n");
                        printf("Naziv datotke iz direktorijuma %s: %s\n", fullPath1, dirp1->d_name);
                        printf("Naziv datotke iz direktorijuma %s: %s\n", fullPath2, dirp2->d_name);
                        brIstihDatoteka++;
                    }
                }
            }
        }
    }

    if (brIstihDatoteka == 0) {
        printf("Nema istih datoteka...\n");
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Nedovoljan broj argumenata\n");
        exit(1);
    }

    processdir(argv[1], argv[2]);

    return 0;
}
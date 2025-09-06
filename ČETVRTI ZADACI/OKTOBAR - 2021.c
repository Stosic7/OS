#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_NAME 1024

void processdir(char* path) {
    char fullPath[MAX_NAME];
    char maxPath[MAX_NAME];
    struct stat statbuf;
    DIR * dp;
    struct dirent * dirp;

    dp = opendir(path);
    if (dp == NULL) {
        printf("Doslo je do greske prilikom otvaranja direktorijuma\n");
        exit(-1);
    }

    for(int i = 0; i < 5; i++) {
        printf("Pretrazujem direktorijum...\n");
        printf("ITERACIJA: %d\n", i+1);
        int found = 0;
        int size = 0;
        rewinddir(dp);
        while((dirp = readdir(dp)) != NULL) {
            strcpy(fullPath, path);
            strcat(fullPath, "/");
            strcat(fullPath, dirp->d_name);

            stat(fullPath, &statbuf);
            if (S_ISREG(statbuf.st_mode)) {
                if(statbuf.st_size > size) {
                    strcpy(maxPath, fullPath);
                    size = statbuf.st_size;
                    found = 1;
                }
            }
        }

        if (found) {
            if (fork() == 0) {
            printf("Brisem file: %s\n", maxPath);
            execlp("rm", "rm", maxPath, NULL);
            } else {
                wait(NULL);
            }
        }
    }
    closedir(dp);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Nedovoljan broj argumenata...\n");
        exit(1);
    }

    processdir(argv[1]);

    return 0;
}
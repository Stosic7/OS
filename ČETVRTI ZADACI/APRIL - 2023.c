#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX 256

static off_t gMaxSize = -1;          
static char  gMaxPath[MAX] = "";     

void processdir(char* path, int level) {
    printf("TRENUTNA DUBINA: %d", level+1);
    printf("\n\nTRENUTNO OBRADJUJEM DIREKTORIJUM: %s\n\n", path);

    DIR * dp;
    struct dirent * dirp;
    char path1[MAX];
    struct stat statbuf;

    dp = opendir(path);
    if (dp == NULL) {
        printf("Greska prilikom otvaranja direktorijuma\n");
        exit(-1);
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        strcpy(path1, path);
        strcat(path1, "/");
        strcat(path1, dirp->d_name);

        stat(path1, &statbuf);

        if (S_ISREG(statbuf.st_mode)) {
            if (statbuf.st_size > gMaxSize) { 
                gMaxSize = statbuf.st_size;
                strcpy(gMaxPath, path1);
            }
        }

        if (S_ISDIR(statbuf.st_mode) && level < 5) {
            processdir(path1, level + 1);
        }
    }

    closedir(dp);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Nedovoljan broj argumenata, potrebno: 2\n");
        exit(1);
    }

    processdir(argv[1], 0);

    if (gMaxSize >= 0) {
        char *name = strrchr(gMaxPath, '/');
        printf("%s\n", name ? name + 1 : gMaxPath);
    } else {
        printf("Nema regularnih datoteka do nivoa 5.\n");
    }

    return 0;
}

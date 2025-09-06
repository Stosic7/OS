#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void processDir(char* path, int* N, int* M) {
    DIR* dp;
    struct dirent* dirp;
    struct stat statbuf;
    int result;
    char fullPath[256];

    int brOtvaranja = 0;

    if ((dp = opendir(path)) == NULL) {
        printf("Greska prilikom otvaranja foldera %s!\n", path);
        return;
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, dirp->d_name);

        if ((result = stat(fullPath, &statbuf)) == -1) {
            printf("Neuspesno citanje podataka o objektu %s\n", dirp->d_name);
            continue;
        }

        if (S_ISREG(statbuf.st_mode) && statbuf.st_size > 0) {
            if (brOtvaranja == *N)
                break;

            printf("Otvaram fajl: %s\n", fullPath);

            FILE* f = fopen(fullPath, "r");
            if (f == NULL) {
                printf("Greska prilikom otvaranja fajla %s\n", fullPath);
                continue;
            }

            int brCitanihRedova = 0;
            char linija[256];

            while (fgets(linija, sizeof(linija), f)) {
                if (brCitanihRedova == *M)
                    break;

                printf("%s", linija);
                brCitanihRedova++;
            }

            fclose(f);
            brOtvaranja++;
        }
    }

    closedir(dp);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Neophodno je uneti 3 argumenta: <putanja> <N> <M>\n");
        return 1;
    }

    int n = atoi(argv[2]);
    int m = atoi(argv[3]);

    processDir(argv[1], &n, &m);

    return 0;
}

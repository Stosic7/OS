#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void pronadjiDatoteku(char* path, char* naziv) {
    DIR* dp;
    struct dirent* dirp;
    struct stat statbuf;
    int result;
    int found = 0;

    if ((dp = opendir(path)) == NULL) {
        printf("Greska prilikom otvaranja foldera %s!\n", path);
        return;
    }

    while((dirp = readdir(dp)) != NULL) {
        if(found)
            break;
        char tmp[1024] = "";
        strcat(tmp, path);
        strcat(tmp, "/");
        strcat(tmp, dirp->d_name);

        printf("Obradjujem %s u %s.\n", dirp->d_name, tmp);

        if ((result = stat(tmp, &statbuf)) == -1)
        {
            printf("Neuspesno citanje podataka o objektu: %s\n", tmp);
            continue;
        }

        if(S_ISREG(statbuf.st_mode)){
            printf("Pronadjena datoteka, uporedjujem...\n");
            if(strcmp(dirp->d_name, naziv) == 0) {
               printf("Nadjeno podudaranje.\n");
               printf("Lokacija: %s\n", tmp);
               printf("Velicina fajla: %lld\n", statbuf.st_size);
               return;
            }
        }

        if (S_ISDIR(statbuf.st_mode) && strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
            printf("Otvaram folder %s.\n", tmp);
            pronadjiDatoteku(tmp, naziv);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Nedovoljan broj argumenata. Potreban: 3\n");
        exit(-1);
    }

    pronadjiDatoteku(argv[1], argv[2]);

    return 0;
}
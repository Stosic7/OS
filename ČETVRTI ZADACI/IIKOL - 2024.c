#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void funkcijaZaStatistiku(char* folderName) {
    DIR* dp;
    struct dirent* dirp;
    int brfldr, brdat, brlnk;
    struct stat statbuf;
    int result;

    int brPodFoldera = 0;
    int brLinkova = 0;
    int brPraznihDatoteka = 0;
    int brPunihDatoteka = 0;

    int konacnaStatistika = 0;

    if ((dp = opendir(folderName)) == NULL) {
        printf("Greska prilikom otvaranja foldera %s!\n", folderName);
        return;
    }

    while((dirp = readdir(dp)) != NULL) {

        char tmp[1024] = "";
        strcat(tmp, folderName);
        strcat(tmp, "/");
        strcat(tmp, dirp->d_name);

        if ((result = stat(tmp, &statbuf)) == -1) {
            printf("Neuspesno citanje podataka o objektu: %s\n", tmp);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            printf("Objekat: %s je direktorijum\n", dirp->d_name);
            brPodFoldera++;
            if (statbuf.st_size > 0)
                konacnaStatistika++;
        }

        if (S_ISREG(statbuf.st_mode) && statbuf.st_size == 0) {
            printf("Objekat: %s je prazna regularna datoteka\n", dirp->d_name);
            brPraznihDatoteka++;
        }

        if (S_ISREG(statbuf.st_mode) && statbuf.st_size > 0) {
            printf("Objekat: %s je regularna datoteka\n", dirp->d_name);
            brPunihDatoteka++;
            konacnaStatistika++;
        }

        if (S_ISLNK(statbuf.st_mode)) {
            printf("Objekat: %s je link\n", dirp->d_name);
            brLinkova++;
            if (statbuf.st_size > 0)
                konacnaStatistika++;
        }
    }

    printf("Statistika za folder:\nPodfolder:%d\nReg. datoteka: %d\nReg. Prazna datoteka: %d\nLinkova:%d\n", brPodFoldera, brPunihDatoteka, brPraznihDatoteka, brLinkova);
    printf("\n\nKonacna statistika za sve fajlove preko 0B: %d", konacnaStatistika);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Neophodna su dva argumenta (putanja do foldera i string koji setrazi)!\n");
        exit(1);
    }

    funkcijaZaStatistiku(argv[1]);

    return 0;
}
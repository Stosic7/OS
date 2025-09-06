#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define LEN 80

int main() {
    int pd1[2];
    int pid;

    if (pipe(pd1) == -1) {
        exit(1);
    }

    if (pid = fork() != 0) {

        close(pd1[0]);

        char putanja[LEN];
        char kljucnaRec[LEN];
        int len;

        printf("Unesi putanju: ");
        gets(putanja);
        printf("\n");
        len = strlen(putanja);

        printf("Unesi kljucnu rec: ");
        gets(kljucnaRec);
        printf("\n");

        write(pd1[1], &putanja, LEN);
        write(pd1[1], &kljucnaRec, LEN);
        write(pd1[1], &len, sizeof(int));

        printf("Poslata putanja: %s", putanja);
        printf("\nPoslata kljucna Rec: %s", kljucnaRec);
        printf("\nPoslata duzana putanje: %d\n", len);
        printf("\n");

        close(pd1[1]);

        wait(NULL);

    } else {

        close(pd1[1]);

        char putanja[LEN];
        char kljucnaRec[LEN];
        int len;

        read(pd1[0], &putanja, LEN);
        read(pd1[0], &kljucnaRec, LEN);
        read(pd1[0], &len, sizeof(int));

        printf("Procitana putanja: %s", putanja);
        printf("\nProcitana kljucna Rec: %s", kljucnaRec);
        printf("\nProcitana duzina putanje: %d", len);
        printf("\n");

        FILE *file = fopen(putanja, "r");
        if (file == NULL) {
            perror("Greska pri otvaranju fajla");
            return 1;
        }

        printf("Fajl je uspesno otvoren: %s\n", putanja);

        char linija[LEN];
        int i = 0;
        while(!feof(file)) {
            fgets(linija, LEN, file);
            if (strstr(linija, kljucnaRec) != NULL) {
                printf("Nadjena kljucna rec u liniji: %d\n", i);
            }
            i++;
        }

        close(pd1[0]);
    }
}
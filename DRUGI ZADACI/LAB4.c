#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define N 20

int main() {
    int pd1[2];
    int pd2[2];
    int niz[N];
    int suma = 0;

    if (pipe(pd1) == -1) {
        printf("Greska prilikom kreiranja prvog datavoda!\n");
        return -1;
    }
    if (pipe(pd2) == -1) {
        printf("Greska prilikom kreiranja drugog datavoda!\n");
        return -1;
    }

    int pid1, pid2;

    if ((pid1 = fork()) != 0) {
        close(pd1[0]);
        close(pd2[0]);
        close(pd2[1]);

        for (int i = 0; i < N; i++) {
            niz[i] = (time(NULL)*rand()) % 1001;
            suma += niz[i];
            write(pd1[1], &niz[i], sizeof(int));
        }
        printf("Poslati brojevi\n");
        write(pd1[1], &suma, sizeof(int));
        printf("Poslata suma: %d\n", suma);
        close(pd1[1]);

        wait(NULL);
    } else if ((pid2 = fork()) != 0) {
        printf("Kreiran drugi proces\n");
        close(pd1[1]);
        close(pd2[0]);

        int brojevi[N];
        int primljena_suma;

        for (int i = 0; i < N; i++) {
            read(pd1[0], &brojevi[i], sizeof(int));
        }
        read(pd1[0], &primljena_suma, sizeof(int));
        printf("Primljena suma: %d\n", primljena_suma);
        close(pd1[0]);

        if (primljena_suma % 2 == 0) {
            FILE* f = fopen("BROJEVI.txt", "w");
            if (!f) {
                perror("Greska pri otvaranju datoteke");
                exit(1);
            }
            for (int i = 0; i < N; i++) {
                printf("Upisujem u fajl broj: %d\n", brojevi[i]);
                fprintf(f, "%d\n", brojevi[i]);
            }
            fclose(f);
        } else {
            for (int i = 0; i < N; i++) {
                write(pd2[1], &brojevi[i], sizeof(int));
                printf("Šaljem broj: %d\n", brojevi[i]);
            }
        }
        close(pd2[1]);
        wait(NULL); 
    } else {
        printf("Kreiran treci proces\n");
        close(pd1[0]);
        close(pd1[1]);
        close(pd2[1]);

        int broj;

        while (read(pd2[0], &broj, sizeof(int)) > 0) {
            printf("%d ", broj);
        }

        printf("\n");
        close(pd2[0]);
        exit(0);
}

    return 0;
}

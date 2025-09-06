#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 20

int main() {
    int pd1[2], pd2[2];
    int pid;

    if (pipe(pd1) == -1) {
        printf("Greska pri PIPE 1!\n");
        exit(1);
    }
    if (pipe(pd2) == -1) {
        printf("Greska pri PIPE 2!\n");
        exit(1);
    }

    pipe(pd1);
    pipe(pd2);

    if (pid = fork() != 0) {
        close(pd1[0]);
        close(pd2[1]);

        int brojac = 0;

        int niz[N];

        for (int i = 0; i < N; i++) {
            niz[i] = ((time(NULL) * rand()) % 101) + 200;
            write(pd1[1], &niz[i], sizeof(int));
            printf("Saljem broj %d\n", niz[i]);
            if (niz[i] % 3 == 0) {
                brojac++;
            }
        }
        printf("Broj brojeva u trenutnom nizu koji su deljivi sa 3: %d\n", brojac);

        for(int i = 0; i < brojac; i++) {
            read(pd2[0], &niz[i], sizeof(int));
            printf("Stampam broj: %d\n", niz[i]);
        }

        close(pd1[1]);

        wait(NULL);
    }
    else {
        close(pd1[1]);
        close(pd2[0]);
        int brojevi[N];

        for (int i = 0; i < N; i++) {
            read(pd1[0], &brojevi[i], sizeof(int));
            if (brojevi[i] % 3 == 0) {
                write(pd2[1], &brojevi[i], sizeof(int));
            }
        }

        close(pd1[0]);
        close(pd2[1]);

        exit(0);

    }

    return 0;
}
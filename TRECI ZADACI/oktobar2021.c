#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 255
#define K 10101
#define P 0666
#define C IPC_CREAT
#define R IPC_RMID

struct buffer {
    long tip;
    int indeks;
    char ime[MAX];
    char prezime[MAX];
};

int main() {
    int msgid = msgget(K, P | C);
    struct buffer buff;

    if (fork() != 0) {
        while(1) {
            printf("Unesi indeks, ime, prezime; 0 za kraj\n");
            scanf("%d", &buff.indeks);

            if (buff.indeks == 0) {
                buff.tip = 1;
                msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 0);
                printf("Primljena 0, izlazim...\n");
                break;
            }

            buff.tip = 1;
            scanf("%s", buff.ime);
            scanf("%s", buff.prezime);

            msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 0);
            printf("poslat student\n");
        }

        wait(NULL);

        msgctl(msgid, R, NULL);
    } else {
        struct buffer studenti[MAX];
        int i = 0;
        while(1) {
            msgrcv(msgid, &buff, sizeof(buff)-sizeof(long), 1, 0);

            if (buff.indeks == 0) {
                break;
            }

            studenti[i++] = buff;
        }

        for (int j = 0; j < i-1; j++) {
            for (int k = j + 1; k < i; k++) {
                if (studenti[j].indeks > studenti[k].indeks) {
                    struct buffer temp;
                    temp = studenti[j];
                    studenti[j] = studenti[k];
                    studenti[k] = temp;
                }
            }
        }

        printf("Sortirani spisak\n");
        for (int j = 0; j < i; j++) {
            printf("%d. %d %s %s\n", j+1, studenti[j].indeks, studenti[j].ime, studenti[j].prezime);
        }
    }

    return 0;
}
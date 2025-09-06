#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct poruka {
    long tip;
    int broj;
};

int main() {
    int msgid = msgget(10101, 0666 | IPC_CREAT);
    struct poruka buff;

    int iteracija = 0;

    if (fork() != 0) {
        while(1) {
            if (iteracija > 9)
                break;
            
            printf("ITERACIJA: %d\n", iteracija+1);
            printf("Unesite visecifreni broj: ");
            scanf("%d", &buff.broj);

            if (buff.broj > 9) {
                buff.tip = 1;
                msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 0);
                printf("\nPoslat broj: %d, u iteraciji: %d\n", buff.broj, iteracija+1);
                iteracija++;
            } else {
                buff.tip = 2;
                printf("Niste uneli dobar broj, ponavlja se iteracija: %d\n", iteracija+1);
            }

            buff.broj = 0;
            msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 1);

            wait(NULL);
            msgctl(msgid, IPC_RMID, NULL);
        }
    } else {
        while(1) {
            msgrcv(msgid, &buff, sizeof(buff)-sizeof(long), 1, 0);

            if (buff.broj == 0)
                break;

            int prvobitniBroj = buff.broj;

            int zbir = 0;
            while(buff.broj != 0) {
                zbir += buff.broj % 10;
                buff.broj /= 10;
            }

            printf("Zbir cifara broja: %d je: %d\n", prvobitniBroj, zbir);
        }
    }

    return 0;
}
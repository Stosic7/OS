#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct poruka {
    long tip;
    int broj;
};

int main() {
    int msgid = msgget(10101, 0666);
    struct poruka buff;

    buff.tip = 1;
    int suma = 0;

    while(suma <= 1000) {
        scanf("%d", &buff.broj);

        suma += buff.broj;
        printf("Trenutna suma: %d\n", suma);

        if (suma > 1000)
            printf("prekoracen limit, izlazim...\n");

        msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 0);
    }

    return 0;
}
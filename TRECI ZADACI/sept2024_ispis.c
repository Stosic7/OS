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

    int suma = 0;

    while (suma <= 1000) {
        msgrcv(msgid, &buff, sizeof(buff)-sizeof(long), 1, 0);

        printf("Primljen broj: %d\n", buff.broj);
        suma += buff.broj;
    }

    return 0;
}
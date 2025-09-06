#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX 256

struct poruka {
    long tip;
    int cifra;
    char tekst[MAX];
};

int main() {
    int msgid;
    struct poruka buff;

    msgid = msgget(10101, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    int pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        // Child 1: broji karaktere
        while (1) {
            if (msgrcv(msgid, &buff, sizeof(buff) - sizeof(long), 1, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            if (buff.cifra == 0)
                break;

            int brKaraktera = strlen(buff.tekst);
            printf("\nChild 1 - Broj karaktera: %d\n", brKaraktera);
            fflush(stdout);
        }
        exit(0);
    }

    pid2 = fork();
    if (pid2 == 0) {
        // Child 2: pretvara u velika slova
        while (1) {
            if (msgrcv(msgid, &buff, sizeof(buff) - sizeof(long), 2, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            if (buff.cifra == 0)
                break;

            for (int i = 0; i < strlen(buff.tekst); i++) {
                buff.tekst[i] = toupper(buff.tekst[i]);
            }

            printf("\nChild 2 - Sva velika slova: %s\n", buff.tekst);
            fflush(stdout);
        }
        exit(0);
    }

    // Parent
    while (1) {
        printf("Unesite cifru 1 ili 2 (ili bilo šta drugo za kraj): ");
        fflush(stdout);

        if (scanf("%d", &buff.cifra) != 1 || (buff.cifra != 1 && buff.cifra != 2))
            break;

        printf("Unesi tekst: ");
        fflush(stdout);
        scanf("%s", buff.tekst);

        buff.tip = buff.cifra;
        if (msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            break;
        }
    }

    // Pošalji signal za kraj child procesima
    buff.cifra = 0;
    buff.tip = 1;
    msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);
    buff.tip = 2;
    msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);

    // Sačekaj child procese
    wait(NULL);
    wait(NULL);

    // Ukloni message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}

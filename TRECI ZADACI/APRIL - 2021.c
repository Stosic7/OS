#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>

#define MAX 256
#define KEY 10101
#define PERM 0666
#define CREATE IPC_CREAT

struct buffer {
    long tip;
    char tekst[MAX];
    int brLinije;
    int brVelikihSlova;
};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Potrebno je upisati dva argumenta!\n");
        exit(1);
    }

    int msgid = msgget(KEY, PERM | CREATE);
    struct buffer buf;

    if(fork() != 0) {
        buf.brLinije = 0;
        FILE* f = fopen(argv[1], "r");
        if (f == 0) {
            printf("Greska prilikom citanja fajla.\n");
            exit(-1);
        }
        while(!feof(f)) {
            char linija[MAX];
            fgets(linija, MAX, f);

            strcpy(buf.tekst, linija);
            buf.tip = 1;
            buf.brLinije++;
            buf.brVelikihSlova = 0;
            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);

            if (msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 2, 0) != 0)
                printf("Primljena poruka: %s\nPrimljena poruka broj: %d\nBroj velikih slova: %d\n\n\n",buf.tekst, buf.brLinije, buf.brVelikihSlova);
        }
        fclose(f);
        printf("KRAJ!\n");

        buf.tip = 1;
        strcpy(buf.tekst, "KRAJ");
        msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    } else {
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 1, 0);

            if (strcmp(buf.tekst, "KRAJ")==0)
                break;

            for(int i = 0; i < strlen(buf.tekst); i++) {
                if(isupper(buf.tekst[i]))
                    buf.brVelikihSlova++;
            }

            buf.tip = 2;
            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
        }
    }

    return 0;
}
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

struct buffer {
    long tip;
    int broj;
    char tekst[MAX];

};

int main() {
    int msgid = msgget(KEY, PERM | IPC_CREAT);
    struct buffer buf;

    int pid1 = fork();
    if (pid1 == 0) {
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 1, 0);

            if(strcmp(buf.tekst, "KRAJ")==0)
                break;
            
            int duzinaPoruka = strlen(buf.tekst);
            printf("Duzina poruke je: %d\n", duzinaPoruka);
        }
        exit(-1);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 2, 0);

            if (strcmp(buf.tekst, "KRAJ")==0)
                break;
            
            for(int i = 0; i < strlen(buf.tekst); i++){
                buf.tekst[i] = toupper(buf.tekst[i]);
            }

            printf("%s\n", buf.tekst);
        }
        exit(-1);
    }

    while(1) {
        printf("Unesite 1 ili 2: ");
        scanf("%d", &buf.broj);
        getchar();

        if (buf.broj != 1 && buf.broj != 2) {
            printf("\nPrimljen je broj razlicit od 1 ili 2, izlazim...\n");
            break;
        }

        printf("Unesite tekst: ");
        fgets(buf.tekst, MAX, stdin);
        buf.tekst[strcspn(buf.tekst, "\n")] = 0;

        if (buf.broj == 1) {
            buf.tip = 1;
            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
            printf("\nPoslata poruka processu 2.\n");
        } else {
            buf.tip = 2;
            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
            printf("\nPoslata poruka processu 3.\n");
        }
    }

    strcpy(buf.tekst, "KRAJ");

    buf.tip = 1;
    msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
    buf.tip = 2;
    msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);

    wait(NULL);
    wait(NULL);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
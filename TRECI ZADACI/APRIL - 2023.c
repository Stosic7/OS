#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>

#define KEY 10101
#define PERM 0666
#define CREATE IPC_CREAT

struct buffer {
    long tip;
    int broj;
};

int main() {
    int msgid = msgget(KEY, PERM | CREATE);
    struct buffer buf;
    int brPoslatihBrojeva = 0;

    if (fork() != 0) {
        while(1) {
            printf("Unesite broj u intervalu [0,127]: ");
            scanf("%d", &buf.broj);
            buf.tip = 1;

            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
            brPoslatihBrojeva++;

            if(buf.broj == 0) {
                printf("Primljena 0, izlazim...\n");
                break;
            }
        }

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    } else {
        int brPrimljenih = 0;
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 1, 0);

            if(buf.broj == 0){
                printf("Broj primljenih brojeva: %d\n", brPrimljenih);
                break;
            }

            printf("Broj %d je ASCII karakter: %c\n", buf.broj, (char)buf.broj);
            brPrimljenih++;
        }
    }

    return 0;
}

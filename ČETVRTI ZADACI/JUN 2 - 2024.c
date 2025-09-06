#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define LEN 50

struct mymsgbuf{
    long tip;
    int broj;
};

int main() {
    int pid;
    struct mymsgbuf buff;
    FILE* f;

    int msgid = msgget(10101, 0666 | IPC_CREAT);

    f = fopen("jun22024.txt", "w");
    if (f == NULL)
        exit(1);

    pid = fork();

    if(pid == 0) {
        while(1) {
            msgrcv(msgid, &buff, sizeof(buff)-sizeof(long), 1, 0);

            if(buff.broj == 0) {
                printf("izlazim...");
                break;
            }
            
            int zbir = (buff.broj % 10) + ((buff.broj / 10) % 10) + (buff.broj / 100);
            fprintf(f, "Primljen broj: %d, Zbir cifara: %d\n", buff.broj, zbir);
        }
    } else {
        while(1) {
            printf("Unesi broj: ");
            scanf("%d", &buff.broj);
            printf("\n");
            
            if (buff.broj == 0) {
                buff.tip = 1;
                msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 0);
                break;
            }

            if(buff.broj > 99 && buff.broj < 1000) {
                buff.tip = 1;
                msgsnd(msgid, &buff, sizeof(buff)-sizeof(long), 0);
                printf("Poslat broj.\n");
            } else {
                buff.tip = 2;
            }
        }

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    }
    
    fclose(f);
    return 0;
}
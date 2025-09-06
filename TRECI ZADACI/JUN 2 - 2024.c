#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>


struct mojBroj{
    long tip;
    int broj;
};

int main() {
    int msgid;
    struct mojBroj buf;

    FILE* f;
    f = fopen("jun22024.txt", "w");
    if (f == NULL)
        exit(-1);

    msgid = msgget(10101, 0666 | IPC_CREAT);
    int pid = fork();

    if (pid == 0) {
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 1, 0);

            if (buf.broj == 0) {
                break;
            }

            int zbir = (buf.broj % 10) + ((buf.broj / 10) % 10) + (buf.broj / 100);

            fprintf(f, "Upisujem broj: %d, zbir cifara: %d\n", buf.broj, zbir);
        }

    } else {
        while(1) {
            printf("Unesi broj: ");
            scanf("%d", &buf.broj);

            if (buf.broj == 0) {
                buf.tip = 1;
                msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
                break;
            }

            if (buf.broj > 99 && buf.broj < 1000) {
                buf.tip = 1;
                printf("Broj se salje...\n");
                msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
                printf("broj poslat.\n");
            } else {
                buf.tip = 2;
            }
        }

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    }
    return 0;
}
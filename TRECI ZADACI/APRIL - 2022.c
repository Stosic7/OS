#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 10101
#define PERM 0666

struct buffer {
    long tip;
    int randomBroj;
};

int main() {
    int msgid = msgget(KEY, PERM | IPC_CREAT);
    struct buffer buf;

    int brParnih = 0;
    int brNeparnih = 0;

    if (fork() != 0) {
        int N = rand() % 20;
        printf("Random broj N je: %d\n", N);

        for (int i = 0; i < N; i++) {
            printf("ITERACIJA: %d\n", i+1);

            buf.randomBroj = rand() % 1001;
            if (buf.randomBroj == -1)
                continue;
            
            printf("Generisan random broj: %d\n", buf.randomBroj);
            buf.tip = 1;

            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
        }

        buf.tip = 1;
        buf.randomBroj = -1;
        msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    } else {
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 1, 0);
            printf("Primljen broj: %d\n", buf.randomBroj);

            if (buf.randomBroj == -1)
                break;
            
            if (buf.randomBroj % 2 == 0)
                brParnih++;
            else
                brNeparnih++;
        }

        printf("Broj parnih: %d\nBroj neparnih: %d\n", brParnih, brNeparnih);
    }

    return 0;
}
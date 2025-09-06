#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX 256

struct buffer {
    long tip;
    int ID;
};

int main() {
    int msgid;
    struct buffer buff;
    FILE* f;
    
    msgid = msgget(10101, 0666 | IPC_CREAT);

    int A1 = fork();
    if (A1 == 0) {
        int maxID = INT_MIN;
        int minID = INT_MAX;
        while (1) {
            msgrcv(msgid, &buff, sizeof(buff)-sizeof(long), 1, 0);
            if (buff.ID == -1) break;

            if (buff.ID > maxID) maxID = buff.ID;
            if (buff.ID < minID) minID = buff.ID;
        }
        printf("A1 - min: %d, max: %d\n", minID, maxID);
        exit(0);
    }


    int A2 = fork();
    if (A2 == 0) {
        int maxID = INT_MIN;
        int minID = INT_MAX;
        while (1) {
            msgrcv(msgid, &buff, sizeof(buff)-sizeof(long), 2, 0);
            if (buff.ID == -1) break;

            if (buff.ID > maxID) maxID = buff.ID;
            if (buff.ID < minID) minID = buff.ID;
        }
        printf("A2 - min: %d, max: %d\n", minID, maxID);
        exit(0);
    }

    // parent process
    f = fopen("prijava.txt", "r");
    if (f == NULL) {
        printf("Greska sa otvaranjem fajla");
        exit(-1);
    }

    while(!feof(f)) {
        char linija[MAX];
        fgets(linija, MAX, f);

        char idString[MAX];
        int j = 0;

        for (int i = 0; i < strlen(linija); i++) {
            if (isdigit(linija[i])) {
                idString[j++] = linija[i];
            }
        }

        idString[j] = '\0';
        buff.ID = atoi(idString);

        if (buff.ID <= 18000) {
            buff.tip = 1;
            msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);
        } else {
            buff.tip = 2;
            msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);
        }
    }

    fclose(f);

    buff.ID = -1;
    buff.tip = 1;
    msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);
    buff.tip = 2;
    msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);

    wait(NULL);
    wait(NULL);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
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

struct poruka {
    long tip;
    char tekst[MAX];
};

int main() {
    int msgid = msgget(10101, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    struct poruka buff;

    int pid1 = fork();
    if (pid1 == 0) {
        // CHILD 1 – toupper
        FILE* f1 = fopen("sredjeno.txt", "a");
        if (!f1) {
            perror("fopen f1");
            exit(1);
        }

        while (1) {
            if (msgrcv(msgid, &buff, sizeof(buff) - sizeof(long), 1, 0) == -1) {
                perror("msgrcv tip 1");
                break;
            }

            if (strcmp(buff.tekst, "KRAJ") == 0)
                break;

            for (int i = 0; i < strlen(buff.tekst); i++) {
                buff.tekst[i] = toupper(buff.tekst[i]);
            }

            fprintf(f1, "%s\n", buff.tekst);
            fflush(f1);
        }

        fprintf(f1, "\n");
        fclose(f1);
        exit(0);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        // CHILD 2 – tolower
        FILE* f2 = fopen("sredjeno.txt", "a");
        if (!f2) {
            perror("fopen f2");
            exit(1);
        }

        while (1) {
            if (msgrcv(msgid, &buff, sizeof(buff) - sizeof(long), 2, 0) == -1) {
                perror("msgrcv tip 2");
                break;
            }

            if (strcmp(buff.tekst, "KRAJ") == 0)
                break;

            for (int i = 0; i < strlen(buff.tekst); i++) {
                buff.tekst[i] = tolower(buff.tekst[i]);
            }

            fprintf(f2, "%s\n", buff.tekst);
            fflush(f2);
        }

        fprintf(f2, "\n");
        fclose(f2);
        exit(0);
    }

    // PARENT – čita iz poruka.txt i šalje
    FILE* f = fopen("poruka.txt", "r");
    if (!f) {
        perror("fopen poruka.txt");
        exit(1);
    }

    printf("Otvaraj fajl...\n");

    int brReda = 0;
    while (!feof(f)) {
        char linija[MAX];

        if (fgets(linija, MAX, f) == NULL)
            break;

        linija[strcspn(linija, "\n")] = '\0';
        if (strlen(linija) == 0)
            continue;

        strcpy(buff.tekst, linija);
        buff.tip = (brReda % 2 == 0) ? 1 : 2;
        brReda++;

        if (msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
    }

    // Šalji kraj obe grane
    strcpy(buff.tekst, "KRAJ");
    buff.tip = 1;
    msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);
    buff.tip = 2;
    msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0);

    fclose(f);
    wait(NULL);
    wait(NULL);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}

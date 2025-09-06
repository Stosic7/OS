#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX 256

struct poruka {
    long tip;
    char tekst[MAX];
};

int main() {
    struct poruka buf;
    int msgid = msgget(10101, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    FILE* f1 = fopen("prva.txt", "r");
    if (f1 == NULL) {
        perror("fopen prva.txt");
        exit(1);
    }

    FILE* f2 = fopen("druga.txt", "r");
    if (f2 == NULL) {
        perror("fopen druga.txt");
        exit(1);
    }

    int pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        // CHILD 1: Čita iz prva.txt i šalje poruke
        while (!feof(f1)) {
            char temp[MAX];
            if (fgets(temp, MAX, f1) != NULL) {
                buf.tip = 1;
                strcpy(buf.tekst, temp);
                printf("Saljem tekst iz f1.\n");
                msgsnd(msgid, &buf, sizeof(buf) - sizeof(long), 0);
            }
        }
        // Šalji kraj
        buf.tip = 1;
        strcpy(buf.tekst, "KRAJ");
        msgsnd(msgid, &buf, sizeof(buf) - sizeof(long), 0);

        fclose(f1);
        exit(0);
    }

    int pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0) {
        // CHILD 2: Čita iz druga.txt i šalje poruke
        while (!feof(f2)) {
            char temp[MAX];
            if (fgets(temp, MAX, f2) != NULL) {
                buf.tip = 2;
                strcpy(buf.tekst, temp);
                printf("Saljem tekst iz f2.\n");
                msgsnd(msgid, &buf, sizeof(buf) - sizeof(long), 0);
            }
        }
        // Šalji kraj
        buf.tip = 2;
        strcpy(buf.tekst, "KRAJ");
        msgsnd(msgid, &buf, sizeof(buf) - sizeof(long), 0);

        fclose(f2);
        exit(0);
    }

    // PARENT proces
    FILE* f3 = fopen("prva_mod.txt", "w");
    if (f3 == NULL) {
        perror("fopen prva_mod.txt");
        exit(1);
    }

    FILE* f4 = fopen("druga_mod.txt", "w");
    if (f4 == NULL) {
        perror("fopen druga_mod.txt");
        exit(1);
    }

    // Primi poruke tip 1
    while (1) {
        if (msgrcv(msgid, &buf, sizeof(buf) - sizeof(long), 1, 0) > 0) {
            if (strcmp(buf.tekst, "KRAJ") == 0)
                break;

            printf("Primljena poruka iz f1\n");

            for (int i = 0; i < strlen(buf.tekst); i++) {
                buf.tekst[i] = toupper((unsigned char)buf.tekst[i]);
            }

            fputs(buf.tekst, f3);
        }
    }

    // Primi poruke tip 2
    while (1) {
        if (msgrcv(msgid, &buf, sizeof(buf) - sizeof(long), 2, 0) > 0) {
            if (strcmp(buf.tekst, "KRAJ") == 0)
                break;

            printf("Primljena poruka iz f2\n");

            for (int i = 0; i < strlen(buf.tekst); i++) {
                buf.tekst[i] = tolower((unsigned char)buf.tekst[i]);
            }

            fputs(buf.tekst, f4);
        }
    }

    // Čekaj decu
    wait(NULL);
    wait(NULL);

    // Očisti message queue
    msgctl(msgid, IPC_RMID, NULL);

    fclose(f3);
    fclose(f4);

    return 0;
}

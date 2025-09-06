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

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("nevalidan broj argumenata\n");
        return 1;
    }

    int msgid = msgget(10101, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    struct poruka buff;

    int pid1 = fork();
    if (pid1 == 0) {
        // CHILD 1 – tip 1
        FILE* f1 = fopen("prvakol.txt", "w");
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

            fprintf(f1, "Primljeni tekst: %s\n", buff.tekst);
        }

        fclose(f1);
        exit(0);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        // CHILD 2 – tip 2
        FILE* f2 = fopen("drugakol.txt", "w");
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

            fprintf(f2, "Primljeni tekst: %s\n", buff.tekst);
        }

        fclose(f2);
        exit(0);
    }

    // PARENT
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen input");
        return 1;
    }

    printf("fajl krenuo sa citanjem\n");

    while (!feof(f)) {
        char linija[MAX];

        if (fgets(linija, MAX, f) == NULL)
            break;

        linija[strcspn(linija, "\n")] = '\0';  // skini \n

        if (strlen(linija) == 0)
            continue; // preskoči prazne linije

        buff.tip = (linija[0] == 'Z' || linija[0] == 'z') ? 1 : 2;
        strcpy(buff.tekst, linija);

        if (msgsnd(msgid, &buff, sizeof(buff) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            break;
        }
    }

    // KRAJ poruke za oba procesa
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

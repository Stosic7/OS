#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX 256
#define KEY 10101
#define PERM 0666

struct buffer {
    long tip;
    int indeks;
    char imePrezime[MAX];
};

int main() {
    int msgid = msgget(KEY, PERM | IPC_CREAT);
    struct buffer buf;

    if (fork() != 0) {
        printf("Unesi podatke o studentu. Unesi KRAJ nakon unosenja poslednjeg studenta.\n");
        while(1) {
            char krajString[MAX];
            printf("Kraj?\n");
            scanf("%s", krajString);

            if(strcmp(krajString, "KRAJ") == 0)
                break;

            printf("\nUnesi broj indeksa: \n");
            scanf("%d", &buf.indeks);

            printf("\nUnesi ime i prezime studenta: \n");
            getchar();
            fgets(buf.imePrezime, MAX, stdin);

            buf.imePrezime[strcspn(buf.imePrezime, "\n")] = 0;

            buf.tip = 1;
            msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0);
        }

        buf.tip = 1;
        buf.indeks = 0;
        strcpy(buf.imePrezime, "KRAJ");

        msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 1);

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    } else {
        struct buffer studenti[100];
        int j = 0;
        while(1) {
            msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 1, 0);

            if (buf.indeks == 0 && strcmp(buf.imePrezime, "KRAJ") == 0)
                break;
            
            printf("Primljen student: %s, sa brojem indeksa: %d\n", buf.imePrezime, buf.indeks);

            studenti[j] = buf;
            j++;
        }

        for (int i = 0; i < j-1; i++) {
            for(int k = i + 1; k < j; k++) {
                if (studenti[i].indeks > studenti[k].indeks) {
                    struct buffer temp;
                    temp = studenti[i];
                    studenti[i] = studenti[k];
                    studenti[k] = temp;
                }
            }
        }

        for (int i = 0; i < j; i++) {
            printf("%d %s\n", studenti[i].indeks, studenti[i].imePrezime);
        }
    }

    return 0;
}
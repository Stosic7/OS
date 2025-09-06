#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define LEN 80

int main() {
    int pd1[2], pd2[2];
    int pid;

    if (pipe(pd1) == -1) {
        printf("Greska pri PIPE 1\n");
        exit(1);
    }

    if (pipe(pd2) == -1) {
        printf("Greska pri PIPE 2\n");
        exit(1);
    }

    if ((pid = fork()) != 0) {
        close(pd1[0]);
        close(pd2[1]);

        int suma;
        char recenica[LEN];
        printf("Unesi recenicu: ");
        gets(recenica);
        recenica[strcspn(recenica, "\n")] = '\0'; // Uklanja newline

        int brojacReci = 0;
        char reci[LEN][LEN];

        char* token = strtok(recenica, " ");
        while (token != NULL) {
            strcpy(reci[brojacReci++], token);
            token = strtok(NULL, " ");
        }

        write(pd1[1], &brojacReci, sizeof(int));
        printf("Recenica je podeljena na %d reci i one su poslate procesu detetu.\n", brojacReci);

        for (int i = 0; i < brojacReci; i++) {
            write(pd1[1], &reci[i], sizeof(reci[i]));
            printf("Saljem rec: %s\n", reci[i]);
            read(pd2[0], &suma, sizeof(int));
            printf("Reč: %s, Suma brojeva: %d\n", reci[i], suma);
        }

        close(pd1[1]);
        close(pd2[0]);

        wait(NULL);
    } else {
        
        close(pd1[1]);
        close(pd2[0]);

        int brojacReci = 0;
        char reci[LEN][LEN];

        read(pd1[0], &brojacReci, sizeof(int));
        printf("Proces dete je procitao %d reci\n", brojacReci);

        for (int i = 0; i < brojacReci; i++) {
            int suma = 0;
            read(pd1[0], reci[i], sizeof(reci[i]));
            for (int j = 0; reci[i][j] != '\0'; j++) {
                if (isdigit(reci[i][j])) {
                    suma += reci[i][j] - '0';
                }
            }
            write(pd2[1], &suma, sizeof(int));
        }

        close(pd1[0]);
        close(pd2[1]);

        exit(0);
    }

    return 0;
}

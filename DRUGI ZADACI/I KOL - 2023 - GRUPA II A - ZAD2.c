#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define LEN 1024

int main() {
    int pd1[2];
    int pd2[2];
    int pid;

    // Create the pipes
    if (pipe(pd1) == -1) {
        printf("Error creating pipe 1\n");
        exit(1);
    }
    if (pipe(pd2) == -1) {
        printf("Error creating pipe 2\n");
        exit(1);
    }

    if ((pid = fork()) != 0) {
        
        close(pd1[0]);
        close(pd2[1]);

        char putanja[LEN];
        char kljucnaRec[LEN];
        int brTrLinije;

        printf("Enter the file path: ");
        fgets(putanja, LEN, stdin);
        putanja[strcspn(putanja, "\n")] = '\0';

        printf("Enter the keyword: ");
        fgets(kljucnaRec, LEN, stdin);
        kljucnaRec[strcspn(kljucnaRec, "\n")] = '\0';

        write(pd1[1], putanja, strlen(putanja) + 1);
        write(pd1[1], kljucnaRec, strlen(kljucnaRec) + 1);

        printf("Parent process sent file path: %s\n", putanja);
        printf("Parent process sent keyword: %s\n", kljucnaRec);

        while (read(pd2[0], &brTrLinije, sizeof(int)) > 0) {
            printf("Keyword found at line: %d\n", brTrLinije);
        }

        close(pd1[1]); 
        close(pd2[0]); 

        wait(NULL);
    } else {
        
        close(pd1[1]); 
        close(pd2[0]);

        char putanja[LEN];
        char kljucnaRec[LEN];

        read(pd1[0], putanja, LEN);
        read(pd1[0], kljucnaRec, LEN);

        printf("Child process received file path: %s\n", putanja);
        printf("Child process received keyword: %s\n", kljucnaRec);

        FILE *f = fopen(putanja, "r");
        if (f == NULL) {
            perror("Error opening file");
            exit(1);
        }

        char linija[LEN];
        int brTrLinije = 1;

        while (!feof(f)) {
            if (fgets(linija, LEN, f) != NULL && strstr(linija, kljucnaRec) != NULL) {
                write(pd2[1], &brTrLinije, sizeof(int));
            }
            brTrLinije++;
        }

        fclose(f);
        close(pd1[0]);
        close(pd2[1]);

        exit(0);
    }

    return 0;
}

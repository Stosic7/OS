#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define MAX 100

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    int pd1[2];
    int pid;

    if (pipe(pd1) == -1) {
        perror("Pipe error");
        exit(1);
    }

    if ((pid = fork()) != 0) { 
        close(pd1[0]);
        int i = 0;
        char linija[MAX];
        FILE* f = fopen(argv[1], "r");

        if (!f) {
            perror("Error opening file");
            exit(1);
        }

        printf("File opened successfully\n");

        while (!feof(f)) { 
            fgets(linija, MAX, f);
            i++;
            if (strlen(linija) > 10) {
                write(pd1[1], linija, MAX);
                printf("Sent line: %s\n", linija);
                write(pd1[1], &i, sizeof(int));
            }
        }

        fclose(f);
        close(pd1[1]);
        wait(NULL); 
    } else { 
        close(pd1[1]);
        char linija[MAX];
        int i;

        while (read(pd1[0], &linija, MAX) && (read(pd1[0], &i, sizeof(int))) > 0) {
            if (i % 2 == 0) {
                printf("Chil process stampa: %s, Line Number: %d\n", linija, i);
            }
        }
        
        close(pd1[0]);
        exit(0);
    }

    return 0;
}

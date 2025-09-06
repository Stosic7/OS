#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define LEN 1000

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Neodogovarajuci broj argumenata.\n");
        exit(1);
    }

    int pd[2];
    int pid;

    pipe(pd);

    if (pid = fork() != 0) {

        close(pd[0]);

        FILE* f = fopen(argv[1], "r");
        if (f == 0)
            exit(1);

        char buff[LEN];

        while(!feof(f)) {
            for (int i = 0; i < LEN; i++) {
                write(pd[1], &buff[i], sizeof(char));
            }
        }

        fclose(f);

        close(pd[1]);

        wait(NULL);
    }
    else {
        close(pd[1]);

        FILE* f = fopen(argv[2], "w");
        if (f == 0)
            exit(1);
        
        char procitaniBajt;
        int brBajta = 1;
        while(read(pd[0], &procitaniBajt, sizeof(char))) {
            int XOR = procitaniBajt ^ brBajta;
            fprintf(f, "%d%c", XOR, procitaniBajt);
            ++brBajta;
        }

        fclose(f);
        close(pd[0]);
    }

    return 0;

}
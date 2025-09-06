#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int pd1[2], pd2[2];
    int pid1, pid2;

    if (pipe(pd1) == -1 || pipe(pd2) == -1) {
        printf("Greska pri pipe-ovanju\n");
        exit(1);
    }

    if (pid1 = fork() != 0) {
        close(pd1[0]);
        close(pd2[0]);
        close(pd2[1]);
        int n;
        for(int i = 0; i < 10; i++) {
            n = (time(NULL) * rand()) % 100;
            write(pd1[1], &n, sizeof(int));
            printf("%d. (pd1[1]) Saljem broj: %d\n",i+1, n);
        }
        close(pd1[1]);

        wait(NULL);
    }
    else if (pid2 = fork() != 0) {
        close(pd1[1]);
        close(pd2[0]);
        int n;
        for(int i = 0; i < 10; i++) {
            read(pd1[0], &n, sizeof(int));
            printf("%d. (pd1[0]) Primljen broj: %d\n", i+1, n);
            if (n % 2 == 0) {
                printf("Broj %d je deljiv.\n", n);
                n = n/2;
                printf("Modifikovan broj za prvo dete proces; %d\n", n);
                write(pd2[1], &n, sizeof(int));
                printf("%d. (pd2[1]) Saljem broj: %d\n", i+1, n);
            }
            write(pd2[1], &n, sizeof(int));
            printf("%d. (pd2[1]) Saljem broj: %d\n", i+1, n);
        }
        
        close(pd1[0]);
        close(pd2[1]);
    } else {
        close(pd1[1]);
        close(pd1[0]);
        close(pd2[1]);
        int n;

        int fd = open("redirekcija.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (fd == -1) {
        perror("Greska pri otvaranju fajla");
        exit(1);
    }
        printf("Otovren fajl.\n");

        dup2(fd, 1);

        for(int i = 0; i < 10; i++) {
            read(pd2[0], &n, sizeof(int));
            if (n > 10) {
                printf("%d\n", n);
            }
        }

        close(pd2[0]);
        exit(0);
    }
    return 0;
}
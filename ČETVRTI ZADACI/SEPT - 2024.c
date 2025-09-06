#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define KEY 10001
#define KEY2 10002
#define MEM_KEY 10002
#define N 5

int main() {
    union semun arg;
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};
    int* shmem;
    int memid;
    memid = shmget(MEM_KEY, N*sizeof(int), IPC_CREAT | 0666);

    int sem = semget(KEY, 1, IPC_CREAT | 0666);
    int sem2 = semget(KEY2, 1, IPC_CREAT | 0666);

    arg.val = 1; semctl(sem,  0, SETVAL, arg);
    arg.val = 0; semctl(sem2, 0, SETVAL, arg);

    if(fork() != 0) {
        shmem = shmat(memid, NULL, 0);
        if (shmem == NULL) {
            printf("Greska prilikom mapiranja");
            exit(1);
        }

        while(1) {
            semop(sem, &lock, 1); //sem je sada 0, izvrsava se
            if (shmem[1] == 0) {
                printf("Nadjena 0, izlazim...");
                break;
            }
            int broj;
            for (int i = 0; i < N; i++) {
                printf("Unesi broj #%d: \n", i +1);
                scanf("%d", &broj);
                shmem[i] = broj;
            }

            printf("Unlockujem sem2\n");
            semop(sem2, &unlock, 1); // kazi da krene sem2

            semop(sem, &lock, 1); // cekaj dok opet ne budes u mogucnosti da radis

            printf("Suma je: %d\n", shmem[1]);

            semop(sem, &unlock,1);
        }

        wait(NULL);
        shmdt(shmem);
        semctl(sem, 0, IPC_RMID, 0);
        semctl(sem2, 0, IPC_RMID, 0);
    } else {
        printf("Ulazim u child process\n");
        shmem = shmat(memid, NULL, 0);
        if (shmem == NULL) {
            printf("Greska prilikom mapiranja");
            exit(1);
        }
        while (1) {
            semop(sem2, &lock, 1);
            if (shmem[1] == 0) break;
            int suma = 0;
            for (int i = 0; i < N; i++) suma += shmem[i];
            shmem[1] = suma;
            semop(sem, &unlock, 1);
        }

    }

    return 0;
}
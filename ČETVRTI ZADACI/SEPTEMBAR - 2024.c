#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define MEM_KEY 1122
#define PROC_A_KEY 10101
#define PROC_B_KEY 10102
#define N 5

int main() {
    int procaid, procbid;
    union semun opts;

    struct sembuf sem_lock = {0, -1, 0};
    struct sembuf sem_unlock = {0, 1, 0};

    procaid = semget((key_t)PROC_A_KEY, 1, 0666 | IPC_CREAT);
    procbid = semget((key_t)PROC_B_KEY, 1, 0666 | IPC_CREAT);

    opts.val = 1;
    semctl(procaid, 0, SETVAL, opts);
    opts.val = 0;
    semctl(procbid, 0, SETVAL, opts);

    int shm_id = shmget(MEM_KEY, N * sizeof(int), IPC_CREAT | 0666);
    int* shm_ptr = shmat(shm_id, NULL, 0);

    if (fork() != 0) {
        //roditelj
        while(1) {
            semop(procaid, &sem_lock, 1);

            if (shm_ptr[0] == 0)
                break;

            int brojevi[N];
            int broj;
            int suma[1];
            for(int i = 0; i < N; i++) {
                printf("Unesi broj #%d: ", i+1);
                scanf("%d", &broj);
                brojevi[i] = broj;
            }

            for (int i = 0; i < N; i++)
                shm_ptr[i] = brojevi[i];

            semop(procbid, &sem_unlock, 1); // kreni proc b

        }
        wait(NULL);

        shmctl(shm_id, IPC_RMID, NULL);
        semctl(procaid, 0, IPC_RMID);
        semctl(procbid, 0, IPC_RMID);
    } else {
        //dete
        while(1) {
            semop(procbid, &sem_lock, 1); // obrni jedno proc b, zato je i napisano 1.

            int suma = 0;
            
            for (int i = 0; i < N; i++) {
                suma += shm_ptr[i];
            }

            printf("Suma brojeva: %d\n", suma);

            // upisi sumu u deljivu memoriju
            shm_ptr[0] = suma;

            semop(procaid, &sem_unlock, 1);

            if (suma == 0)
                break;
        }
    }
    
    return 0;
}
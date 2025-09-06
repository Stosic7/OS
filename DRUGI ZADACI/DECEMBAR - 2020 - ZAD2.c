#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 20

sem_t sem_even, sem_odd;
FILE *file;

void* write_even(void* arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&sem_even);
        fprintf(file, "%d\n", i * 2);
        fflush(file);
        sem_post(&sem_odd);
    }
    return NULL;
}

void* write_odd(void* arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&sem_odd);
        fprintf(file, "%d\n", i * 2 + 1);
        fflush(file);
        sem_post(&sem_even);
    }
    return NULL;
}

int main() {
    pthread_t even_thread, odd_thread;

    sem_init(&sem_even, 0, 1);
    sem_init(&sem_odd, 0, 0);

    file = fopen("izlaz.txt", "w");
    if (file == NULL) {
        perror("Greska pri otvaranju datoteke");
        return 1;
    }

    pthread_create(&even_thread, NULL, write_even, NULL);
    pthread_create(&odd_thread, NULL, write_odd, NULL);

    pthread_join(even_thread, NULL);
    pthread_join(odd_thread, NULL);

    fclose(file);
    sem_destroy(&sem_even);
    sem_destroy(&sem_odd);

    printf("Upis zavrsen. Pogledajte izlaz.txt za rezultate.\n");

    return 0;
}

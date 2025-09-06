#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10

int niz[N];
int zbirNiz;

sem_t upis;

void* randomUpis(void* arg) {
    while(1) {
        sem_wait(&upis);

        if(zbirNiz > 1000)
        break;
    
        for(int i = 0; i < N; i++)
            niz[i] = rand() % 200;
        
        sem_post(&upis);
    }
    return NULL;
}

int main() {
    srand(time(NULL) * rand());

    sem_init(&upis, 0, 0);

    pthread_t t;

    pthread_create(&t, NULL, randomUpis, NULL);

    do {
        sem_wait(&upis);

        zbirNiz = 0;
        for (int i = 0; i < N; i++)
            zbirNiz += niz[i];
        
        if (zbirNiz > 800)
            printf("Pusi ga, manje od 800\n");
        else
            printf("Svaka cast, vise od 800\n");
        sem_post(&upis);
    } while(zbirNiz <= 1000);

    printf("\nZbir niza je veci od 1000, kraj\n");

    pthread_join(t, NULL);

    sem_destroy(&upis);

    return 0;
}
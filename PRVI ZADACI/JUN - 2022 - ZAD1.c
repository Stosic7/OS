#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 20

int niz[N];

sem_t sortiraj, cekaj;

void* threadFunc(void* arg) {
    int n = (*(int*)arg);
    printf("\nKrekiran thread\n");
    
    sem_wait(&sortiraj);
    for(int i = 0; i < n-1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (niz[j] < niz[i]) {
                int temp = niz[j];
                niz[j] = niz[i];
                niz[i] = temp;
            }
        }
    }
    sem_post(&cekaj);
}

int main() {
    FILE* f = fopen("ulaz.txt", "r");
    int i = 0;

    pthread_t t;

    sem_init(&cekaj, 0, 0); 
    sem_init(&sortiraj, 0, 0);

    char linija[5];
    while(!feof(f)) {
        sem_wait(&cekaj);
        fgets(linija, 5, f);
        niz[i] = atoi(linija);
        printf("Procitao sam broj: %d iz fajla\n", niz[i]);
        i++;
    }
    pthread_create(&t, NULL, threadFunc, (void*)&i);
    pthread_join(t, NULL);
    sem_post(&sortiraj);

    printf("Sortiran niz: \n");
    for (int j = 0; j < i; j++) {
        printf("%d ", niz[j]);
    }

    sem_destroy(&cekaj);
    sem_destroy(&sortiraj);

    return 0;
}
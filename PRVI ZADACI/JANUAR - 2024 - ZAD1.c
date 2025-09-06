#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define MAX 100
int niz[MAX];
int suma;

sem_t cekaj, racunaj;

void* threadFunc(void* arg) {
    sem_wait(&racunaj);
    suma = 0;
    int n = *((int*)arg);

    for (int i = 0; i < n; i++) {
        printf("\nU thread je stigo broj: %d, ", niz[i]);
        suma += niz[i];
        printf("Trenutna suma: %d\n", suma);
    }
    sem_post(&cekaj);

    return NULL;
}

int main() {

    pthread_t t;

    sem_init(&cekaj, 0, 1);
    sem_init(&racunaj, 0, 0);

    FILE* f = fopen("jan2024.txt", "r");
    if (f == 0) {
        printf("Fajl nije uspesno otvoren\n");
        exit(1);
    }
    printf("Fajl je otvoren\n");

    char linija[MAX];
    char* token;
    int n;
    while(!feof(f)) {
        sem_wait(&cekaj);
        fgets(linija, MAX, f);
        token = strtok(linija, " ");
        int i = 0;
        while(token != NULL) {
            niz[i] = atoi(token);
            token = strtok(NULL, " ");
            i++;
        }
        printf("\n");
        int n = i;
        pthread_create(&t, NULL, threadFunc, (void*)&n);
        printf("Niz ima sledece brojeve: ");
        for (i = 0; i < n; i++) {
            printf("%d ", niz[i]);
        }
        sem_post(&racunaj);
        sem_wait(&cekaj);
        printf("\n");

        printf("Suma niza: %d", suma);
        sem_post(&cekaj);

        pthread_join(t, NULL);
    }

    sem_destroy(&cekaj);
    sem_destroy(&racunaj);

    return 0;

}
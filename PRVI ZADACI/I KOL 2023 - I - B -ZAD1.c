#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

int terminate = FALSE;

sem_t cekaj, stampaj;

void* threadFunc(void* arg) {
    sem_wait(&stampaj);
    int n = *((int*)arg);
    FILE* f = fopen("data.txt", "r");
    if (f == 0) {
        printf("greska pri otvaranju.\n");
        exit(1);
    }
    printf("\nFile otvoren\n");

    char linija[256];

    while(!feof(f)) {
        for(int i = 0; i < n; i++){
            fgets(linija, 256, f);
            printf("%s\n", linija);
        }
        sem_post(&cekaj);
        break;
    }

    return NULL;
}

int main() {

    int N;
    pthread_t t;

    sem_init(&cekaj, 0, 1);
    sem_init(&stampaj, 0, 0);

    while(1) {
        sem_wait(&cekaj);
        printf("Unesi N: ");
        scanf("%d", &N);
        pthread_create(&t, NULL, threadFunc, (void*)&N);
        if (N == 99) {
            terminate = TRUE;
            break;
        }
        sem_post(&stampaj);
    }

    sem_destroy(&cekaj);
    sem_destroy(&stampaj);

    return 0;
}
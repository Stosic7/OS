#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10
#define M 10

int niz[N];
int zbirNiz;

sem_t upis, check;

void* upisiNaParnim(void* arg) {
    for (int i = 0; i < 5; i++) {
        sem_wait(&upis);
        for (int j = 0; j < N; j += 2) {
            niz[j] = rand() % 300;
            printf("PARNI THREAD: U iteraciji %d pisem broj %d na poziciji %d\n", i, niz[j], j);
        }
        sem_post(&check);
    }
    return NULL;
}

void* upisNaNeparnim(void* arg) {
    for (int i = 0; i < 5; i++) {
        sem_wait(&upis);
        for (int j = 1; j < N; j += 2) {
            niz[j] = (rand() % 200) + 300;
            printf("NEPARNI THREAD: U iteraciji %d pisem broj %d na poziciji %d\n", i, niz[j], j);
        }
        sem_post(&check);
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    sem_init(&upis, 0, 2);
    sem_init(&check, 0, 0);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, upisiNaParnim, NULL);
    pthread_create(&t2, NULL, upisNaNeparnim, NULL);

    for (int i = 0; i < 5; i++) {
        printf("%d. ", i + 1);
        sem_wait(&check);
        sem_wait(&check);

        zbirNiz = 0;
        for (int j = 0; j < N; j++) {
            zbirNiz += niz[j];
        }

        if (zbirNiz > 2000) {
            printf("Zbir niza: %d je veci od 2000\n", zbirNiz);
        } else {
            printf("Zbir niza: %d je manji od 2000\n", zbirNiz);
        }

        sem_post(&upis);
        sem_post(&upis);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&upis);
    sem_destroy(&check);

    return 0;
}
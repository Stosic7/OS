#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

int buffer[2];

pthread_mutex_t mutex;
pthread_cond_t condVarFull;
pthread_cond_t condVarEmpty;
int bufferFull = FALSE;
int terminate = FALSE;

void* threadFunc(void* arg) {
    int i;
    int bufferPointer = 0;

    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&mutex);

        while(bufferFull)
            pthread_cond_wait(&condVarEmpty, &mutex);
        
        buffer[bufferPointer] = (rand() % 100) + 100;
        bufferPointer = (bufferPointer + 1) % 2;

        if (bufferPointer == 0) {
            bufferFull = TRUE;
            pthread_cond_signal(&condVarFull);
        }

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t t;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condVarEmpty, NULL);
    pthread_cond_init(&condVarFull, NULL);

    pthread_create(&t, NULL, threadFunc, NULL);

    for (int i = 0; i < 10; i++) {
        printf("%d. ", i+1);
        pthread_mutex_lock(&mutex);

        while(!bufferFull)
            pthread_cond_wait(&condVarFull, &mutex);

        if (buffer[0] == buffer[1]) {
            printf("Brojevi koji su pristigli su: %d i %d, i oni su jednaki.\n", buffer[0], buffer[1]);
        }
        else {
            printf("Brojevi koji su pristigli su: %d i %d, i oni nisu jednaki.\n", buffer[0], buffer[1]);
        }

        bufferFull = FALSE;
        pthread_cond_signal(&condVarEmpty);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condVarEmpty);
    pthread_cond_destroy(&condVarFull);

    return 0;
}
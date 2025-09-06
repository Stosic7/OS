#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

int N = 0;
int buffer;
pthread_mutex_t mutex;
pthread_cond_t condVarFull;
pthread_cond_t condVarWaiting;
int bufferFull = FALSE;
int terminate = FALSE;

void* threadFunc(void* arg) {
    printf("Odbrojavanje pocinje.\n");
    printf("\n");
    sleep(1);
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);
        if (terminate) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        while (bufferFull)
            pthread_cond_wait(&condVarWaiting, &mutex);

        buffer = i;
        printf("Stampam broj: %d\n", i);
        if (buffer == N - 1) {
            bufferFull = TRUE;
            pthread_cond_signal(&condVarFull);
        }

        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t threadID;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condVarFull, NULL);
    pthread_cond_init(&condVarWaiting, NULL);

    while (1) {
        printf("Unesi N: ");
        scanf("%d", &N);
        printf("\n");

        if (N == 999) {
            pthread_mutex_lock(&mutex);
            terminate = TRUE;
            pthread_cond_signal(&condVarWaiting);
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_create(&threadID, NULL, threadFunc, NULL);

        pthread_mutex_lock(&mutex);

        while (!bufferFull)
            pthread_cond_wait(&condVarFull, &mutex);

        bufferFull = FALSE;
        pthread_cond_signal(&condVarWaiting);
        pthread_mutex_unlock(&mutex);

        printf("Buffer je resetovan.\n");

        pthread_join(threadID, NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condVarFull);
    pthread_cond_destroy(&condVarWaiting);

    return 0;
}

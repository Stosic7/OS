#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

int buffer;
int N;

pthread_mutex_t mutex;
pthread_cond_t condVarFull;
pthread_cond_t condVarEmpty;

int bufferFull = FALSE;
int terminate = FALSE;

void* threadFunc(void* arg) {
    printf("\nSTAMPAM BROJEVE\n");
    sleep(1);
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);

        if (terminate) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        while(bufferFull)
            pthread_cond_wait(&condVarEmpty, &mutex);
        
        buffer = i;
        printf("Odstampan je broj %d\n", buffer);

        if (buffer == N-1) {
            bufferFull = TRUE;
            pthread_cond_signal(&condVarFull);
        }

        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
    return NULL;
}

int main() {
    pthread_t t;

    int A;
    
    while (1) {
        printf("\nUnesite broj (uniste 99 za kraj): ");
        scanf("%d", &A);
        if (A == 99) {
            pthread_mutex_lock(&mutex);
            terminate = TRUE;
            pthread_cond_signal(&condVarEmpty);
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&condVarEmpty, NULL);
        pthread_cond_init(&condVarFull, NULL);

        pthread_create(&t, NULL, threadFunc, NULL);

        N = A;
        while (!bufferFull)
            pthread_cond_wait(&condVarFull, &mutex);
        
        
        printf("\nBrojevi su odstampani.\n");
        
        bufferFull = FALSE;
        pthread_cond_signal(&condVarEmpty);
        pthread_mutex_unlock(&mutex);

        pthread_join(t, NULL);
    }


    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condVarEmpty);
    pthread_cond_destroy(&condVarFull);

    return 0;
}
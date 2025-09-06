#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

int buffer[10];

pthread_mutex_t mutex;
pthread_cond_t condVarEmpty;
pthread_cond_t condVarFull;
int bufferFull = FALSE;
int terminate = FALSE;

void* threadFunc(void* arg) {
    printf("Krekiran thread\n");
    int i = 0;

    while(1) {
        pthread_mutex_lock(&mutex);

        if(terminate) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        while(bufferFull) {
            pthread_cond_wait(&condVarEmpty, &mutex);
            i=0;
        }
        int random = 0;
        random = (rand())%100 +100;
        buffer[i] = random;
        printf("buffer[%d]: %d\n", i, random);
        i++;
        if (i == 10) {
            bufferFull = TRUE;
            pthread_cond_signal(&condVarFull);
            i = 0;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t t;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condVarEmpty, NULL);
    pthread_cond_init(&condVarFull, NULL);

    pthread_create(&t, NULL, threadFunc, NULL);

    while(1) {
        pthread_mutex_lock(&mutex);
        int zbir = 0;

        while(!bufferFull)
            pthread_cond_wait(&condVarFull, &mutex);

        for(int i = 0; i < 10; i++) {
            zbir += buffer[i];
        }

        printf("Zbir je: %d\n", zbir);

        if(zbir > 1000) {
            printf("Zbir je veci od 1000: %d\n", zbir);
            terminate = TRUE;
            pthread_cond_signal(&condVarEmpty);
            pthread_mutex_unlock(&mutex);
            break;
        }

        if(zbir >= 800) {
            printf("Zbir veci od 800: %d\n", zbir);
            sleep(5);
        }
        else if (zbir < 800) {
            printf("Zbir manji od 800: %d\n", zbir);
            sleep(5);
        }

        
        bufferFull = FALSE;
        pthread_cond_signal(&condVarEmpty);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condVarFull);
    pthread_cond_destroy(&condVarEmpty);

    return 0;


}
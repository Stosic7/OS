#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>

#define MAX_LEN 100

char recenica[MAX_LEN];
char reci[MAX_LEN][MAX_LEN];
int brojReci = 0;
sem_t sortSem, formatSem;

void* sortirajReci(void* arg) {
    sem_wait(&sortSem);

    for (int i = 0; i < brojReci - 1; i++) {
        for (int j = i + 1; j < brojReci; j++) {
            if (strcmp(reci[i], reci[j]) > 0) {
                char temp[MAX_LEN];
                strcpy(temp, reci[i]);
                strcpy(reci[i], reci[j]);
                strcpy(reci[j], temp);
            }
        }
    }

    sem_post(&formatSem);
    return NULL;
}

void* formatirajReci(void* arg) {
    sem_wait(&formatSem);

    recenica[0] = '\0';

    for (int i = 0; i < brojReci; i++) {
        strcat(recenica, reci[i]);
        if (i < brojReci - 1) {
            strcat(recenica, " ");
        }
    }

    if (strlen(recenica) > 0) {
        recenica[0] = toupper(recenica[0]);
        strcat(recenica, ".");
    }

    printf("Sortirana i formatirana recenica: %s\n", recenica);

    return NULL;
}

int main() {
    sem_init(&sortSem, 0, 0);
    sem_init(&formatSem, 0, 0);

    pthread_t sortThread, formatThread;

    pthread_create(&sortThread, NULL, sortirajReci, NULL);
    pthread_create(&formatThread, NULL, formatirajReci, NULL);

    while (1) {
        printf("Unesite recenicu (bez velikog pocetnog slova i bez tacke na kraju, ili 'KRAJ' za izlaz):\n");
        fgets(recenica, MAX_LEN, stdin);

        size_t len = strlen(recenica);
        if (len > 0 && recenica[len - 1] == '\n') {
            recenica[len - 1] = '\0';
        }

        if (strcmp(recenica, "KRAJ") == 0) {
            break;
        }

        // Podela recenice na reci
        brojReci = 0; // Reset broja reci
        char* token = strtok(recenica, " ");
        while (token != NULL) {
            strcpy(reci[brojReci++], token);
            token = strtok(NULL, " ");
        }

        sem_post(&sortSem);

        pthread_join(sortThread, NULL);
        pthread_join(formatThread, NULL);
    }

    sem_destroy(&sortSem);
    sem_destroy(&formatSem);

    return 0;
}

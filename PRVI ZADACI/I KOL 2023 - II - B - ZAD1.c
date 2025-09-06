#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define MAX 256
#define MAX_LINES 100

FILE* f;
char parneLinije[MAX_LINES][MAX];
char neparneLinije[MAX_LINES][MAX];

sem_t parni, neparni;
int parniIndeks = 0, neparniIndeks = 0;

void* parniF(void* arg) {
    FILE* parniFile = fopen("parni.txt", "w");
    if (!parniFile) {
        perror("Greska pri otvaranju parni.txt");
        pthread_exit(NULL);
    }

    for (int i = 0; i < parniIndeks; i++) {
        sem_wait(&parni);
        fprintf(parniFile, "%s", parneLinije[i]);
        printf("Upisano u parni.txt: %s", parneLinije[i]);
        sem_post(&neparni);
    }

    fclose(parniFile);
    return NULL;
}

void* neparniF(void* arg) {
    FILE* neparniFile = fopen("neparni.txt", "w");
    if (!neparniFile) {
        perror("Greska pri otvaranju neparni.txt");
        pthread_exit(NULL);
    }

    for (int i = 0; i < neparniIndeks; i++) {
        sem_wait(&neparni);
        fprintf(neparniFile, "%s", neparneLinije[i]);
        printf("Upisano u neparni.txt: %s", neparneLinije[i]);
        sem_post(&parni);
    }

    fclose(neparniFile);
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Greska za parametre\n");
        return 0;
    }

    pthread_t t1, t2;

    sem_init(&parni, 0, 0);
    sem_init(&neparni, 0, 0);

    f = fopen(argv[1], "r");
    if (!f) {
        perror("Greska pri otvaranju ulaznog fajla");
        exit(1);
    }

    printf("Otvoren fajl: %s\n", argv[1]);

    char linija[MAX];
    int brLinije = 0;

    while (fgets(linija, MAX, f)) {
        if (brLinije % 2 == 0) {
            strcpy(parneLinije[parniIndeks], linija);
            printf("Sadrzaj parne linije [%d]: %s", brLinije, parneLinije[parniIndeks]);
            parniIndeks++;
        } else {
            strcpy(neparneLinije[neparniIndeks], linija);
            printf("Sadrzaj neparne linije [%d]: %s", brLinije, neparneLinije[neparniIndeks]);
            neparniIndeks++;
        }
        brLinije++;
    }
    sem_post(&parni);

    fclose(f);

    pthread_create(&t1, NULL, parniF, NULL);
    pthread_create(&t2, NULL, neparniF, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&parni);
    sem_destroy(&neparni);

    return 0;
}

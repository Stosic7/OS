#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define length 80

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Neodgovarajuci broj parametara\n");
        exit(1);
    }

    int pd1[2];
    int pd2[2];

    pipe(pd1);
    pipe(pd2);

    int pid1, pid2;

    if (pid1 = fork() != 0) {
        close(pd1[0]);
        close(pd2[0]);
        close(pd2[1]);

        for (int i = 1; i < argc; i++)
            write(pd1[1], argv[i], length);
        
        close(pd1[1]);

        wait(NULL);

    }
    else if(pid2 = fork() != 0) {
        close(pd1[1]);
        close(pd2[0]);

        char imeFajla[length];
        read(pd1[0], imeFajla, length);

        int brKljucnihReci = argc-2;
        char kljucneReci[brKljucnihReci][length];
        int i = 0;
        while(read(pd1[0], kljucneReci[i++], length));

        FILE* f = fopen(imeFajla, "r");
        if (f == 0)
            exit(1);
        
        char linija[200];
        int brTrLinije = 1;
        while (!feof(f)) {
            fgets(linija, 200, f);
            for(int i = 0; i < brKljucnihReci; i++) {
                if (strstr(linija, kljucneReci[i]) != NULL)
                    write(pd2[1], &brTrLinije, sizeof(int));
            }
            ++brTrLinije;
        }

        fclose(f);

        close(pd1[0]);
        close(pd2[1]);
        
        wait(NULL);
    }
    else {
        close(pd2[1]);
        close(pd1[0]);
        close(pd1[1]);

        int procitaniBroj;
        int suma = 0;
        while(read(pd2[0], &procitaniBroj, sizeof(int))) {
            printf("Pronalazak u liniji: %d\n", procitaniBroj);
            suma += procitaniBroj;
        }
        printf("Suma procitanih linija: %d\n", suma);

        close(pd2[0]);
    }

    return 0;
}
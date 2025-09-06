#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

// argv[1] - ime ulazne datoteke
// argv[2] - broj bajtova (N)
// argv[3] - ime izlazne datoteke
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <N> <output_file>\n", argv[0]);
        exit(1);
    }

    int N = atoi(argv[2]);
    if (N <= 0 || N > BUFFER_SIZE) {
        fprintf(stderr, "Invalid N value. It must be > 0 and <= %d.\n", BUFFER_SIZE);
        exit(1);
    }

    // Kreiranje pipe-a
    int pd[2];
    if (pipe(pd) == -1) {
        perror("Pipe error");
        exit(1);
    }

    // Kreiranje child procesa
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork error");
        exit(1);
    }

    if (pid > 0) {
        // Roditeljski proces
        close(pd[0]); // Zatvaramo kraj za čitanje pipe-a

        FILE *input_fd = fopen(argv[1], "rb");
        if (input_fd == NULL) {
            perror("Error opening input file");
            close(pd[1]);
            exit(1);
        }

        char buffer[BUFFER_SIZE];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, N, input_fd)) > 0) {
            if (write(pd[1], buffer, bytesRead) == -1) {
                perror("Error writing to pipe");
                fclose(input_fd);
                close(pd[1]);
                exit(1);
            }
        }

        fclose(input_fd); // Zatvaramo ulaznu datoteku
        close(pd[1]);     // Zatvaramo kraj za pisanje pipe-a
        wait(NULL);       // Čekamo da child proces završi
    } else {
        // Child proces
        close(pd[1]); // Zatvaramo kraj za pisanje pipe-a

        FILE *output_fd = fopen(argv[3], "wb");
        if (output_fd == NULL) {
            perror("Error opening output file");
            close(pd[0]);
            exit(1);
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(pd[0], buffer, N)) > 0) {
            if (fwrite(buffer, 1, bytesRead, output_fd) != (size_t)bytesRead) {
                perror("Error writing to output file");
                fclose(output_fd);
                close(pd[0]);
                exit(1);
            }
        }

        fclose(output_fd); // Zatvaramo izlaznu datoteku
        close(pd[0]);      // Zatvaramo kraj za čitanje pipe-a
        exit(0);
    }

    return 0;
}

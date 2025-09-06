#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

int main() {
    int msgid = msgget(10101, 0666 | IPC_CREAT);

    int pid1 = fork();

    if (pid1 == 0) {
        execl("./unos", "unos", NULL);
        perror("execl unos");
        exit(1);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        execl("./ispis", "ispis", NULL);
        perror("execl ispis");
        exit(1);
    }

    wait(NULL);
    wait(NULL);

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
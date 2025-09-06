#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#define MAX 256

static char  g_names[MAX][PATH_MAX];
static off_t g_sizes[MAX];

void processdir(char* path, int* no) {
    DIR* dp;
    struct dirent* dirp;
    struct stat statbuf;
    char tmp[PATH_MAX];

    if ((dp = opendir(path)) == NULL) {
        printf("Greska prilikom otvaranja foldera: %s\n", path);
        return;
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        snprintf(tmp, sizeof(tmp), "%s/%s", path, dirp->d_name);

        if (stat(tmp, &statbuf) == -1) {
            continue;
        }

        if (S_ISREG(statbuf.st_mode) && statbuf.st_size > 100 * 1024 && strcmp(dirp->d_name, ".DS_Store") != 0) {
            if (*no < MAX) {
                int pos = *no;
                while (pos > 0 && g_sizes[pos - 1] > statbuf.st_size) {
                    g_sizes[pos] = g_sizes[pos - 1];
                    strcpy(g_names[pos], g_names[pos - 1]);
                    pos--;
                }
                g_sizes[pos] = statbuf.st_size;
                strncpy(g_names[pos], dirp->d_name, PATH_MAX - 1);
                g_names[pos][PATH_MAX - 1] = '\0';
                (*no)++;
            }
        }

        if (S_ISDIR(statbuf.st_mode)) {
            processdir(tmp, no);
        }
    }

    closedir(dp);
}


int main(int argc, char** argv) {
    if (argc != 2) { printf("Koriscenje: %s <apsolutna_putanja>\n", argv[0]); return 1; }
    int n = 0;
    processdir(argv[1], &n);
    for (int i = 0; i < n; i++) {
        printf("%s\n", g_names[i]);
    }
    return 0;
}


#include <stdlib.h>
#include "utils.h"

void debug(char *msg) {
    if (DEBUG) write(STDERR_FILENO, msg, strlen(msg));
}

void print(char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

void printc(char msg) {
    write(STDOUT_FILENO, msg, sizeof(char));
}

char *readFileDescriptor(int fd) {
    char mychar;
    char * string;
    int index = 0, length;

    string = malloc(sizeof(char));
    while (1) {
        length = (int) read(fd, &mychar, sizeof(char));
        string[index] = mychar;
        if (mychar == '\n' || mychar == '\0' || !length) {
            string[index] = '\0';
            return string;
        }
        index++;
        string = realloc(string, sizeof(string) * (index + 1));
    }
}




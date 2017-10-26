#include "utils.h"

void debug(char *msg) {
    if (DEBUG) write(STDERR_FILENO, msg, strlen(msg));
}

void print(char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

void println(char *msg) {
    char aux[LENGTH];
    sprintf(aux, "%s\n", msg);
    write(STDOUT_FILENO, aux, strlen(aux));
}

void printc(char msg) {
    write(STDOUT_FILENO, &msg, sizeof(char));
}

char *readFileDescriptor(int fd) {
    char mychar;
    char *string;
    int index = 0, length;

    string = NULL;

    while (1) {
        length = (int) read(fd, &mychar, sizeof(char));
        if (mychar == '\n' || mychar == '\0' || !length) {
            if(string != NULL)
                string[index] = '\0';
            return string;
        }
        string = realloc(string, sizeof(string) * (index + 1));
        string[index] = mychar;
        index++;
    }
}

/*char *readFileDescriptor(int fd) {
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
}*/




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

void printi(int num) {
	char aux[15];
	sprintf(aux, "%d", num);
	write(STDOUT_FILENO, aux, strlen(aux));
}

void printc(char msg) {
	write(STDOUT_FILENO, &msg, sizeof(char));
}

char *readFileDescriptor(int fd) {
	char mychar = '\0';
	int index = 0;
	char *string;
	int length;

	string = NULL;
	while (1) {
		length = (int) read(fd, &mychar, sizeof(char));

		if (mychar == '\n' || mychar == '\0' || !length)
			return string;

		string = realloc(string, sizeof(char) * (index + 2));
		string[index] = mychar;
		string[index + 1] = '\0';
		index++;
	}
}


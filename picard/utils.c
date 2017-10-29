#include "utils.h"


void debug(char *msg) {
	if (DEBUG) write(STDERR_FILENO, msg, strlen(msg));
}

void print(char *msg) {
	write(STDOUT_FILENO, msg, strlen(msg));
}

void printc(char mychar) {
	write(STDOUT_FILENO, &mychar, 1);
}

void printi(int myint) {
	char aux[12];
	sprintf(aux, "%d", myint);
	write(STDOUT_FILENO, aux, strlen(aux));
}

char *getWord(int *index, const char *string) {
	int i;
	char *word;

	for (; string[*index] == ' ' || string[*index] == '\t'; (*index)++);

	word = malloc(sizeof(char));

	if (string[*index] == '\0') {
		word[0] = '\0';
		return word;
	}

	for (i = 0; string[*index]; (*index)++) {
		word[i] = string[*index];
		if (string[*index] == ' ' || string[*index] == '\t')
			break;
		i++;
		word = realloc(word, sizeof(char) * (i + 1));
	}

	word[i] = '\0';

	return word;
}

char endOfWord(int index, const char *string) {
	for (; string[index]; index++)
		if (string[index] != ' ' && string[index] != '\t' && string[index])
			return 0;
	return 1;
}

char checkNumber(const char *word) {
	int i;
	if (word[0] == '\0') return 0;
	for (i = 0; word[i]; i++)
		if (word[i] < '0' || word[i] > '9')
			return 0;
	return 1;
}

char *readFileDescriptor(int fd) {
	char mychar = '\0';
	int index = 0;
	char *string;

	string = NULL;
	while (1) {
		read(fd, &mychar, sizeof(char));
		if (mychar == '\n' || mychar == '\0') {
			if (string != NULL)
				string[fd ? index - 1 : index] = '\0';
			return string;
		}
		string = realloc(string, sizeof(string) * (index + 1));
		string[index] = mychar;
		index++;
	}
}

void myItoa(int num, char *buff) {
	int i, j;
	char aux;

	for (i = 0; num > 0; i++) {
		buff[i] = (char) (num % 10 + '0');
		num /= 10;
	}
	buff[i] = '\0';
	for (j = 0; j < i / 2; j++) {
		aux = buff[j];
		buff[j] = buff[i - j - 1];
		buff[i - j - 1] = aux;
	}
}


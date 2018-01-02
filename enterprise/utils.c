#include "utils.h"

/**
 * Funció que escriu el missatge que rep com a paràmetre si DEBUG val diferent a 0.
 * S'utilitza per depurar.
 *
 * @param msg 	Missatge a mostrar
 */
void debug(char *msg) {
	if (DEBUG) write(STDERR_FILENO, msg, strlen(msg));
}

/**
 * Funció per printar per pantalla una cadena de caràcters.
 *
 * @param msg 	Missatge a mostrar
 */
void print(char *msg) {
	write(STDOUT_FILENO, msg, strlen(msg));
}

/**
 * Funció per llegir caràcter a caràcter d'un file descriptor i a memòria dinàmica fins a trobar un salt de línia
 * o un '\0'.
 *
 * @param fd	File descriptor
 * @return		Punter a memòria dinàmica amb el text llegit
 */
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

		string = realloc(string, (size_t) index + 2);
		string[index] = mychar;
		string[index + 1] = '\0';
		index++;
	}
}

/**
 * Funció per passar un número a ASCII.
 * @param num 	Número a obtenir en ASCII
 * @param buff 	Cadena amb el resultat
 */
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

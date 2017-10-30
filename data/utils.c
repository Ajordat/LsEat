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
	char mychar;
	char *string;
	int index = 0, length;

	string = NULL;

	while (1) {
		length = (int) read(fd, &mychar, sizeof(char));
		if (mychar == '\n' || mychar == '\0' || !length) {
			if (string != NULL)
				string[!length ? index : index - 1] = '\0';
			return string;
		}
		string = realloc(string, sizeof(char) * (index + 2));
		string[index] = mychar;
		index++;
	}
}




#include "utils.h"

/**
 * Funció que escriu el missatge que rep com a paràmetre si DEBUG val diferent a 0.
 * S'utilitza per depurar.
 *
 * @param msg 	Missatge a mostrar
 */
void debug(char *msg) {
	if (DEBUG) {
		write(STDERR_FILENO, COLOR_DEBUG, strlen(COLOR_DEBUG));
		write(STDERR_FILENO, msg, strlen(msg));
		write(STDERR_FILENO, COLOR_RESET, strlen(COLOR_RESET));
	}
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
 * Funció per obtenir una paraula dins d'una cadena. Elimina els espais i els tabulats i obté el següent conjunt de
 * caràcters fins a trobar un espai, tabulat o '\0'.
 * S'utilitza per a ignorar els espais a les comandes introduides per l'usuari i quedar-nos directament amb
 * els arguments que escriu.
 *
 * @param index 	Posició de la cadena a partir de la que començar a buscar
 * @param string 	Cadena a analitzar
 * @return 			Punter a una cadena amb la següent paraula escrita
 */
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
		word = realloc(word, (size_t) i + 1);
	}

	word[i] = '\0';

	return word;
}

/**
 * Funció per indicar si a partir de l'índex indicat no hi ha més paraules.
 * S'utilitza per saber si l'usuari ja no ha introduït més arguments a la CLI.
 *
 * @param index 	Índex a partir del que començar a buscar
 * @param string 	Cadena de caràcters a analitzar
 * @return 			Retorna 1 si no queden més paraules a la cadena. Altrament, 0
 */
char endOfWord(int index, const char *string) {
	for (; string[index]; index++)
		if (string[index] != ' ' && string[index] != '\t' && string[index])
			return 0;
	return 1;
}

/**
 * Funció que comprova que una nombre sencer escrit a una cadena de caràcters és correcte
 * i no hi ha caràcters errònis.
 *
 * @param word 	Cadena amb el nombre sencer
 * @return		Retorna 1 si ha tingut èxit i 0 si l'estructura és incorrecta
 */
char checkNumber(const char *word) {
	int i = 0;

	if (word[0] == '\0') return 0;
	if (word[0] == '-')i++;
	for (; word[i]; i++)
		if (word[i] < '0' || word[i] > '9')
			return 0;
	return 1;
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

	string = NULL;
	while (1) {
		read(fd, &mychar, sizeof(char));
		if (mychar == '\n' || mychar == '\0') {
			if (string != NULL)
				string[fd ? index - 1 : index] = '\0';
			return string;
		}
		string = realloc(string, (size_t) index + 2);
		string[index] = mychar;
		index++;
	}
}

/**
 * Funció per passar un número a ASCII.
 * @param num 	Número a obtenir en ASCII
 * @param buff 	Cadena amb el resultat
 */
int myItoa(int num, char *buff) {
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
	return i;
}

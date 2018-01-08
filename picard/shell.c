#include "shell.h"


void initShell() {
	nLog = 0;
	indexLog = 0;
	tcgetattr(STDIN_FILENO, &old);
}

/**
 * Funció utilitzada per comprovar que no hi ha més arguments dels que hi hauria d'haver a una comanda interna de Picard.
 * Si no n'hi ha més retorna el codi rebut, altrament indica un error.
 *
 * @param index 	Index a partir del que s'ha de comprovar si hi ha més arguments
 * @param command 	Comanda introduïda per l'usuari
 * @param code 		Codi a retornar si el nombre d'arguments és correcte
 * @return 			Retorna el codi rebut com a paràmetre o un error segons la comanda introduïda sigui correcta
 */
char
checkParameters(int index, const char *command, char code) {    //Si es compilés en c99 podria fer que fos inline :(
	return endOfWord(index, command) ? code : ERR_N_PARAMS;
}

/**
 * Funció per extreure la comanda introduïda per l'usuari i convertir-la a una representació més tractable.
 * Extreu la comanda escrita i els seus paràmetres i avalua si es correcte en cada cas. Si no ho és, ho indica
 * amb un codi d'error al tipus Command.
 *
 * @param command 	Comanda introduïda per l'usuari al terminal del Picard
 * @return 			Tipus Command que conté el codi de la comanda a executar i informació extra (si la necessita)
 */
Command substractCommand(char *command) {
	int i = 0;
	size_t length;
	char *word;
	Command cmd;

	cmd.plat = NULL;
	cmd.code = ERR_UNK_CMD;

	if (command == NULL) {
		print("\n");
		return cmd;
	}
	word = getWord(&i, command);

	if (!strcasecmp(CMD_CONNECT, word)) {

		cmd.code = checkParameters(i, command, CODE_CONNECT);

	} else if (!strcasecmp(CMD_SHOW, word)) {

		free(word);
		word = getWord(&i, command);

		if (!strcasecmp(CMD_MENU, word)) {
			cmd.code = checkParameters(i, command, CODE_SHOWMENU);

		} else if (!strcasecmp(CMD_ORDER, word)) {
			cmd.code = checkParameters(i, command, CODE_SHOWORDER);

		} else {
			cmd.code = ERR_UNK_CMD;
			cmd.plat = malloc((size_t) strlen(command) + 1);
			memset(cmd.plat, '\0', (size_t) strlen(command) + 1);
			memcpy(cmd.plat, command, (size_t) strlen(command));
		}

	} else if (!strcasecmp(CMD_REQUEST, word)) {

		free(word);
		word = getWord(&i, command);

		if (checkNumber(word)) {
			cmd.unitats = atoi(word);  // NOLINT
			while (command[i] == ' ' || command[i] == '\t') i++;
			if (endOfWord(i, command)) {
				cmd.code = ERR_N_PARAMS;
				free(cmd.plat);
			} else {
				length = strlen(command) - i + 1;
				cmd.plat = malloc(length);
				memmove(cmd.plat, command + i, length);
				cmd.code = CODE_REQUEST;
			}
		} else
			cmd.code = ERR_N_PARAMS;


	} else if (!strcasecmp(CMD_REMOVE, word)) {

		free(word);
		word = getWord(&i, command);
		if (checkNumber(word)) {
			cmd.unitats = atoi(word); // NOLINT
			while (command[i] == ' ' || command[i] == '\t') i++;
			if (endOfWord(i, command)) {
				cmd.code = ERR_N_PARAMS;
				free(cmd.plat);
			} else {
				length = strlen(command) - i + 1;
				cmd.plat = malloc(length);
				memmove(cmd.plat, command + i, length);
				cmd.code = CODE_REMOVE;
			}
		} else
			cmd.code = ERR_N_PARAMS;


	} else if (!strcasecmp(CMD_PAY, word)) {
		cmd.code = checkParameters(i, command, CODE_PAYMENT);

	} else if (!strcasecmp(CMD_DISCONNECT, word)) {
		cmd.code = checkParameters(i, command, CODE_DISCONNECT);

	} else {
		cmd.plat = malloc(strlen(command) + 1);
		memcpy(cmd.plat, command, strlen(command) + 1);
	}
	free(word);
	return cmd;
}

void appendCommand(Command cmd) {
	char aux[INT_LENGTH];
	size_t length;

	if (history[nLog] != NULL)
		free(history[nLog]);

	switch (cmd.code) {

		case CODE_CONNECT:
			length = strlen(CMD_CONNECT) + 2;
			history[nLog] = malloc(length);
			memcpy(history[nLog], CMD_CONNECT, length);
			break;

		case CODE_SHOWMENU:
			length = strlen(CMD_SHOW) + 1 + strlen(CMD_MENU) + 1;
			history[nLog] = malloc(length);
			memset(history[nLog], '\0', length);
			sprintf(history[nLog], "%s %s", CMD_SHOW, CMD_MENU);
			break;

		case CODE_SHOWORDER:
			length = strlen(CMD_SHOW) + 1 + strlen(CMD_ORDER) + 1;
			history[nLog] = malloc(length);
			memset(history[nLog], '\0', length);
			sprintf(history[nLog], "%s %s", CMD_SHOW, CMD_ORDER);
			break;

		case CODE_REQUEST:
			myItoa(cmd.unitats * (cmd.unitats < 0 ? -1 : 1), aux);
			length = strlen(CMD_REQUEST) + 1 + (!cmd.unitats) + (strlen(aux)) + (cmd.unitats < 0) + 1 +
					 strlen(cmd.plat) + 1;
			history[nLog] = malloc(length);
			memset(history[nLog], '\0', length);    //Aquest length era length-1
			sprintf(history[nLog], "%s %d %s", CMD_REQUEST, cmd.unitats, cmd.plat);
			break;

		case CODE_REMOVE:
			myItoa(cmd.unitats * (cmd.unitats < 0 ? -1 : 1), aux);
			length = strlen(CMD_REQUEST) + 1 + (!cmd.unitats) + (strlen(aux)) + (cmd.unitats < 0) + 1 +
					 strlen(cmd.plat) + 1;
			history[nLog] = malloc(length);
			memset(history[nLog], '\0', length);    //Aquest length era length-2
			sprintf(history[nLog], "%s %d %s", CMD_REMOVE, cmd.unitats, cmd.plat);
			break;

		case CODE_PAYMENT:
			length = strlen(CMD_PAY) + 1;
			history[nLog] = malloc(length);
			memcpy(history[nLog], CMD_PAY, length);
			break;

		case CODE_DISCONNECT:
			length = strlen(CMD_DISCONNECT) + 2;
			history[nLog] = malloc(length);
			memcpy(history[nLog], CMD_DISCONNECT, length);
			break;
			
		default:
			if (cmd.plat == NULL)
				return;
			debug("Plat -> |");
			debug(cmd.plat);
			debug("|\n");
			length = strlen(cmd.plat) + 1;
			history[nLog] = malloc(length);
			memcpy(history[nLog], cmd.plat, length);
			break;
	}
	nLog++;

//	printHistory();
}

char readChar() {
	char mychar;
	struct termios new;

	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	new.c_lflag |= (ISIG);
	tcsetattr(STDIN_FILENO, TCSANOW, &new);

	read(STDIN_FILENO, &mychar, sizeof(char));

	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	return mychar;
}

void printRemainingCommand(char *string, int index, int size) {
	int i;
	for (i = index; i <= size; i++)
		write(STDOUT_FILENO, &string[i], sizeof(char));
	for (; i > index + 1; i--)
		write(STDOUT_FILENO, "\b", sizeof(char));
}

/**
 * Funció per llegir una comanda de l'usuari al terminal del Picard.
 *
 * @return 	Comanda introduïda per l'usuari
 */
char *readCommand() {
	char mychar;
	int index = 0, size = 0;

	history = realloc(history, (nLog + 1) * sizeof(char *));
	history[nLog] = NULL;
	indexLog = nLog;

	while (1) {
		mychar = readChar();
		if (mychar == '\n' || mychar == '\0') {
			if (history[indexLog] != NULL) {
				history[indexLog][size] = '\0';
				for (; index < size; index++)
					write(STDOUT_FILENO, &(history[indexLog][index]), sizeof(char));
				write(STDOUT_FILENO, "\n", sizeof(char));
			}
			return history[indexLog];
		}
		if (mychar == KEY_ARROW1) {
			mychar = readChar();
			if (mychar == KEY_ARROW2) {
				mychar = readChar();
				switch (mychar) {
					case KEY_UP:
						if (indexLog > 0) {
							indexLog--;
							for (; index < size; index++) write(STDOUT_FILENO, " ", sizeof(char));
							for (; size; size--) write(STDOUT_FILENO, "\b \b", (size_t) 3);
							size = index = (int) strlen(history[indexLog]);
							print(history[indexLog]);
						}
						break;
					case KEY_DOWN:
						if (indexLog < nLog) {
							indexLog++;
							for (; index < size; index++) write(STDOUT_FILENO, " ", sizeof(char));
							for (; size; size--) write(STDOUT_FILENO, "\b \b", (size_t) 3);
							if (history[indexLog] != NULL) {
								size = index = (int) strlen(history[indexLog]);
								print(history[indexLog]);
							} else
								size = index = 0;
						}
						break;
					case KEY_RIGHT:
						if (index < size) {
							write(STDOUT_FILENO, &(history[indexLog][index]), sizeof(char));
							index++;
						}
						break;
					case KEY_LEFT:
						if (index > 0) {
							index--;
							write(STDOUT_FILENO, "\b", sizeof(char));
						}
						break;
					default:
						break;
				}
			}
		} else if ((mychar &= 0xFF) == 0177) {    //delete char
			if (index > 0) {
				size--;
				index--;
				if (index == size) {
					history[indexLog] = realloc(history[indexLog], (size_t) size + 1);
					history[indexLog][size] = '\0';
					write(STDOUT_FILENO, "\b \b", (size_t) 3);
				} else {
					memmove(history[indexLog] + index, history[indexLog] + index + 1, (size_t) size - index + 1);
					history[indexLog] = realloc(history[indexLog], (size_t) size + 1);
					write(STDOUT_FILENO, "\b", sizeof(char));
					printRemainingCommand(history[indexLog], index, size);
				}
			}
		} else {    //insert char
			if (size < SHELL_LENGTH) {
				size++;
				write(STDOUT_FILENO, &mychar, sizeof(char));
				history[indexLog] = realloc(history[indexLog], (size_t) size + 1);
				if (index + 1 == size) {
					history[indexLog][size - 1] = mychar;
					history[indexLog][size] = '\0';
				} else {
					memmove(history[indexLog] + index + 1, history[indexLog] + index, (size_t) size - index);
					history[indexLog][index] = mychar;
					printRemainingCommand(history[indexLog], index + 1, size);
				}
				index++;
			}
		}
	}
}


/**
 * Funció per executar la funcionalitat escollida per l'usuari segons la comanda introduïda.
 *
 * @param command 	Comanda introduïda per l'usuari
 * @return 			Un flag indicant si s'ha de tancar l'execució del Picard
 */
char solveCommand(char *command) {
	Command cmd;

	cmd = substractCommand(command);

	if (command != NULL)
		appendCommand(cmd);

	return menuOptions(cmd);
}

/**
 * Funció per mostrar la shell a l'usuari i que aquest pugui interactuar amb el Picard.
 */
void shell() {
	char *command;
	int flag;

	initShell();
	do {
		print(ANSI_COLOR_GREEN);
		print(config.name);
		print("> "ANSI_COLOR_RESET);
		command = readCommand();

		flag = solveCommand(command);

	} while (!flag);
}

void printHistory() {
	int i;
	char aux[LENGTH];

	for (i = 0; i < nLog; i++) {
		sprintf(aux, "%s\n", history[i]);
		debug(aux);
	}
}

void freeHistory(int reason) {

	nLog += reason;

	while (nLog--)
		free(history[nLog]);

	free(history);
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
}


/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint() {
	debug("\nSIGINT REBUT");
	if (sock > 0) {
		disconnect(config.name);
		close(sock);
	}
	freeResources();
	freeHistory(SIGNAL);
	exit(EXIT_SUCCESS);
}
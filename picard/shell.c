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
char checkParameters(int index, const char *command, char code) {
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
	char *word;
	Command cmd;

	if (command == NULL) {
		cmd.code = ERR_UNK_CMD;
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
		} else {
			cmd.code = ERR_UNK_CMD;
			cmd.plat = malloc((strlen(command)+1)* sizeof(char));
			memcpy(cmd.plat, command, (strlen(command)+1)* sizeof(char));
//			memset(cmd.plat, '\0', (strlen(command)+1)* sizeof(char));
		}

	} else if (!strcasecmp(CMD_REQUEST, word)) {

		free(word);
		word = getWord(&i, command);
		if (checkNumber(word)) {
			cmd.unitats = atoi(word);  // NOLINT
			cmd.plat = getWord(&i, command);
			if (cmd.plat[0] == '\0') {
				cmd.code = ERR_N_PARAMS;
				free(cmd.plat);
			} else {
				cmd.code = checkParameters(i, command, CODE_REQUEST);
				if (cmd.code != CODE_REQUEST)
					free(cmd.plat);
			}
		} else {
			cmd.code = ERR_N_PARAMS;
		}

	} else if (!strcasecmp(CMD_REMOVE, word)) {

		free(word);
		word = getWord(&i, command);
		if (checkNumber(word)) {
			cmd.unitats = atoi(word); // NOLINT
			cmd.plat = getWord(&i, command);
			if (cmd.plat[0] == '\0') {
				cmd.code = ERR_N_PARAMS;
				free(cmd.plat);
			} else {
				cmd.code = checkParameters(i, command, CODE_REMOVE);
				if (cmd.code != CODE_REMOVE)
					free(cmd.plat);
			}
		} else {
			cmd.code = ERR_N_PARAMS;
		}

	} else if (!strcasecmp(CMD_PAY, word)) {

		cmd.code = checkParameters(i, command, CODE_PAY);

	} else if (!strcasecmp(CMD_DISCONNECT, word)) {

		cmd.code = checkParameters(i, command, CODE_DISCONNECT);

	} else {
		cmd.plat = command;
		cmd.code = ERR_UNK_CMD;
	}
	free(word);
	return cmd;
}

void appendCommand(Command cmd) {
	char aux[10];

	if (history[nLog] != NULL)
		free(history[nLog]);

	switch (cmd.code) {
		case CODE_CONNECT:
			history[nLog] = malloc((strlen(CMD_CONNECT) + 2) * sizeof(char));
			memset(history[nLog], '\0', (strlen(CMD_CONNECT) + 2) * sizeof(char));
			sprintf(history[nLog], "%s", CMD_CONNECT);
			break;
		case CODE_SHOWMENU:
			history[nLog] = malloc((strlen(CMD_SHOW) + 1 + strlen(CMD_MENU) + 1) * sizeof(char));
			memset(history[nLog], '\0', (strlen(CMD_SHOW) + 1 + strlen(CMD_MENU) + 1) * sizeof(char));
			sprintf(history[nLog], "%s %s", CMD_SHOW, CMD_MENU);
			break;
		case CODE_REQUEST:
			myItoa(cmd.unitats, aux);
			history[nLog] = malloc((strlen(CMD_REQUEST) + 1 + strlen(aux) + 1 + strlen(cmd.plat) + 1) * sizeof(char));
			memset(history[nLog], '\0', (strlen(CMD_REQUEST) + strlen(aux) + strlen(cmd.plat) + 1) * sizeof(char));
			sprintf(history[nLog], "%s %d %s", CMD_REQUEST, cmd.unitats, cmd.plat);
			break;
		case CODE_REMOVE:
			myItoa(cmd.unitats, aux);
			history[nLog] = malloc((strlen(CMD_REMOVE) + 1 + strlen(aux) + 1 + strlen(cmd.plat) + 1) * sizeof(char));
			memset(history[nLog], '\0', (strlen(CMD_REMOVE) + strlen(aux) + strlen(cmd.plat) + 1) * sizeof(char));
			sprintf(history[nLog], "%s %d %s", CMD_REMOVE, cmd.unitats, cmd.plat);
			break;
		case CODE_PAY:
			history[nLog] = malloc((strlen(CMD_PAY) + 1) * sizeof(char));
			memset(history[nLog], '\0', (strlen(CMD_PAY) + 1) * sizeof(char));
			sprintf(history[nLog], "%s", CMD_PAY);
			break;
		case CODE_DISCONNECT:
			history[nLog] = malloc((strlen(CMD_DISCONNECT) + 2) * sizeof(char));
			memset(history[nLog], '\0', (strlen(CMD_DISCONNECT) + 2) * sizeof(char));
			sprintf(history[nLog], "%s", CMD_DISCONNECT);
			break;
		default:
			if (cmd.plat != NULL)
				history[nLog] = cmd.plat;
			break;
	}
	nLog++;

	printHistory();
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
	write(STDOUT_FILENO, " ", sizeof(char));
	for (; i > index; i--)
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

	//TODO: Arreglar si ens introdueixen una comanda incorrecta, hi ha fugues de memòria en aquests casos
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
							for (; size; size--) write(STDOUT_FILENO, "\b \b", sizeof(char) * 3);
							size = index = (int) strlen(history[indexLog]);
							print(history[indexLog]);
						}
						break;
					case KEY_DOWN:
						if (indexLog < nLog) {
							indexLog++;
							for (; index < size; index++) write(STDOUT_FILENO, " ", sizeof(char));
							for (; size; size--) write(STDOUT_FILENO, "\b \b", sizeof(char) * 3);
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
		} else if ((mychar &= 0xFF) == 0177) {
			if (index > 0) {
				size--;
				index--;
				if (index == size) {
					write(STDOUT_FILENO, "\b \b", sizeof(char) * 3);
				} else {
					shiftLeft(history[indexLog], index);
					write(STDOUT_FILENO, "\b", sizeof(char));
					printRemainingCommand(history[indexLog], index, size);
				}
			}
		} else {
			write(STDOUT_FILENO, &mychar, sizeof(char));
			history[indexLog] = realloc(history[indexLog], sizeof(char) * (size + 2));
			index++;
			size++;
			if (index == size) {
				history[indexLog][size - 1] = mychar;
				history[indexLog][size] = '\0';
			} else {
				shiftRight(history[indexLog], index - 1);
				history[indexLog][index - 1] = mychar;
				printRemainingCommand(history[indexLog], index, size);
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
	Command cmd = substractCommand(command);
	static char connected = 0;

	if (command != NULL)
		appendCommand(cmd);

	switch (cmd.code) {
		case CODE_CONNECT:
			debug("Toca connectar\n");
			if (connected) {
				print("Ja estàs connectat a un Enterprise!\n");
				break;
			}
			connected = initConnection();
			break;

		case CODE_SHOWMENU:
			debug("Toca mostrar el menú\n");
			print("[Comanda OK]\n");
			break;

		case CODE_REQUEST:
			debug("Toca demanar\n");
			print("[Comanda OK]\n");
			free(cmd.plat);
			break;

		case CODE_REMOVE:
			debug("Toca eliminar\n");
			print("[Comanda OK]\n");
			free(cmd.plat);
			break;

		case CODE_PAY:
			debug("Toca pagar\n");
			print("[Comanda OK]\n");
			break;

		case CODE_DISCONNECT:
			/* TODO: Si connectem amb èxit amb un Enterprise i aquest es tanca abans que fem un DESCONNECTA,
			 * no alliberem tots els recursos.
			 * */
			debug("Toca desconnectar\n");
			if (connected) {
				disconnect(config.name);
				close(sock);
			} else {
				print("Encara no t'havies connectat!\n");
			}
			freeResources();
			return 1;

		case ERR_UNK_CMD:
			print("Comanda no reconeguda");
			char aux[LENGTH];
			sprintf(aux, " -> [%s]", command);
			debug(aux);
			print("\n");
			break;

		case ERR_N_PARAMS:
			print("Nombre de paràmetres incorrecte\n");
			break;

		default:
			print("Comanda no reconeguda\n");
			break;

	}
	return 0;
}

/**
 * Funció per mostrar la shell a l'usuari i que aquest pugui interactuar amb el Picard.
 */
void shell() {
	char *command;
	int flag;

	initShell();
	do {
		print(config.name);
		print("> ");
		command = readCommand();

		flag = solveCommand(command);

//		free(command);
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

void freeHistory() {
	int i = 0;
	for (; i < nLog; i++) {
		free(history[i]);
	}
	free(history);
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
}
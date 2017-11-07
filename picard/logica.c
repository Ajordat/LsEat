#include "logica.h"

/**
 * Funció per comprovar que el nombre d'arguments sigui correcte.
 *
 * @param argc 	Nombre d'arguments
 * @return 		0 si és correcte. Altrament, 1.
 */
char checkProgramArguments(int argc) {
	char aux[LENGTH];

	if (argc != 2) {
		sprintf(aux, "El format de la crida és incorrecte, ha de ser:\n\tpicard <config_file.dat>\n");
		print(aux);
		return 1;
	}
	return 0;
}

/**
 * Funció per mostrar el missatge de benvinguda.
 */
void welcomeMessage() {
	char aux[LENGTH];

	sprintf(aux, "Benvingut %s\n", config.name);
	print(aux);
	sprintf(aux, "Tens %d euros disponibles\n", config.money);
	print(aux);
	sprintf(aux, "Introdueix comandes...\n");
	print(aux);
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
 * Funció per llegir el fitxer de configuració del Picard.
 *
 * @param filename 	Fitxer de configuració del Picard
 */
void readConfigFile(char *filename) {
	int file;
	char msg[LENGTH], *aux;

	file = open(filename, O_RDONLY);
	if (file <= 0) {
		sprintf(msg, "Error a l'obrir el fitxer %s.\n", filename);
		print(msg);
		exit(EXIT_FAILURE);
	}

	config.name = readFileDescriptor(file);
	aux = readFileDescriptor(file);
	config.money = atoi(aux); // NOLINT
	free(aux);
	config.ip = readFileDescriptor(file);
	aux = readFileDescriptor(file);
	config.port = atoi(aux); // NOLINT
	free(aux);

	close(file);

	sprintf(msg, "|%s - %d - %s - %d|\n", config.name, config.money, config.ip, config.port);
	debug(msg);
}

/**
 * Funció per extreure la comanda introduïda per l'usuari i convertir-la a una representació més tractable.
 * Extreu la comanda escrita i els seus paràmetres i avalua si es correcte en cada cas. Si no ho és, ho indica
 * amb un codi d'error al tipus Command.
 *
 * @param command 	Comanda introduïda per l'usuari al terminal del Picard
 * @return 			Tipus Command que conté el codi de la comanda a executar i informació extra (si la necessita)
 */
Command substractCommand(const char *command) {
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
				cmd.code = CODE_REQUEST;
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
				cmd.code = CODE_REMOVE;
			}
		} else {
			cmd.code = ERR_N_PARAMS;
		}

	} else if (!strcasecmp(CMD_PAY, word)) {

		cmd.code = checkParameters(i, command, CODE_PAY);

	} else if (!strcasecmp(CMD_DISCONNECT, word)) {

		cmd.code = checkParameters(i, command, CODE_DISCONNECT);

	} else {

		cmd.code = ERR_UNK_CMD;
	}
	free(word);
	return cmd;
}

/**
 * Funció per llegir una comanda de l'usuari al terminal del Picard.
 *
 * @return 	Comanda introduïda per l'usuari
 */
char *readCommand() {
	return readFileDescriptor(STDIN_FILENO);
}

/**
 * Funció per extreure la informació de l'Enterprise retornat pel servidor Data. Extreu l'adreça ip i el port
 * del camp de dades.
 *
 * @param frame 	Trama rebuda des de Data. Conté la informació d'un Enterprise
 * @return 			Retorna la informació de connexió a un Enterprise
 */
Socket resolveEnterprise(Frame frame) {
	int i, j;
	char *aux;
	Socket socket;


	for (j = 0; frame.data[j] != '&'; j++);
	j++;

	socket.ip = malloc(sizeof(char));
	for (i = 0; frame.data[j] != '&'; j++) {
		socket.ip[i] = frame.data[j];
		i++;
		socket.ip = realloc(socket.ip, sizeof(char) * (i + 1));
	}
	socket.ip[i] = '\0';
	j++; //saltem &

	aux = malloc(sizeof(char));
	for (i = 0; frame.data[j]; j++) {
		aux[i] = frame.data[j];
		i++;
		aux = realloc(aux, sizeof(char) * (i + 1));
	}
	aux[i] = '\0';
	socket.port = atoi(aux); // NOLINT
	free(aux);

	return socket;
}

/**
 * Funció per connectar-se a un Enterprise a partir de la trama retornada per Data.
 *
 * @param frame 	Trama retornada per Data
 * @return 			0 si s'ha pogut iniciar la connexió amb èxit. Altrament, -1
 */
char tryConnectionEnterprise(Frame frame) {
	Socket socket;

	debugFrame(frame);
	close(sock);

	debug("[STARTING ENTERPRISE CONNECTION]\n");
	if (strcmp(frame.header, HEADER_PIC_DATA_OK)) { // NOLINT
		print(MSG_PIC_DATA_KO);
		free(frame.data);
		return -1;
	}
	print(MSG_PIC_DATA_OK);
	socket = resolveEnterprise(frame);

	if (getSocket(socket.ip, socket.port) < 0) {
		debug("[FAILURE]\n");
		print(MSG_PIC_ENT_KO);
		free(socket.ip);
		free(frame.data);
		return -1;
	}
	print(MSG_PIC_ENT_OK);
	debug("[DONE]\n");
	free(socket.ip);
	free(frame.data);
	connectEnterprise(config.name, config.money);    //HARDCODED
	print(MSG_CONEX_OK);
	return 0;
}

/**
 * Funció per connectar-se amb un Enterprise. Primer es connecta a Data, recull l'Enterprise que li indica aquest
 * i s'hi connecta (si pot).
 *
 * @return 	1 si aconsegueix connectar-se i 0 si hi ha algun error
 */
char initConnection() {
	Frame frame;
	print("Connectant amb LsEat...\n");
	if (getSocket(config.ip, config.port) < 0) {
		print("[No s'ha pogut connectar amb Data]\n");
		return 0;
	}
	frame = establishConnection(config.name);
	return (tryConnectionEnterprise(frame) >= 0);
}

/**
 * Funció per executar la funcionalitat escollida per l'usuari segons la comanda introduïda.
 *
 * @param command 	Comanda introduïda per l'usuari
 * @return 			Un flag indicant si s'ha de tancar l'execució del Picard
 */
char solveCommand(const char *command) {
	Command cmd = substractCommand(command);
	static char connected = 0;

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
			} else {
				print("Encara no t'havies connectat!\n");
			}
			freeResources();
			return 1;

		case ERR_UNK_CMD:
			print("Comanda no reconeguda\n");
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

	do {
		print(config.name);
		print("> ");
		command = readCommand();

		flag = solveCommand(command);

		free(command);
	} while (!flag);

}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {

	print("\nGràcies per fer servir LsEat. Fins la propera.\n");
	free(config.name);
	free(config.ip);
	if (sock > 0)
		close(sock);
}

/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint() {
	debug("\nSIGINT REBUT");
	freeResources();
	exit(EXIT_SUCCESS);
}

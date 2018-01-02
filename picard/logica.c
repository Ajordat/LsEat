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
	sprintf(aux, "Tens %d%s disponibles\n", config.money, MONEDA);
	print(aux);
	sprintf(aux, "Introdueix comandes...\n");
	print(aux);
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
		socket.ip = realloc(socket.ip, (size_t) i + 1);
	}
	socket.ip[i] = '\0';
	j++; //saltem &

	aux = malloc(sizeof(char));
	for (i = 0; frame.data[j]; j++) {
		aux[i] = frame.data[j];
		i++;
		aux = realloc(aux, (size_t) i + 1);
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
		print(MSG_PIC_ENT_KO);
		destroyFrame(&frame);
		return -1;
	}
	print(MSG_PIC_DATA_OK);
	socket = resolveEnterprise(frame);

	sock = createClientSocket(socket.ip, socket.port);
	if (sock < 0) {
		debug("[FAILURE]\n");
		print(MSG_PIC_ENT_KO);
		free(socket.ip);
		destroyFrame(&frame);
		return -1;
	}
	print(MSG_PIC_ENT_OK);
	debug("[DONE]\n");
	free(socket.ip);
	destroyFrame(&frame);
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
	sock = createClientSocket(config.ip, config.port);
	if (sock < 0) {
		print(MSG_PIC_DATA_KO);
		return 0;
	}
	frame = establishConnection(config.name);
	return (tryConnectionEnterprise(frame) >= 0);
}

Dish parseDish(const char *string) {
	Dish dish;
	char aux[LENGTH];
	int i, j;

	dish.name = malloc(sizeof(char));
	for (i = 0; string[i] != '&'; i++) {
		dish.name[i] = string[i];
		dish.name = realloc(dish.name, (size_t) i + 2);
	}
	dish.name[i] = '\0';

	memset(aux, '\0', LENGTH);
	for (j = 0, i++; string[i] != '&'; i++, j++) {
		aux[j] = string[i];
	}
	dish.price = atoi(aux);    // NOLINT

	memset(aux, '\0', LENGTH);
	for (j = 0, i++; string[i]; i++, j++) {
		aux[j] = string[i];
	}
	dish.stock = atoi(aux);    // NOLINT

	return dish;
}

void requestMenu() {
	Frame frame;
	Dish dish;
	char aux[LENGTH];

	frame = createFrame(CODE_SHOWMENU, HEADER_MENU_REQ, NULL);
	writeFrame(frame);

	while (1) {
		destroyFrame(&frame);
		frame = readFrame();    //TODO: CONTROLAR UNA CAIGUDA D'ENTERPRISE

		if (!strcmp(frame.header, HEADER_MENU_END)) break;

		dish = parseDish(frame.data);

		if (dish.stock > 0) {
			sprintf(aux, "%s (x%d) --- %d%s\n", dish.name, dish.stock, dish.price, MONEDA);
			print(aux);
		}
		free(dish.name);
	}
	destroyFrame(&frame);
}

void requestDish(Command cmd) {
	Frame frame;
	char aux[LENGTH];
	char buffer[INT_LENGTH];

	memset(aux, '\0', LENGTH);
	memcpy(aux, cmd.plat, strlen(cmd.plat));

	strcat(aux, "&");

	memset(buffer, '\0', INT_LENGTH);
	myItoa(cmd.unitats, buffer);
	strcat(aux, buffer);

	frame = createFrame(CODE_REQUEST, HEADER_REQ_DISH, aux);
	writeFrame(frame);
	destroyFrame(&frame);

	frame = readFrame();	//TODO: CONTROLAR UNA CAIGUDA D'ENTERPRISE

	if (!strcasecmp(frame.header, HEADER_REQ_DISH_OK)) {
		//TODO: Afegir a llista de plats
		memset(aux, '\0', LENGTH);
		sprintf(aux, "Comanda acceptada!\n");
		print(aux);
	} else if (!strcasecmp(frame.header, HEADER_REQ_DISH_KO)) {
		memset(aux, '\0', LENGTH);
		sprintf(aux, "Comanda rebutjada.\n%s\n", frame.data);
		print(aux);
	}

	destroyFrame(&frame);
	free(cmd.plat);
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {
	print("\nGràcies per fer servir LsEat. Fins la propera.\n");
	free(config.name);
	free(config.ip);
}


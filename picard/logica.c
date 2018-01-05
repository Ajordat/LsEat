#include "logica.h"
#include "shell.h"

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

	print(MSG_CONN_DATA_OK);

	if (strcmp(frame.header, HEADER_PIC_DATA_OK)) { // NOLINT
		print(MSG_CONN_ENT_KO);
		destroyFrame(&frame);
		return -1;
	}

	socket = resolveEnterprise(frame);

	debug("[STARTING ENTERPRISE CONNECTION]\n");
	sock = createClientSocket(socket.ip, socket.port);
	if (sock < 0) {                //TODO: MILLORAR MISSATGE D'ERROR
		debug("[FAILURE]\n");
		print(MSG_CONN_ENT_KO);
		free(socket.ip);
		destroyFrame(&frame);
		return -1;
	}
	print(MSG_CONN_ENT_OK);
	debug("[DONE]\n");
	free(socket.ip);
	destroyFrame(&frame);
	connectEnterprise(config.name, config.money);    //HARDCODED
	print(MSG_CONN_OK);
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

	print(MSG_INIT_CONN);
	sock = createClientSocket(config.ip, config.port);
	if (sock < 0) {
		print(MSG_CONN_DATA_KO);
		return 0;
	}
	frame = establishConnection(config.name);
	if (frame.type == FRAME_NULL) {
		destroyFrame(&frame);
		return 0;
	}
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

char requestMenu() {
	Frame frame;
	Dish dish;
	char aux[LENGTH];
	int resp, header = 0;

	frame = createFrame(CODE_SHOWMENU, HEADER_MENU_REQ, NULL);
	resp = writeFrame(frame);
	destroyFrame(&frame);

	if (!resp)
		return 0;

	while (1) {
		frame = readFrame();

		if (frame.type == FRAME_NULL) {
			return 0;
		}

		if (!strcmp(frame.header, HEADER_MENU_END)) {
			if (!header)
				print(MSG_MENU_KO);
			break;
		}

		dish = parseDish(frame.data);

		if (dish.stock > 0) {
			if (!header) {
				header = 1;
				print(MSG_MENU_OK);
			}
			sprintf(aux, "%s (x%d) --- %d%s\n", dish.name, dish.stock, dish.price, MONEDA);
			print(aux);
		}
		destroyFrame(&frame);
		free(dish.name);
	}
	destroyFrame(&frame);

	return 1;
}

void showOrder() {
	int i, header;
	char aux[LENGTH];

	for (i = 0, header = 0; i < dishes.quantity; i++) {
		if (dishes.menu[i].stock) {
			if (!header) {
				header = 1;
				print(MSG_SHOW_ORDER_OK);
			}
			sprintf(aux, "%s (x%d)\n", dishes.menu[i].name, dishes.menu[i].stock);
			print(aux);
		}
	}

	if (!header)
		print(MSG_SHOW_ORDER_KO);
}

char requestDish(Command cmd) {
	char aux[LENGTH];
	char buffer[INT_LENGTH];

	memset(aux, '\0', LENGTH);
	memcpy(aux, cmd.plat, strlen(cmd.plat));

	strcat(aux, "&");

	memset(buffer, '\0', INT_LENGTH);
	myItoa(cmd.unitats, buffer);
	strcat(aux, buffer);

	return sendRequestDish(cmd, aux, &dishes);
}

char removeDish(Command cmd) {
	char aux[LENGTH];
	char buffer[INT_LENGTH];
	int i;
	char resp;


	for (i = 0; i < dishes.quantity; i++)
		if (!strcasecmp(dishes.menu[i].name, cmd.plat))
			break;

	if (i == dishes.quantity) {
		sprintf(aux, MSG_ORDER_KO "No has demanat el plat %s\n", cmd.plat);
		print(aux);
		return 1;
	}

	if (cmd.unitats > dishes.menu[i].stock) {
		sprintf(aux, MSG_ORDER_KO "Vols eliminar %d unitats i només n'has demanat %d.\n", cmd.unitats,
				dishes.menu[i].stock);
		print(aux);
		return 1;
	}

	memset(aux, '\0', LENGTH);
	memcpy(aux, cmd.plat, strlen(cmd.plat));

	strcat(aux, "&");

	memset(buffer, '\0', INT_LENGTH);
	myItoa(cmd.unitats, buffer);
	strcat(aux, buffer);

	resp = sendRemoveDish(aux);

	if (!resp)
		dishes.menu[i].stock -= cmd.unitats;

	return (char) (resp + 1);

}

char requestPayment() {
	Frame frame;
	char aux[LENGTH];
	int i, resp;

	frame = createFrame(CODE_PAYMENT, HEADER_PAYMENT, NULL);

	resp = writeFrame(frame);
	destroyFrame(&frame);

	if (!resp)
		return 0;

	frame = readFrame();

	if (frame.type == FRAME_NULL)
		return 0;

	if (!strcasecmp(frame.header, HEADER_PAYMENT_OK)) {
		config.money -= atoi(frame.data);    // NOLINT
		memset(aux, '\0', LENGTH);
		sprintf(aux, MSG_PAYMENT_OK " Et queden %d%s\n", config.money, MONEDA);
		print(aux);
		for (i = 0; i < dishes.quantity; i++) {
			free(dishes.menu[i].name);
		}
		free(dishes.menu);
		dishes.menu = NULL;
		dishes.quantity = 0;
	} else if (!strcasecmp(frame.header, HEADER_PAYMENT_KO)) {
		memset(aux, '\0', LENGTH);
		sprintf(aux, MSG_PAYMENT_KO " %s\n", frame.data);
		print(aux);
	}

	destroyFrame(&frame);
	return 1;
}

char recoverConnection() {
	char connected;
	Frame frame;
	char aux[LENGTH];
	char buffer[INT_LENGTH];
	int i, j, resp;


	print(MSG_CONN_RECOVERY);
	connected = initConnection();

	if (!connected)
		return 0;

	for (i = 0; i < dishes.quantity; i++) {

		if (!dishes.menu[i].stock)
			continue;

		memset(aux, '\0', LENGTH);
		memcpy(aux, dishes.menu[i].name, strlen(dishes.menu[i].name));

		strcat(aux, "&");

		memset(buffer, '\0', INT_LENGTH);
		myItoa(dishes.menu[i].stock, buffer);
		strcat(aux, buffer);

		frame = createFrame(CODE_REQUEST, HEADER_REQ_DISH, aux);
		resp = writeFrame(frame);
		destroyFrame(&frame);

		if (!resp)
			return 0;

		frame = readFrame();

		if (frame.type == FRAME_NULL)
			return 0;

		if (!strcasecmp(frame.header, HEADER_ORDER_OK)) {
			memset(aux, '\0', LENGTH);
			sprintf(aux, MSG_ORDER_OK " %s (x%d)\n", dishes.menu[i].name, dishes.menu[i].stock);
			print(aux);

		} else if (!strcasecmp(frame.header, HEADER_ORDER_KO)) {
			print(MSG_ORDER_KO);

			switch (atoi(frame.data)) {    // NOLINT
				case DATA_DISH_NOT_STOCK:
					for (j = 0; frame.data[j] != '&'; j++);
					dishes.menu[i].stock = atoi(frame.data + j + 1);    // NOLINT

					if (dishes.menu[i].stock) {
						sprintf(aux, " Reintentant amb %d unitats.\n", dishes.menu[i].stock);
						print(aux);
						i--;
					} else
						print(" No queda cap unitat.\n");

					break;
				case DATA_DISH_NOT_FOUND:
					print(" No s'ha trobat el plat demanat.\n");
					dishes.menu[i].stock = 0;
					break;
				default:
					print(" Error a la trama. S'ha rebut [");
					print(frame.data);
					print("]\n");
			}

		}

		destroyFrame(&frame);
	}

	return connected;
}

char recoveryHandler(Command cmd, char response) {
	char connected = 1;
	char *aux;

	if (!response) {
		print(MSG_CONN_LOSS " S'ha perdut la connexió amb el servidor!\n");
		close(sock);
		connected = recoverConnection();
		if (connected) {
			if (cmd.code == CODE_REMOVE || cmd.code == CODE_REQUEST) {
				aux = malloc(strlen(cmd.plat) + 1);
				memcpy(aux, cmd.plat, strlen(cmd.plat) + 1);
				cmd.plat = aux;
			}
			menuOptions(cmd);
		} else
			print("No s'ha pogut recuperar automàticament la connexió.\n");
	}
	return connected;
}

char menuOptions(Command cmd) {
	static char connected = 0;
	char aux[LENGTH];
	char resp;

	switch (cmd.code) {
		case CODE_CONNECT:
			debug("Toca connectar\n");
			if (connected) {
				print(MSG_CMD_KO " Ja estàs connectat a un Enterprise!\n");
				break;
			}
			connected = initConnection();
			break;

		case CODE_SHOWMENU:
			debug("Toca mostrar el menú\n");
			if (connected)
				connected = recoveryHandler(cmd, requestMenu());
			else
				print(MSG_CMD_KO " Encara no t'has connectat!\n");
			break;

		case CODE_SHOWORDER:
			debug("Toca mostrar la comanda\n");
			if (connected)
				showOrder();
			else
				print(MSG_CMD_KO " Encara no t'has connectat!\n");
			break;

		case CODE_REQUEST:
			debug("Toca demanar\n");
			if (connected)
				connected = recoveryHandler(cmd, requestDish(cmd));
			else
				print(MSG_CMD_KO " Encara no t'has connectat!\n");
			free(cmd.plat);
			break;

		case CODE_REMOVE:
			debug("Toca eliminar\n");
			if (connected)
				connected = recoveryHandler(cmd, removeDish(cmd));
			else
				print(MSG_CMD_KO " Encara no t'has connectat!\n");
			free(cmd.plat);
			break;

		case CODE_PAYMENT:
			debug("Toca pagar\n");
			if (connected)
				connected = recoveryHandler(cmd, requestPayment());
			else
				print(MSG_CMD_KO " Encara no t'has connectat!\n");
			break;

		case CODE_DISCONNECT:
			debug("Toca desconnectar\n");
			if (connected) {
				resp = disconnect(config.name);
				close(sock);
				if (!resp)
					print(MSG_CONN_LOSS " S'ha perdut la connexió amb el servidor!\n");
			} else
				print("Encara no t'havies connectat!\n");

			freeResources();
			freeHistory(DISCONNECT);
			return 1;

		case ERR_UNK_CMD:
			print(MSG_CONN_LOSS " Comanda no reconeguda");
			sprintf(aux, " -> [%s]", history[nLog - 1]);
			debug(aux);
			print("\n");
			free(cmd.plat);
			break;

		case ERR_N_PARAMS:
			print(MSG_CMD_KO " Nombre de paràmetres incorrecte\n");
			break;

		default:
			print(MSG_CMD_KO " Comanda no reconeguda\n");
			break;

	}

	return 0;
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {
	int i;

	print("\nGràcies per fer servir LsEat. Fins la propera.\n");
	free(config.name);
	free(config.ip);

	for (i = 0; i < dishes.quantity; i++) {
		free(dishes.menu[i].name);
	}
	if (dishes.quantity)
		free(dishes.menu);
}


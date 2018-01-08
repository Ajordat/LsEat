#include "logica.h"


char connectData() {
	Frame frame;
	char aux[LENGTH];

	sock_data = createClientSocket(config.ip_data, config.port_data);

	if (sock_data < 0)
		return 0;

	memset(aux, '\0', LENGTH);
	sprintf(aux, "%s&%s&%d", config.name, config.ip_picard, config.port_picard);
	frame = createFrame(CODE_CONNECT, "ENT_INF", aux);

	sendFrame(sock_data, frame);
	destroyFrame(&frame);

	frame = readFrame(sock_data);
	destroyFrame(&frame);

	close(sock_data);
	sock_data = -1;

	return !strcmp(frame.header, "CONOK");
}

/**
 * Funció per comprovar que el nombre d'arguments sigui correcte.
 *
 * @param argc 	Nombre d'arguments
 * @return 		1 si és correcte. Altrament, 0.
 */
char checkProgramArguments(int argc) {	//inline :(
	return argc == 3;
}

/**
 * Funció per llegir el fitxer de configuració de l'Enterprise.
 *
 * @param filename 	Fitxer de configuració de l'Enterprise
 */
void readConfigFile(char *filename) {
	int file;
	char msg[LENGTH], *aux;

	file = open(filename, O_RDONLY);
	if (file <= 0) {
		sprintf(msg, MSG_FILE_ERR, filename);
		print(msg);
		exit(EXIT_FAILURE);
	}

	config.name = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.refresh = atoi(aux); // NOLINT
	free(aux);

	config.ip_data = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.port_data = atoi(aux); // NOLINT
	free(aux);

	config.ip_picard = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.port_picard = atoi(aux); // NOLINT
	free(aux);

	close(file);

	sprintf(msg, "|%s/%d/%s/%d/%s/%d|\n", config.name, config.refresh,
			config.ip_data, config.port_data,
			config.ip_picard, config.port_picard);
	debug(msg);
}

/**
 * Funció per llegir el fitxer de menú de l'Enterprise.
 *
 * @param filename 	Fitxer de menú de l'Enterprise
 */
void readMenuFile(char *filename) {
	int file, index;
	char msg[LENGTH], *aux;
	Dish dish;

	file = open(filename, O_RDONLY);
	if (file <= 0) {
		sprintf(msg, MSG_FILE_ERR, filename);
		print(msg);

		free(config.name);
		free(config.ip_picard);
		free(config.ip_data);

		exit(EXIT_FAILURE);
	}

	menu.menu = NULL;
	index = 0;
	dish.name = NULL;

	while (1) {

		dish.name = readFileDescriptor(file);

		if (dish.name == NULL) {
			close(file);
			menu.quantity = index;
			return;
		}

		dish.name[strlen(dish.name) - 1] = '\0';

		sprintf(msg, "[(%s)]\n", dish.name);
		debug(msg);

		aux = readFileDescriptor(file);
		dish.stock = atoi(aux); // NOLINT
		free(aux);

		aux = readFileDescriptor(file);
		dish.price = atoi(aux); // NOLINT
		free(aux);

		menu.menu = realloc(menu.menu, sizeof(Dish) * (index + 1));
		menu.menu[index].name = dish.name;
		menu.menu[index].stock = dish.stock;
		menu.menu[index].price = dish.price;

		index++;
	}
}


void *updateInformation() {
	char aux[LENGTH];
	Frame frame;
	char resp;

	while (1) {
		sleep((uint) config.refresh);
		debug("[UPDATE]\n");
		sock_data = createClientSocket(config.ip_data, config.port_data);

		if (sock_data < 0) {
			print("[UPDATE FAILURE]\n");
			sock_data = -1;
			pthread_exit(NULL);
		}

		memset(aux, '\0', LENGTH);
		pthread_mutex_lock(&mutUsers);
		sprintf(aux, "%d&%d", config.port_picard, nUsers);
		pthread_mutex_unlock(&mutUsers);
		frame = createFrame(CODE_UPDATE, "UPDATE", aux);

		debugFrame(frame);
		resp = sendFrame(sock_data, frame);
		destroyFrame(&frame);

		if (!resp) {
			close(sock_data);
			sock_data = -1;
			print("[UPDATE FAILURE]\n");
			pthread_exit(NULL);
		}

		frame = readFrame(sock_data);	//TODO: LLEGIR SI SÍ O NO
		destroyFrame(&frame);

		close(sock_data);
		sock_data = -1;

		if (frame.type == FRAME_NULL) {
			print("[UPDATE FAILURE]\n");
			pthread_exit(NULL);
		}

	}
}

void createUpdateThread() {
	pthread_create(&update, NULL, (void *(*)(void *)) updateInformation, NULL);
	print(MSG_CREATE_UPD);
}


/**
 * Funció d'escolta del socket. Bloqueja l'execució esperant que un client es connecti al seu port.
 *
 * @param sock 	Socket a escoltar
 */
void listenSocket(int sock) {
	struct sockaddr_in addr;
	socklen_t addr_len;
	int *new_sock, aux_sock;
	pthread_t id;
	char aux[LENGTH];

	print(MSG_WAITING);

	listen(sock, MAX_REQUESTS);

	addr_len = sizeof(addr);

	mutUsers = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	mutMenu = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	nUsers = 0;

	while (1) {

		debug("[WAITING]\n");
		if ((aux_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
			sprintf(aux, MSG_CONN_ERR, strerror(errno));
			print(aux);
			freeResources(CONNECTED);
			exit(EXIT_FAILURE);
		}

		//TODO: Arreglar (i canviar documentació)
		//Funciona bé, però si rep un SIGINT mentre hi ha un thread obert és una cagada, es queda la
		//memòria dinàmica del thread sense alliberar. S'ha de trobar una manera millor.

		new_sock = malloc(sizeof(int));
		*new_sock = aux_sock;

		pthread_create(&id, NULL, attendPetition, new_sock);
		pthread_detach(id);
	}
}

char sendMenu(int sock) {
	int i, quantity;
	Frame frame;
	char dish[LENGTH];
	char buffer[INT_LENGTH];
	char resp;

	pthread_mutex_lock(&mutMenu);
	quantity = menu.quantity;
	pthread_mutex_unlock(&mutMenu);

	for (i = 0; i < quantity; i++) {

		memset(dish, '\0', LENGTH);

		pthread_mutex_lock(&mutMenu);
		memcpy(dish, menu.menu[i].name, strlen(menu.menu[i].name));
		pthread_mutex_unlock(&mutMenu);

		strcat(dish, "&");

		memset(buffer, '\0', INT_LENGTH);

		pthread_mutex_lock(&mutMenu);
		myItoa(menu.menu[i].price, buffer);    // NOLINT
		pthread_mutex_unlock(&mutMenu);

		strcat(dish, buffer);
		strcat(dish, "&");

		memset(buffer, '\0', INT_LENGTH);

		pthread_mutex_lock(&mutMenu);
		myItoa(menu.menu[i].stock, buffer);    // NOLINT
		pthread_mutex_unlock(&mutMenu);

		strcat(dish, buffer);

		frame = createFrame(CODE_SHOWMENU, HEADER_SHW_MENU_DISH, dish);
		resp = sendFrame(sock, frame);
		destroyFrame(&frame);

		if (!resp)
			return 0;
	}

	frame = createFrame(CODE_SHOWMENU, HEADER_SHW_MENU_END_DISH, NULL);
	resp = sendFrame(sock, frame);
	destroyFrame(&frame);

	return resp;
}

Dish parseDish(const char *string) {
	Dish dish;
	char aux[INT_LENGTH];
	int i, j;

	dish.name = malloc(sizeof(char));
	for (i = 0; string[i] != '&'; i++) {
		dish.name[i] = string[i];
		dish.name = realloc(dish.name, (size_t) i + 2);
	}
	dish.name[i] = '\0';

	memset(aux, '\0', INT_LENGTH);
	for (j = 0, i++; string[i]; i++, j++) {
		aux[j] = string[i];
	}
	dish.stock = atoi(aux);    // NOLINT

	return dish;
}

char acceptDish(int sock, Frame frame, Menu *dishes) {
	Dish dish;
	int i, flag = 0, quantity;
	size_t length;
	char aux[LENGTH];
	char resp;

	dish = parseDish(frame.data);
	destroyFrame(&frame);

	sprintf(aux, MSG_REQ_DISH, dish.name);
	print(aux);

	pthread_mutex_lock(&mutMenu);
	quantity = menu.quantity;
	pthread_mutex_unlock(&mutMenu);

	for (i = 0; i < quantity; i++) {
		pthread_mutex_lock(&mutMenu);
		if (!strcasecmp(menu.menu[i].name, dish.name)) {
			if (menu.menu[i].stock >= dish.stock && dish.stock > 0) {
				menu.menu[i].stock -= dish.stock;
				dish.price = menu.menu[i].price;
				pthread_mutex_unlock(&mutMenu);

				sprintf(aux, MSG_REQ_OK, dish.stock);
				print(aux);

				frame = createFrame(CODE_REQUEST, HEADER_REQ_OK, NULL);
				debug("ORDOK\n");

				for (i = 0; i < dishes->quantity; i++) {
					if (!strcasecmp(dishes->menu[i].name, dish.name)) {
						dishes->menu[i].stock += dish.stock;
						break;
					}
				}
				if (i == dishes->quantity) {
					dishes->quantity++;
					dishes->menu = realloc(dishes->menu, sizeof(Dish) * dishes->quantity);
					length = strlen(dish.name) + 1;
					dishes->menu[i].name = malloc(length);
					memcpy(dishes->menu[i].name, dish.name, length);
					dishes->menu[i].stock = dish.stock;
					dishes->menu[i].price = dish.price;
				}
				for (i = 0; i < dishes->quantity; i++) {
					sprintf(aux, "[ADDED] %s (x%d)\n", dishes->menu[i].name, dishes->menu[i].stock);
					debug(aux);
				}
			} else {
				sprintf(aux, "%d&%d", DATA_DISH_NOT_STOCK, menu.menu[i].stock);
				pthread_mutex_unlock(&mutMenu);

				print(MSG_REQ_KO);
				frame = createFrame(CODE_REQUEST, HEADER_REQ_KO, aux);
				debug("ORDKO\n");
			}
			flag = 1;
			break;
		} else pthread_mutex_unlock(&mutMenu);
	}

	if (!flag) {
		print(MSG_REQ_NOT_FOUND);
		frame = createFrame(CODE_REQUEST, HEADER_REQ_KO, DATA_DISH_NOT_FOUND);
		debug("ORDKO\n");
	}

	resp = sendFrame(sock, frame);

	destroyFrame(&frame);
	free(dish.name);
	return resp;
}

char removeDish(int sock, Frame frame, Menu *dishes) {
	Dish dish;
	int i, flag, quantity;
	char resp;
	char aux[LENGTH];

	dish = parseDish(frame.data);
	destroyFrame(&frame);

	sprintf(aux, MSG_REM_DISH, dish.name);
	print(aux);

	pthread_mutex_lock(&mutMenu);
	quantity = menu.quantity;
	pthread_mutex_unlock(&mutMenu);

	for (i = 0, flag = 0; i < dishes->quantity; i++) {
		if (!strcasecmp(dish.name, dishes->menu[i].name)) {
			if (dishes->menu[i].stock >= dish.stock && dish.stock > 0) {
				dishes->menu[i].stock -= dish.stock;

				for (i = 0; i < quantity; i++) {
					pthread_mutex_lock(&mutMenu);
					if (!strcasecmp(dish.name, menu.menu[i].name)) {
						menu.menu[i].stock += dish.stock;
						pthread_mutex_unlock(&mutMenu);

						sprintf(aux, MSG_REM_OK, dish.stock);
						print(aux);

						frame = createFrame(CODE_REMOVE, HEADER_REM_OK, NULL);
						debug("ORDOK\n");
						flag = 1;
						break;
					} else {
						pthread_mutex_unlock(&mutMenu);
					}
				}
				pthread_mutex_unlock(&mutMenu);
			} else {
				print(MSG_REM_KO);
				frame = createFrame(CODE_REMOVE, HEADER_REM_KO, DATA_REM_KO);
				debug("ORDKO\n");
			}
			break;
		}
	}

	if (!flag) {
		print(MSG_REM_NOT_FOUND);
		frame = createFrame(CODE_REMOVE, HEADER_REM_KO, DATA_REM_NOT_FOUND);
		debug("ORDKO\n");
	}

	resp = sendFrame(sock, frame);

	destroyFrame(&frame);
	free(dish.name);
	return resp;
}

char solvePayment(int sock, Picard *picard, Menu *dishes) {
	Frame frame;
	int cost;
	int i, quantity;
	char aux[LENGTH];
	char resp;

	cost = quantity = 0;
	for (i = 0; i < dishes->quantity; i++) {
		quantity += dishes->menu[i].stock;
		cost += dishes->menu[i].stock * dishes->menu[i].price;
	}

	if (!cost && !quantity) {
		print(MSG_PAY_EMPTY);
		frame = createFrame(CODE_PAYMENT, HEADER_PAY_KO, DATA_PAY_EMPTY);

	} else if (picard->money >= cost) {
		sprintf(aux, MSG_PAY_OK, cost, MONEDA);
		print(aux);

		memset(aux, '\0', LENGTH);
		myItoa(cost, aux);
		frame = createFrame(CODE_PAYMENT, HEADER_PAY_OK, aux);
		for (i = 0; i < dishes->quantity; i++) {
			free(dishes->menu[i].name);
		}
		free(dishes->menu);
		dishes->menu = NULL;
		dishes->quantity = 0;
		picard->money -= cost;

	} else {
		sprintf(aux, MSG_PAY_CANT_AFFORD, cost, MONEDA, picard->money, MONEDA);
		print(aux);

		sprintf(aux, DATA_PAY_CANT_AFFORD, cost, MONEDA, picard->money, MONEDA);
		frame = createFrame(CODE_PAYMENT, HEADER_PAY_KO, aux);
	}

	resp = sendFrame(sock, frame);
	destroyFrame(&frame);
	return resp;
}

void exitThread(Menu *dishes) {
	int i, j;

	pthread_mutex_lock(&mutUsers);
	nUsers--;
	pthread_mutex_unlock(&mutUsers);

	pthread_mutex_lock(&mutMenu);
	for (i = 0; i < dishes->quantity; i++) {
		for (j = 0; j < menu.quantity; j++) {
			if (!strcasecmp(dishes->menu[i].name, menu.menu[j].name)) {
				menu.menu[j].stock += dishes->menu[i].stock;
				break;
			}
		}
		free(dishes->menu[i].name);
	}
	pthread_mutex_unlock(&mutMenu);
	if (dishes->quantity)
		free(dishes->menu);
}

/**
 * Funció dedicada a un únic socket fins que aquest es desconnecti des de Picard. Aquesta funció es crida
 * en un thread deatached.
 *
 * @param sock_aux	Socket de la connexió amb el Picard
 * @return			NULL
 */
void *attendPetition(void *sock_aux) {
	Frame frame;
	int sock = *((int *) sock_aux);
	Menu dishes;
	Picard picard;
	int active = 0;

	free(sock_aux);
	dishes.quantity = 0;
	dishes.menu = NULL;

	do {
		frame = readFrame(sock);
		if (frame.type == FRAME_NULL)
			break;

		debugFrame(frame);
		switch (frame.type) {

			case CODE_CONNECT:
				print(MSG_CONN_IN);
				active = connectPicard(sock, frame, &picard);
				break;

			case CODE_DISCONNECT:
				print(MSG_DISC_IN);
				disconnectPicard(sock, frame);
				active = 0;
				break;

			case CODE_SHOWMENU:
				print(MSG_SHOW_MENU_IN);
				destroyFrame(&frame);
				active = sendMenu(sock);
				break;

			case CODE_REQUEST:
				print(MSG_REQ_IN);
				active = acceptDish(sock, frame, &dishes);
				break;

			case CODE_REMOVE:
				print(MSG_REM_IN);
				active = removeDish(sock, frame, &dishes);
				break;

			case CODE_PAYMENT:
				print(MSG_PAY_IN);
				destroyFrame(&frame);
				active = solvePayment(sock, &picard, &dishes);
				break;

			default:
				break;

		}
	} while (active);

	exitThread(&dishes);

	close(sock);
	return NULL;
}

/**
 * Funció per a connectar un Picard a l'Enterprise. Rep la petició com a paràmetre i respon al Picard
 * en qüestió segons si pot connectar-lo.
 *
 * @param sock 		Socket de la communicació
 * @param frame 	Trama rebuda amb informació sobre el Picard.
 */
char connectPicard(int sock, Frame frame, Picard *picard) {
	char *name, *money;
	int ref, i;
	char aux[LENGTH];
	char resp;

	name = frame.data;

	debug("[READING FRAME]\n");
	for (ref = 0; name[ref] != '&'; ref++);

	money = malloc((size_t) frame.length - ref + 1);
	memset(money, '\0', (size_t) frame.length - ref + 1);
	debug("[READING MONEY]\n");

	for (i = ref + 1; i < frame.length; i++)
		money[i - ref - 1] = name[i];

	debug("[DONE]\n");
	sprintf(aux, "[MONEY] -> |%s|\n", money);
	debug(aux);

	picard->money = atoi(money); // NOLINT
	free(money);
	name[ref] = '\0';
	picard->name = malloc((size_t) strlen(name) + 1);
	memset(picard->name, '\0', (size_t) strlen(name) + 1);
	strcpy(picard->name, name);
	debug("[DONE]\n");

	destroyFrame(&frame);

	frame = createFrame(CODE_CONNECT, HEADER_PIC_CONN_OK, NULL);

	pthread_mutex_lock(&mutUsers);
	nUsers++;
	pthread_mutex_unlock(&mutUsers);

	debug("[SENDING FRAME]\n");

	resp = sendFrame(sock, frame);

	if (resp)
		debugFrame(frame);
	debug("[SENT]\n");

	sprintf(aux, MSG_CONN_PIC, picard->name);
	print(aux);

	destroyFrame(&frame);
	free(picard->name);
	return resp;
}

/**
 * Funció per a desconnectar un Picard de l'Enterprise. Rep la petició com a paràmetre i respon al Picard
 * en qüestió segons si pot desconnectar-lo.
 *
 * @param sock 		Socket de la communicació
 * @param frame 	Trama rebuda amb informació sobre el Picard.
 */
void disconnectPicard(int sock, Frame frame) {
	char *name;
	char aux[LENGTH];


	name = malloc(strlen(frame.data) + 1);
	memset(name, '\0', strlen(frame.data) + 1);
	strcpy(name, frame.data);

	destroyFrame(&frame);

	debug("[SENDING FRAME]\n");
	frame = createFrame(CODE_DISCONNECT, HEADER_PIC_ENT_DISC_OK, NULL);    //TODO: Hi haurà algun cas que serà KO?
	sendFrame(sock, frame);

	debugFrame(frame);
	debug("[SENT]\n");

	sprintf(aux, MSG_DISC_PIC, name);
	print(aux);
	free(name);
	destroyFrame(&frame);
}

void disconnectFromData() {
	char aux[LENGTH];
	Frame frame;
	char resp;

	sock_data = createClientSocket(config.ip_data, config.port_data);

	if (sock_data < 0)
		return;

	memset(aux, '\0', LENGTH);
	sprintf(aux, "%d", config.port_picard);
	frame = createFrame(CODE_DISCONNECT, "ENT_INF", aux);

	debugFrame(frame);
	resp = sendFrame(sock_data, frame);
	destroyFrame(&frame);

	if (!resp) {
		close(sock_data);
		sock_data = -1;
		return;
	}

	frame = readFrame(sock_data);
	debugFrame(frame);
	destroyFrame(&frame);

	close(sock_data);
	sock_data = -1;
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources(char connected) {
	int i;


	pthread_cancel(update);
	pthread_join(update, NULL);

	pthread_mutex_destroy(&mutUsers);
	pthread_mutex_destroy(&mutMenu);

	if (connected)
		disconnectFromData();

	free(config.name);
	free(config.ip_picard);
	free(config.ip_data);

	for (i = 0; i < menu.quantity; i++)
		free(menu.menu[i].name);

	free(menu.menu);

	if (sock_picard >= 0)
		close(sock_picard);

	if (sock_data >= 0)
		close(sock_data);
}

/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint() {
	freeResources(CONNECTED);
	exit(EXIT_SUCCESS);
}

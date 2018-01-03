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
	free(frame.data);

	frame = readFrame(sock_data);
	free(frame.data);

	close(sock_data);

	return !strcmp(frame.header, "CONOK");
}

/**
 * Funció per comprovar que el nombre d'arguments sigui correcte.
 *
 * @param argc 	Nombre d'arguments
 * @return 		0 si és correcte. Altrament, 1.
 */
char checkProgramArguments(int argc) {
	char aux[LENGTH];

	if (argc != 3) {
		sprintf(aux, "El format de la crida és incorrecte, ha de ser:\n");
		print(aux);
		sprintf(aux, "\tenterprise\t<config_file.dat>\t<menu_file.dat>\n");
		print(aux);
		return 1;
	}
	return 0;
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

//TODO thread de update
void *updateInformation() {
	char aux[LENGTH];
	Frame frame;

	while (1) {
		sleep((uint) config.refresh);
		debug("[UPDATE]\n");
		sock_data = createClientSocket(config.ip_data, config.port_data);

		if (sock_data < 0)
			return NULL;

		memset(aux, '\0', LENGTH);
		sprintf(aux, "%d&%d", config.port_picard, nUsers);
		frame = createFrame(CODE_UPDATE, "UPDATE", aux);

		debugFrame(frame);
		sendFrame(sock_data, frame);
		free(frame.data);

		frame = readFrame(sock_data);
		debugFrame(frame);
		free(frame.data);

		close(sock_data);
	}
	return NULL;
}

void updateThread() {

//	fiUpdate = 0;
	pthread_create(&update, NULL, updateInformation, NULL);
//	pthread_detach(update);

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

	print(MSG_WAITING);

	listen(sock, MAX_REQUESTS);

	addr_len = sizeof(addr);

	mutUsers = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	mutMenu = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	nUsers = 0;

	while (1) {

		debug("[WAITING]\n");
		if ((aux_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
			print(MSG_CONEX_ERR);
			freeResources();
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

void sendMenu(int sock) {
	int i, quantity;
	Frame frame;
	char dish[LENGTH];
	char buffer[INT_LENGTH];

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

		frame = createFrame(CODE_SHOWMENU, "DISH", dish);
		sendFrame(sock, frame);
		destroyFrame(&frame);
	}
	frame = createFrame(CODE_SHOWMENU, "END_MENU", NULL);
	sendFrame(sock, frame);
	destroyFrame(&frame);
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

void acceptDish(int sock, Frame frame, Menu *dishes) {
	Dish dish;
	int i, flag = 0, quantity;
	size_t length;
	char aux[LENGTH];

	dish = parseDish(frame.data);
	destroyFrame(&frame);

	pthread_mutex_lock(&mutMenu);
	quantity = menu.quantity;
	pthread_mutex_unlock(&mutMenu);

	for (i = 0; i < quantity; i++) {
		pthread_mutex_lock(&mutMenu);
		if (!strcasecmp(menu.menu[i].name, dish.name)) {
			if (menu.menu[i].stock >= dish.stock && dish.stock > 0) {
				menu.menu[i].stock -= dish.stock;
				pthread_mutex_unlock(&mutMenu);
				frame = createFrame(CODE_REQUEST, "ORDOK", NULL);
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
				}
				for (i = 0; i < dishes->quantity; i++) {
					sprintf(aux, "[ADDED] %s (x%d)\n", dishes->menu[i].name, dishes->menu[i].stock);
					debug(aux);
				}
			} else {
				pthread_mutex_unlock(&mutMenu);
				frame = createFrame(CODE_REQUEST, "ORDKO",
									dish.stock > 0 ? "No hi ha tant stock del plat." : "Valor d'stock incorrecte.");
				debug("ORDKO\n");
			}
			flag = 1;
			break;
		} else pthread_mutex_unlock(&mutMenu);
	}

	if (!flag) {
		frame = createFrame(CODE_REQUEST, "ORDKO", "No s'ha trobat el plat demanat.");
		debug("ORDKO\n");
	}

	sendFrame(sock, frame);

	destroyFrame(&frame);
	free(dish.name);
}

void removeDish(int sock, Frame frame, Menu *dishes) {
	Dish dish;
	int i, flag;

	dish = parseDish(frame.data);
	destroyFrame(&frame);

	for (i = 0, flag = 0; i < dishes->quantity; i++) {
		if (!strcasecmp(dish.name, dishes->menu[i].name)) {
			if (dishes->menu[i].stock >= dish.stock && dish.stock > 0) {
				dishes->menu[i].stock -= dish.stock;
				pthread_mutex_lock(&mutMenu);
				for (i = 0; i < menu.quantity; i++) {
					if (!strcasecmp(dish.name, menu.menu[i].name)) {
						menu.menu[i].stock += dish.stock;
						pthread_mutex_unlock(&mutMenu);
						frame = createFrame(CODE_REMOVE, "ORDOK", NULL);
						debug("ORDOK\n");
						flag = 1;
						break;
					}
				}
				pthread_mutex_unlock(&mutMenu);
			} else {
				frame = createFrame(CODE_REMOVE, "ORDKO", "No es pot cancelar aquesta quantitat d'unitats.");
				debug("ORDKO\n");
			}
			break;
		}
	}

	if (!flag) {
		frame = createFrame(CODE_REMOVE, "ORDKO", "No s'ha trobat el plat demanat.");
		debug("ORDKO\n");
	}

	sendFrame(sock, frame);

	destroyFrame(&frame);
	free(dish.name);
}

/**
 * Funció dedicada a un únic socket fins que aquest es desconnecti des de Picard. Aquesta funció es crida
 * en un thread deatached per a poder tenir diversos Picards de manera simultània.
 *
 * @param sock_aux	Socket de la connexió amb el Picard
 * @return			NULL
 */
void *attendPetition(void *sock_aux) {
	Frame frame;
	Menu dishes;
	int sock = *((int *) sock_aux);


	free(sock_aux);
	dishes.quantity = 0;
	dishes.menu = NULL;

	do {
		frame = readFrame(sock);
		debugFrame(frame);
		switch (frame.type) {

			case CODE_CONNECT:
				connectPicard(sock, frame);
				break;

			case CODE_DISCONNECT:
				disconnectPicard(sock, frame, &dishes);
				break;

			case CODE_SHOWMENU:
				destroyFrame(&frame);
				sendMenu(sock);
				break;

			case CODE_REQUEST:
				acceptDish(sock, frame, &dishes);
				break;

			case CODE_REMOVE:
				removeDish(sock, frame, &dishes);
				break;

			default:
				break;

		}
	} while (frame.type != CODE_DISCONNECT);

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
void connectPicard(int sock, Frame frame) {
	Picard picard;        //TODO: Això serà un element d'alguna estructura (llista segurament)
	char *name, *money;
	int ref, i;
	char aux[LENGTH];

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

	picard.money = atoi(money); // NOLINT
	free(money);
	name[ref] = '\0';
	picard.name = malloc((size_t) strlen(name) + 1);
	memset(picard.name, '\0', (size_t) strlen(name) + 1);
	strcpy(picard.name, name);    //TODO: Inserir a una llista de sockets
	debug("[DONE]\n");

	free(frame.data);

	//TODO: Hi haurà algun cas que serà KO, possiblement si s'arriba a un màxim de connexions o algo així
	frame = createFrame(CODE_CONNECT, HEADER_PIC_ENT_CONN_OK, NULL);
	pthread_mutex_lock(&mutUsers);
	nUsers++;
	pthread_mutex_unlock(&mutUsers);

	debug("[SENDING FRAME]\n");

	sendFrame(sock, frame);
	debugFrame(frame);
	debug("[SENT]\n");

	sprintf(aux, MSG_CONN_PIC, picard.name);
	print(aux);

	free(frame.data);
	free(picard.name);    //TODO: Eliminar free
}

/**
 * Funció per a desconnectar un Picard de l'Enterprise. Rep la petició com a paràmetre i respon al Picard
 * en qüestió segons si pot desconnectar-lo.
 *
 * @param sock 		Socket de la communicació
 * @param frame 	Trama rebuda amb informació sobre el Picard.
 */
void disconnectPicard(int sock, Frame frame, Menu *dishes) {
	char *name;
	char aux[LENGTH];
	int i, j;

	name = malloc(sizeof(char) * (strlen(frame.data) + 1));
	memset(name, '\0', sizeof(char) * (strlen(frame.data) + 1));
	strcpy(name, frame.data);

	destroyFrame(&frame);

	debug("[SENDING FRAME]\n");
	frame = createFrame(CODE_DISCONNECT, HEADER_PIC_ENT_DISC_OK, NULL);    //TODO: Hi haurà algun cas que serà KO
	pthread_mutex_lock(&mutUsers);
	nUsers--;
	pthread_mutex_unlock(&mutUsers);
	sendFrame(sock, frame);

	debugFrame(frame);
	debug("[SENT]\n");

	sprintf(aux, MSG_DISC_PIC, name);
	print(aux);
	free(name);
	destroyFrame(&frame);

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
}

void disconnectFromData() {
	char aux[LENGTH];
	Frame frame;

	sock_data = createClientSocket(config.ip_data, config.port_data);

	if (sock_data < 0)
		return;

	memset(aux, '\0', LENGTH);
	sprintf(aux, "%d", config.port_picard);
	frame = createFrame(CODE_DISCONNECT, "ENT_INF", aux);

	debugFrame(frame);
	sendFrame(sock_data, frame);
	free(frame.data);

	frame = readFrame(sock_data);
	debugFrame(frame);
	free(frame.data);
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {
	int i = 0;

	//TODO: Avisar a Data de la desconnexió de l'Enterprise

//	fiUpdate = 1;
	pthread_cancel(update);
//	pthread_join(update, NULL);
	pthread_mutex_destroy(&mutUsers);

	disconnectFromData();

	free(config.name);
	free(config.ip_picard);
	free(config.ip_data);

	for (i = 0; i < menu.quantity; i++)
		free(menu.menu[i].name);

	free(menu.menu);

	if (sock_picard > 0)
		close(sock_picard);

	if (sock_data > 0)
		close(sock_data);

}

/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint() {
	freeResources();
	exit(EXIT_SUCCESS);
}

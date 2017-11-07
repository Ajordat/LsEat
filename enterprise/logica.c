#include "logica.h"

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
	menu.quantity = index = 0;
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

/**
 * Funció d'escolta del socket. Bloqueja l'execució esperant que un client es connecti al seu port.
 * Hem intentat que sigui dedicat, però és complicat perquè hi ha un int que no acabem d'alliberar.
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

	while (1) {

		debug("[WAITING]\n");
		if ((aux_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
			print(MSG_CONEX_ERR);
			freeResources();
			exit(EXIT_FAILURE);
		}

		//TODO: Arreglar (i canviar documentació)
		//Funciona bé, però si rep un SIGINT mentre hi ha un thread obert és una cagada, es queda la
		//informació del thread sense alliberar. S'ha de trobar una manera millor.

		new_sock = malloc(sizeof(int));
		*new_sock = aux_sock;

		pthread_create(&id, NULL, attendPetition, new_sock);
		pthread_detach(id);
	}

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
	int sock = *((int *) sock_aux);

	free(sock_aux);

	do {
		frame = readFrame(sock);
		debugFrame(frame);
		switch (frame.type) {
			case CODE_CONNECT:
				connectPicard(sock, frame);
				break;

			case CODE_DISCONNECT:
				disconnectPicard(sock, frame);
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

	money = malloc(sizeof(char) * (frame.length - ref + 1));
	memset(money, '\0', sizeof(char) * (frame.length - ref + 1));
	debug("[READING MONEY]\n");
	for (i = ref + 1; i < frame.length; i++)
		money[i - ref - 1] = name[i];

	debug("[DONE]\n");
	sprintf(aux, "[MONEY] -> |%s|\n", money);
	debug(aux);

	picard.money = atoi(money); // NOLINT
	free(money);
	name[ref] = '\0';
	picard.name = malloc(sizeof(char) * (strlen(name) + 1));
	memset(picard.name, '\0', sizeof(char) * (strlen(name) + 1));
	strcpy(picard.name, name);
	debug("[DONE]\n");

	free(frame.data);

	//TODO: Hi haurà algun cas que serà KO, possiblement si s'arriba a un màxim de connexions o algo així
	frame = createFrame(CODE_CONNECT, HEADER_PIC_ENT_CONN_OK, NULL);
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
void disconnectPicard(int sock, Frame frame) {
	char *name;
	char aux[LENGTH];

	name = malloc(sizeof(char) * (strlen(frame.data) + 1));
	memset(name, '\0', sizeof(char) * (strlen(frame.data) + 1));
	strcpy(name, frame.data);

	free(frame.data);

	debug("[SENDING FRAME]\n");
	frame = createFrame(CODE_DISCONNECT, HEADER_PIC_ENT_DISC_OK, NULL);	//TODO: Hi haurà algun cas que serà KO

	sendFrame(sock, frame);

	debugFrame(frame);
	debug("[SENT]\n");

	sprintf(aux, MSG_DISC_PIC, name);
	print(aux);
	free(name);
	free(frame.data);
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {
	int i = 0;

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

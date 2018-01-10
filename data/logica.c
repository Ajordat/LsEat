#include "logica.h"
#include "MinHeap.h"

/**
 * Llegeix el fitxer de configuració que conté la seva ip i els dos ports pels Picards i els Enterprise.
 *
 * @param filename 	Nom del fitxer de configuració
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
	debug("Tot bé!\n");

	config.ip = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.port_picard = atoi(aux); // NOLINT
	free(aux);

	aux = readFileDescriptor(file);
	config.port_enterprise = atoi(aux); // NOLINT
	free(aux);

	sock_picard = sock_enterprise = -1;

	sprintf(msg, "|%s - %d - %d|\n", config.ip, config.port_picard, config.port_enterprise);
	debug(msg);
}

/**
 * Funció d'escolta del socket. Bloqueja l'execució esperant que un client es connecti al port de Picard o d'Enterprise.
 * No és dedicat.
 *
 * @param sock 	Socket a escoltar
 */
void listenServerSockets() {
	struct sockaddr_in addr;
	socklen_t addr_len;
	int new_sock;
	fd_set read_fds;

	print(MSG_WAITING);

	listen(sock_enterprise, MAX_REQUESTS);
	listen(sock_picard, MAX_REQUESTS);

	addr_len = sizeof(addr);

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(sock_enterprise, &read_fds);
		FD_SET(sock_picard, &read_fds);

		if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) > 0) {

			new_sock = accept(
					FD_ISSET(sock_enterprise, &read_fds) ?
					sock_enterprise : sock_picard,
					(void *) &addr,
					&addr_len);

			if (new_sock < 0) {
				print(MSG_CONEX_ERR);
				freeResources();
				exit(EXIT_FAILURE);
			}

			attendPetition(new_sock);
			close(new_sock);

			if (DEBUG)
				HEAP_print(minheap);

		} else return;

	}
}

void updateEnterprise(int sock, const char *data) {
	int i, j;
	Enterprise e, found;
	char aux[LENGTH];
	Frame frame;
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);

	getpeername(sock, (struct sockaddr *) &addr, &addr_size);

	e.ip = malloc(IP_SIZE);
	memset(e.ip, '\0', IP_SIZE);
	strcpy(e.ip, inet_ntoa(addr.sin_addr));

	debug(e.ip);

	for (i = 0; data[i] != '&'; i++) {
		aux[i] = data[i];
	}
	aux[i] = '\0';
	e.port = atoi(aux); // NOLINT
	debug("\nPort: ");
	debug(aux);

	for (j = 0, i++; data[i]; i++, j++) {
		aux[j] = data[i];
	}
	aux[j] = '\0';
	e.users = atoi(aux); // NOLINT
	debug("\nUsers: ");
	debug(aux);
	debug("\n");


	found = HEAP_update(&minheap, e);
	free(e.ip);

	if (found.port < 0) {
		print(MSG_UPDATE_KO);
		frame = createFrame(CODE_UPDATE, HEADER_ENT_UPDATE_KO, NULL);
	} else {
		sprintf(aux, MSG_UPDATE_OK, found.name, e.users);
		print(aux);
		frame = createFrame(CODE_UPDATE, HEADER_ENT_UPDATE_OK, NULL);
	}

	sendFrame(sock, frame);
	destroyFrame(&frame);
}

void disconnectEnterprise(int sock, const char *data) {
	int i;
	char aux[LENGTH];
	Frame frame;
	char answer;
	Enterprise e;
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);

	getpeername(sock, (struct sockaddr *) &addr, &addr_size);

	e.ip = malloc(IP_SIZE);
	memset(e.ip, '\0', IP_SIZE);
	strcpy(e.ip, inet_ntoa(addr.sin_addr));

	debug(e.ip);
	debug("\n");

	for (i = 0; data[i]; i++) {
		aux[i] = data[i];
	}
	aux[i] = '\0';
	e.port = atoi(aux);    // NOLINT
	answer = HEAP_disconnect(&minheap, e);

	if (answer) {
		print(MSG_DISC_OK);
		frame = createFrame(CODE_DISCONNECT, HEADER_ENT_DISC_OK, NULL);
	} else {
		print(MSG_DISC_KO);
		frame = createFrame(CODE_DISCONNECT, HEADER_ENT_DISC_KO, NULL);
	}
	sendFrame(sock, frame);
	destroyFrame(&frame);
	free(e.ip);
}

/**
 * Funció per executar les peticions dels clients a partir del camp type de la trama.
 *
 * @param sock 	Socket de la connexió amb el client
 */
void attendPetition(int sock) {
	Frame frame;

	frame = readFrame(sock);

	if (frame.type == FRAME_NULL)
		return;

	debugFrame(frame);

	switch (frame.type) {
		case CODE_CONNECT:
			print(MSG_CONNEX_IN);
			connectSocket(sock, frame);
			break;

		case CODE_UPDATE:
			print(MSG_UPDATE_IN);
			updateEnterprise(sock, frame.data);
			destroyFrame(&frame);
			break;

		case CODE_DISCONNECT:        //TODO: HI HA ALGUN CAS QUE NO ALLIBERA UN ENTERPRISE
			print(MSG_DISC_IN);
			disconnectEnterprise(sock, frame.data);
			destroyFrame(&frame);
			break;

		default:
			break;

	}
}

Enterprise parseEnterprise(const char *data) {
	int i, j;
	char *aux;
	Enterprise ent;

	ent.name = malloc(sizeof(char));
	for (i = 0, j = 0; data[j] != '&'; j++) {
		ent.name[i] = data[j];
		i++;
		ent.name = realloc(ent.name, (size_t) i + 1);
	}
	ent.name[i] = '\0';
	j++;

	ent.ip = malloc(sizeof(char));
	for (i = 0; data[j] != '&'; j++) {
		ent.ip[i] = data[j];
		i++;
		ent.ip = realloc(ent.ip, (size_t) i + 1);
	}
	ent.ip[i] = '\0';
	j++; //saltem &

	aux = malloc(sizeof(char));
	for (i = 0; data[j] != '&'; j++) {
		aux[i] = data[j];
		i++;
		aux = realloc(aux, (size_t) i + 1);
	}
	aux[i] = '\0';
	ent.port = atoi(aux); // NOLINT
	free(aux);

	aux = malloc(sizeof(char));
	for (i = 0, j++; data[j]; j++) {
		aux[i] = data[j];
		i++;
		aux = realloc(aux, (size_t) i + 1);
	}
	aux[i] = '\0';
	ent.time = atoi(aux); // NOLINT
	free(aux);

	ent.users = 0;

	return ent;
}

/**
 * Funció per tractar les peticions de connexió, tant de Picard com d'enterprise.
 *
 * @param sock 		Socket de connexió amb el client
 * @param frame 	Trama rebuda del client amb la seva informació.
 */
void connectSocket(int sock, Frame frame) {
	char *name;
	char aux[LENGTH];
	Enterprise ent;

	if (!strcmp(frame.header, HEADER_PIC_CONN_REQ)) {
		print(MSG_CONNECT_PIC);

		name = malloc(strlen(frame.data) + 1);
		strcpy(name, frame.data);
		destroyFrame(&frame);

		sprintf(aux, MSG_WELCOME_CLIENT, name);
		print(aux);

		frame = getEnterpriseConnection();
		debugFrame(frame);
		sendFrame(sock, frame);

		destroyFrame(&frame);
		free(name);

	} else if (!strcmp(frame.header, HEADER_ENT_CONN_REQ)) {
		print(MSG_CONNECT_ENT);

		debug(frame.data);
		debug("\n");

		ent = parseEnterprise(frame.data);
		destroyFrame(&frame);

		sprintf(aux, MSG_WELCOME_CLIENT, ent.name);
		print(aux);

		if (HEAP_find(minheap, ent) >= 0) {
			print(MSG_CONN_ENT_KO);
			frame = createFrame(CODE_CONNECT, HEADER_ENT_CON_KO, NULL);
			free(ent.name);
			free(ent.ip);
		} else {
			print(MSG_CONN_ENT_OK);
			HEAP_push(&minheap, ent);

			frame = createFrame(CODE_CONNECT, HEADER_ENT_CON_OK, NULL);
		}
		sendFrame(sock, frame);
		destroyFrame(&frame);
	} else {
		print(MSG_CONNECT_UNK);
	}
}

/**
 * Genera la trama de resposta al client amb la direcció ip i port d'un Enterprise, si tot ha anat bé.
 *
 * @return 	Trama a respondre al client
 */
Frame getEnterpriseConnection() {
	Enterprise ent;
	char aux[LENGTH];


	if (!HEAP_length(minheap)) {
		print(MSG_CONN_PIC_KO);
		return createFrame(CODE_CONNECT, HEADER_PIC_CON_KO, NULL);
	}

	HEAP_print(minheap);
	ent = HEAP_consulta(&minheap);
	HEAP_print(minheap);

	if (ent.time < 0){
		print(MSG_CONN_PIC_KO);
		return createFrame(CODE_CONNECT, HEADER_PIC_CON_KO, NULL);
	}

	sprintf(aux, MSG_CONN_PIC_OK, ent.name);
	print(aux);

	memset(aux, '\0', LENGTH);
	sprintf(aux, "%s&%s&%d", ent.name, ent.ip, ent.port);

	return createFrame(CODE_CONNECT, HEADER_PIC_CON_OK, aux);
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {
	if (sock_picard > 0)
		close(sock_picard);
	if (sock_enterprise > 0)
		close(sock_enterprise);
	free(config.ip);
}

/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint() {
	freeResources();
	HEAP_close(&minheap);
	exit(EXIT_SUCCESS);
}
#include "logica.h"

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
 * Funció d'escolta del socket. Bloqueja l'execució esperant que un client es connecti al seu port. No és dedicat (de moment almenys).
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
			HEAP_print(minheap);
		} else return;

	}
}

char updateEnterprise(const char *data) {
	int i, j;
	int port;
	int users;
	char aux[LENGTH];

	for (i = 0; data[i] != '&'; i++) {
		aux[i] = data[i];
	}
	aux[i] = '\0';
	port = atoi(aux); // NOLINT

	for (j = 0, i++; data[i]; i++, j++) {
		aux[j] = data[i];
	}
	aux[j] = '\0';
	users = atoi(aux); // NOLINT

	return HEAP_update(&minheap, port, users);
}

char disconnectEnterprise(const char *data) {
	int i;
	char aux[LENGTH];

	for (i = 0; data[i]; i++) {
		aux[i] = data[i];
	}
	aux[i] = '\0';

	return HEAP_disconnect(&minheap, atoi(aux));  // NOLINT
}

/**
 * Funció per executar les peticions dels clients a partir del camp type de la trama.
 * De moment només rep connexions de Picard.
 *
 * @param sock 	Socket de la connexió amb el client
 */
void attendPetition(int sock) {        //TODO: Gestionar connexió Enterprise
	Frame frame;
	char answer = 0;

	frame = readFrame(sock);
	debugFrame(frame);

	switch (frame.type) {
		case CODE_CONNECT:
			connectSocket(sock, frame);
			break;

		case CODE_UPDATE:
			answer = updateEnterprise(frame.data);
			free(frame.data);
			frame = createFrame(CODE_UPDATE, answer ? "UPDATEOK" : "UPDATEKO", NULL);
			sendFrame(sock, frame);
			free(frame.data);
			break;

		case CODE_DISCONNECT:
			answer = disconnectEnterprise(frame.data);
			free(frame.data);
			frame = createFrame(CODE_DISCONNECT, answer ? "CONOK" : "CONKO", NULL);
			sendFrame(sock, frame);
			free(frame.data);
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
		ent.name = realloc(ent.name, sizeof(char) * (i + 1));
	}
	ent.name[i] = '\0';
	j++;

	ent.ip = malloc(sizeof(char));
	for (i = 0; data[j] != '&'; j++) {
		ent.ip[i] = data[j];
		i++;
		ent.ip = realloc(ent.ip, sizeof(char) * (i + 1));
	}
	ent.ip[i] = '\0';
	j++; //saltem &

	aux = malloc(sizeof(char));
	for (i = 0; data[j]; j++) {
		aux[i] = data[j];
		i++;
		aux = realloc(aux, sizeof(char) * (i + 1));
	}
	aux[i] = '\0';
	ent.port = atoi(aux); // NOLINT
	free(aux);

	ent.users = 0;

	return ent;
}

/**
 * Funció per tractar les peticions de connexió. De moment només en rep per part dels Picards.
 *
 * @param sock 		Socket de connexió amb el client
 * @param frame 	Trama rebuda del client amb la seva informació.
 */
void connectSocket(int sock, Frame frame) {
	char *name;
	char aux[LENGTH];
	Enterprise ent;

	if (!strcmp(frame.header, HEADER_PIC_DATA_CONNECT)) {
		name = malloc(sizeof(char) * (strlen(frame.data) + 1));
		strcpy(name, frame.data);
		sprintf(aux, MSG_CONNECT_PIC, name);
		print(aux);
		free(frame.data);

		frame = getEnterpriseConnection();
		debugFrame(frame);
		sendFrame(sock, frame);

		sprintf(aux, MSG_DISCONNECT_PIC, name);
		print(aux);

		free(frame.data);
		free(name);
	} else {
		print("PETICIÓ D'ENTERPRISE!\n");
		print(frame.data);
		print("\n");

		ent = parseEnterprise(frame.data);
		HEAP_push(&minheap, ent);

		free(frame.data);
		frame = createFrame(CODE_CONNECT, "CONOK", NULL);
		sendFrame(sock, frame);
		free(frame.data);
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

	//TODO: Obtenir Enterprise de la PQ
	//Actualitzar-li el valor i inserir-lo
	if (!HEAP_length(minheap)) {
		return createFrame(CODE_CONNECT, HEADER_DATA_PIC_CON_KO, NULL);
	}

	ent = HEAP_pop(&minheap);

	ent.users++;

	HEAP_push(&minheap, ent);
	memset(aux, '\0', LENGTH);
	sprintf(aux, "%s&%s&%d", ent.name, ent.ip, ent.port);

	return createFrame(CODE_CONNECT, HEADER_DATA_PIC_CON_OK, aux);
}

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources() {
	if (sock_picard > 0)
		close(sock_picard);
	if (sock_enterprise > 0)
		close(sock_enterprise);
	if (config.ip != NULL)
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
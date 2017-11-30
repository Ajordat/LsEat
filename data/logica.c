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
		} else return;

	}
}

/*
void listenSocket(int sock) {
	struct sockaddr_in addr;
	socklen_t addr_len;
	char aux[LENGTH];
	int new_sock;

	print(MSG_WAITING);

	listen(sock, MAX_REQUESTS);

	addr_len = sizeof(addr);

	while (1) {

		if ((new_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
			print(MSG_CONEX_ERR);
			freeResources();
			exit(EXIT_FAILURE);
		}

		attendPetition(new_sock);
		close(new_sock);
	}
}
 */

/**
 * Funció per executar les peticions dels clients a partir del camp type de la trama.
 * De moment només rep connexions de Picard.
 *
 * @param sock 	Socket de la connexió amb el client
 */
void attendPetition(int sock) {        //TODO: Gestionar connexió Enterprise
	Frame frame;

	frame = readFrame(sock);
	debugFrame(frame);

	switch (frame.type) {
		case CODE_CONNECT:
			connectSocket(sock, frame);
			break;

		case CODE_DISCONNECT:
			break;

		case CODE_UPDATE:
			break;

		default:
			break;

	}
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
		//TODO: Tractar petició Enterprise
		//Inserir a la PQ
		print("PETICIÓ D'ENTERPRISE!\n");
		print(frame.data);
		print("\n");

		free(frame.data);
		frame = createFrame(CODE_CONNECT, "CONOK", NULL);
		sendFrame(sock, frame);
		free(frame.data);
	}
}

/**
 * Funció temporal per simular el resultat de si el Picard es pot connectar o no.
 *
 * @return 	L'estat resultant de la connexió
 */
char getConnectionStatus() { return CONNECT_STATUS; }

/**
 * Genera la trama de resposta al client amb la direcció ip i port d'un Enterprise, si tot ha anat bé.
 *
 * @return 	Trama a respondre al client
 */
Frame getEnterpriseConnection() {

	//TODO: Obtenir Enterprise de la PQ
	//Actualitzar-li el valor i inserir-lo

	return getConnectionStatus() ?
		   createFrame(CODE_CONNECT, HEADER_DATA_PIC_CON_OK, HARDCODED_ENTERPRISE) :
		   createFrame(CODE_CONNECT, HEADER_DATA_PIC_CON_KO, NULL);
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
	exit(EXIT_SUCCESS);
}
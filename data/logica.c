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
		sprintf(msg, "Error a l'obrir el fitxer %s.\n", filename);
		print(msg);
		exit(EXIT_FAILURE);
	}
	debug("Tot bé!\n");

	config.ip = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.port_picard = atoi(aux);
	free(aux);

	aux = readFileDescriptor(file);
	config.port_enterprise = atoi(aux);
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
void listenSocket(int sock) {
	struct sockaddr_in addr;
	socklen_t addr_len;
	char aux[LENGTH];
	int new_sock;

	print("Esperant clients...\n");

	listen(sock, MAX_REQUESTS);

	addr_len = sizeof(addr);

	while (1) {

		if ((new_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
			sprintf(aux, "Error a l'establir connexió. 3\n");
			write(1, aux, strlen(aux));
			freeResources();
			exit(EXIT_FAILURE);
		}

		attendPetition(new_sock);

		close(new_sock);

	}

}

/**
 * Funció per executar les peticions dels clients a partir del camp type de la trama.
 * De moment només rep connexions de Picard.
 *
 * @param sock 	Socket de la connexió amb el client
 */
void attendPetition(int sock) {
	Frame frame;

	frame = readFrame(sock);
	debugFrame(frame);

	switch (frame.type) {
		case CODE_CONNECT:
			connectSocket(sock, frame);

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

	if (!strcmp(frame.header, "PIC_NAME")) {
		name = malloc(sizeof(char) * (strlen(frame.data) + 1));
		strcpy(name, frame.data);
		sprintf(aux, "Connectant %s\n", name);
		print(aux);
		free(frame.data);

		frame = getEnterpriseConnection();
		debugFrame(frame);
		sendFrame(sock, frame);

		sprintf(aux, "Desconnectant %s\n", name);
		print(aux);

		free(name);
	} else {
		//TODO: Tractar petició Enterprise
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
	Frame frame;

	frame.type = CODE_CONNECT;

	memset(frame.header, '\0', HEADER_SIZE);

	if (getConnectionStatus()) {
		sprintf(frame.header, "ENT_INF");
		frame.data = malloc(sizeof("Enterprise A&127.0.0.1&8491"));
		memset(frame.data, '\0', sizeof("Enterprise A&127.0.0.1&8491"));
		sprintf(frame.data, "Enterprise A&127.0.0.1&8491");
		frame.length = (short) strlen(frame.data);
		char aux[LENGTH];
		sprintf(aux, "[MSG] -> |%s|\n[LENGTH] -> %i\n", frame.data, frame.length);
		debug(aux);
	} else {
		sprintf(frame.header, "CONKO");
		frame.length = 0;
		frame.data = malloc(sizeof(char));
		frame.data[0] = '\0';
	}

	return frame;
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
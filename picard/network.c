#include "network.h"

/**
 * Funció per obtenir un socket de client a partir d'una direcció ip i un port.
 *
 * @param ip 	Direcció ip del socket
 * @param port 	Port del socket
 * @return 		0 si ha pogut obrir el socket. Altrament, -1
 */
char getSocket(char *ip, int port) {
	char aux[LENGTH];
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0) {
		sprintf(aux, "Error a l'establir connexió.\n%s\n", strerror(errno));
		write(1, aux, strlen(aux));
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (connect(sock, (void *) &addr, sizeof(addr)) < 0) {
		sprintf(aux, "Error a l'establir connexió. 2\n%s\n", strerror(errno));
		write(1, aux, strlen(aux));
		return -1;
	}

	return 0;
}

/**
 * Funció per enviar a un Enterprise una sol·licitud per connectar-s'hi. Crea una trama, l'envia i en rep la resposta.
 *
 * @param name		Nom del picard
 * @param money		Diners del picard
 * @return			0 si la sol·licitud ha anat bé. Altrament, !0
 */
char connectEnterprise(char *name, int money) {
	Frame frame;
	char aux[LENGTH];

	debug("[MAKING FRAME]\n");
	frame.type = CODE_CONNECT;
	memset(frame.header, '\0', HEADER_SIZE * sizeof(char));
	sprintf(frame.header, "PIC_INF");

	myItoa(money, aux);
	frame.data = malloc((strlen(name) + 2 + strlen(aux)) * sizeof(char));
	strcpy(frame.data, name);
	frame.data[strlen(frame.data) + 1] = '\0';
	frame.data[strlen(frame.data)] = '&';
	strcat(frame.data, aux);

	frame.length = (short) strlen(frame.data);
	sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
	debug(aux);
	debug("[SENDING]\n");
	sendFrame(frame);

	debug("[SENT]\n");
	free(frame.data);

	debug("[READING FRAME]\n");
	frame = readFrame();
	debugFrame(frame);

	if (frame.data != NULL)
		free(frame.data);

	return (char) strcmp(frame.header, "CONOK");
}

/**
 * Funció per demanar a Data un Enterprise al que connectar-se.
 *
 * @param name	Nom del picard
 * @return		Resposta de Data
 */
Frame establishConnection(char *name) {
	Frame frame;
	char aux[LENGTH];

	frame.type = CODE_CONNECT;
	memset(frame.header, '\0', HEADER_SIZE * sizeof(char));
	sprintf(frame.header, "PIC_NAME");

	frame.data = malloc(sizeof(name));
	strcpy(frame.data, name);
	frame.length = (short) strlen(frame.data);
	sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
	debug(aux);

	sendFrame(frame);

	free(frame.data);

	frame = readFrame();

	return frame;
}

/**
 * Funció per desconnectar el client Picard del servidor Enterprise.
 *
 * @param name 	Nom del Picard
 */
void disconnect(char *name) {
	Frame frame;
	char aux[LENGTH];

	frame.type = CODE_DISCONNECT;
	memset(frame.header, '\0', HEADER_SIZE * sizeof(char));
	sprintf(frame.header, "PIC_NAME");

	frame.data = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(frame.data, name);
	frame.length = (short) strlen(frame.data);
	sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
	debug(aux);

	sendFrame(frame);

	free(frame.data);

	frame = readFrame();

	debugFrame(frame);
	free(frame.data);

	if (!strcmp(frame.header, "CONOK"))
		print("[Desconnecta Enterprise OK]\n");
	else
		print("[Desconnecta Enterprise KO]\n");
}

/**
 * Funció per enviar una trama.
 *
 * @param sock 		Socket de connexió amb el client.
 * @param frame 	Trama a enviar
 */
void sendFrame(Frame frame) {
	char *trama;

	trama = malloc(sizeof(char) * (1 + HEADER_SIZE + 2 + strlen(frame.data) + 1));
	memset(trama, '\0', sizeof(char) * (1 + HEADER_SIZE + 2 + strlen(frame.data)+1));
	trama[0] = frame.type;
	memcpy(trama + sizeof(char), frame.header, HEADER_SIZE * sizeof(char));
	trama[HEADER_SIZE + 1] = (char) (((frame.length >> 8)) & 0x00FF);
	trama[HEADER_SIZE + 2] = (char) (frame.length & 0x00FF);
//	trama[HEADER_SIZE + 1] = (char) ((((frame.length >> 8)) & 0x00FF) + '0');
//	trama[HEADER_SIZE + 2] = (char) ((frame.length & 0x00FF) + '0');
	memcpy(trama + sizeof(char) * (HEADER_SIZE + 3), frame.data, (strlen(frame.data) + 1) * sizeof(char));
	write(sock, trama, sizeof(char) * (1 + HEADER_SIZE + 2 + strlen(frame.data)));
	debug(trama);
	free(trama);
}

/**
 * Funció per rebre una trama.
 *
 * @param sock 	Socket de lectura de la trama
 * @return 		Trama llegida
 */
Frame readFrame() {
	Frame frame;
	char length[2];

	debug("readFrame()\n");

	//Lectura del camp type. 1 byte
	read(sock, &frame.type, sizeof(char));

	//Lectura del camp header. 10 bytes
	read(sock, frame.header, HEADER_SIZE * sizeof(char));

	//Lectura del camp length. 2 bytes. Es llegeix un array de dos caràcters i es volquen en una variable
	//de tipus short per facilitar el tractament.
	read(sock, length, 2 * sizeof(char));
	frame.length = (short) ((length[0] << 8) & 0xFF00);
	frame.length = (frame.length | length[1]);
//	frame.length = (short) (length[0] - '0');
//	frame.length = (short) ((frame.length << 8) & 0xFF00);
//	frame.length = (short) (frame.length | (length[1] - '0'));

	//Lectura del camp data segons la llargada indicada al camp length. Mida variable
	frame.data = malloc(sizeof(char) * (frame.length + 1));
	memset(frame.data, '\0', sizeof(char) * (frame.length + 1));
	read(sock, frame.data, (size_t) frame.length);
	frame.data[frame.length] = '\0';

	return frame;
}

/**
 * Funció per mostrar el contingut d'una trama. Només mostra l'output si DEBUG és diferent a 0.
 *
 * @param frame 	Trama a mostrar
 */
void debugFrame(Frame frame) {
	char aux[LENGTH];
	sprintf(aux, "|%d|%s|%i|%s|\n", frame.type, frame.header, frame.length, frame.data);
	debug(aux);
}
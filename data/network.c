#include "network.h"


/**
 * Funció per obtenir un socket de servidor a partir d'una direcció ip i un port.
 *
 * @param ip 	Direcció ip del socket
 * @param port 	Port del socket
 * @return 		File descriptor amb un socket per fer de servidor o -1 si no l'ha pogut obrir
 */
int createSocket(char *ip, int port) {
	struct sockaddr_in addr;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (bind(sock, (void *) &addr, sizeof(addr)) < 0) {
		return -1;
	}
	return sock;
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

/**
 * Funció per enviar una trama.
 *
 * @param sock 		Socket de connexió amb el client.
 * @param frame 	Trama a enviar
 */
char sendFrame(int socket, Frame frame) {
	char *trama;
	ssize_t result;

	//Petició de memòria per la trama a enviar. La seva mida és la suma de les mides dels diferents camps.
	trama = malloc(1 + HEADER_SIZE + 2 + strlen(frame.data) + 1);
	memset(trama, '\0', 1 + HEADER_SIZE + 2 + strlen(frame.data) + 1);

	//Escriptura del camp type. 1 byte.
	trama[0] = frame.type;

	//Escriptura del camp header. 10 bytes.
	memcpy(trama + 1, frame.header, HEADER_SIZE);

	//Escriptura del camp length. 2 bytes.
	trama[HEADER_SIZE + 1] = (char) (((frame.length >> 8)) & 0x00FF);
	trama[HEADER_SIZE + 2] = (char) (frame.length & 0x00FF);

	//Escriptura de la informació de la trama. Mida variable segons el camp length.
	memcpy(trama + HEADER_SIZE + 3, frame.data, strlen(frame.data) + 1);

	//Enviament de la trama.
	result = write(socket, trama, 1 + HEADER_SIZE + 2 + strlen(frame.data));

	//Debug frame
	trama[0] += '0';
	trama[strlen(trama)] = '\n';
	debug(trama);

	//Alliberament de la trama enviada.
	free(trama);

	return result > 0;
}

/**
 * Funció per rebre una trama.
 *
 * @param sock 	Socket de lectura de la trama
 * @return 		Trama llegida
 */
Frame readFrame(int sock) {
	Frame frame;
	char length[2];
	ssize_t resp;

	//Lectura del camp type. 1 byte.
	resp = read(sock, &frame.type, sizeof(char));

	if (resp <= 0) {
		frame.type = FRAME_NULL;
		return frame;
	}

	//Lectura del camp header. 10 bytes.
	read(sock, frame.header, HEADER_SIZE);

	//Lectura del camp length. 2 bytes. Es llegeix un array de dos caràcters i es volquen en una variable
	//de tipus short per facilitar el tractament.
	read(sock, length, (size_t) 2);
	frame.length = (short) (((length[0] << 8) & 0xFF00) | (length[1] & 0x00FF));

	//Lectura del camp data segons la llargada indicada al camp length. Mida variable.
	frame.data = malloc((size_t) frame.length + 1);
	memset(frame.data, '\0', (size_t) frame.length + 1);
	read(sock, frame.data, (size_t) frame.length);
	frame.data[frame.length] = '\0';

	return frame;
}

/**
 * Funció per crear un Frame a utilitzant les dades dels paràmetres.
 *
 * @param type 		Camp type de la trama
 * @param header 	Camp header de la trama
 * @param data 		Camp data de la trama
 * @return 			Frame creat
 */
Frame createFrame(char type, char *header, char *data) {
	Frame frame;

	frame.type = type;
	memset(frame.header, '\0', HEADER_SIZE);
	strcpy(frame.header, header);

	if (data == NULL) {
		frame.length = 0;
		frame.data = malloc(sizeof(char));
		memset(frame.data, '\0', sizeof(char));
	} else {
		frame.data = malloc(strlen(data) + 1);
		strcpy(frame.data, data);
		frame.length = (short) strlen(frame.data);
	}
	return frame;
}

/**
 * Funció per a eliminar el contingut d'una trama. D'aquesta manera, quan s'acaba d'utilitzar un frame es pot
 * destruir i reutilitzar alliberant tota la memòria que requereix.
 *
 * @param frame 	Trama a destruir
 */
void destroyFrame(Frame *frame) {
	free((*frame).data);
}

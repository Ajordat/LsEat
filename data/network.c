#include "network.h"


/**
 * Funció per obtenir un socket de servidor a partir d'una direcció ip i un port.
 *
 * @param ip 	Direcció ip del socket
 * @param port 	Port del socket
 * @return 		File descriptor amb un socket per fer de servidor o -1 si no l'ha pogut obrir
 */
int createSocket(char *ip, int port) {
	char aux[LENGTH];
	struct sockaddr_in addr;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		sprintf(aux, "Error a l'establir connexió. 1\n");
		print(aux);
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (bind(sock, (void *) &addr, sizeof(addr)) < 0) {
		sprintf(aux, "Error a l'establir connexió. 2\n%s\n", strerror(errno));
		print(aux);
		return -1;
	}
	return sock;
}

/**
 * Funció per mostrar el contingut d'una trama. Mostra l'output si DEBUG és diferent a 0.
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
void sendFrame(int sock, Frame frame) {
	char aux[LENGTH];
	char length[2];

	//Escriptura del camp type. 1 byte
	sprintf(aux, "%d", frame.type);
	write(sock, aux, sizeof(char));

	//Escriptura del camp header. 10 bytes
	write(sock, frame.header, HEADER_SIZE * sizeof(char));

	//Escriptura del camp length. 2 bytes. La idea original era enviar un short directament.
	//No va funcionar i ara envia els dos bytes del short en un array de dos caràcters.
	length[0] = (char) ((((frame.length >> 8)) & 0x00FF) + '0');
	length[1] = (char) ((frame.length & 0x00FF) + '0');
	write(sock, length, 2 * sizeof(char));

	//Escriptura de la informació de la trama. Mida variable segons el camp length
	write(sock, frame.data, strlen(frame.data));
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

	//Lectura del camp type. 1 byte
	read(sock, &frame.type, sizeof(char));
	frame.type -= '0';

	//Lectura del camp header. 10 bytes
	read(sock, frame.header, HEADER_SIZE * sizeof(char));

	//Lectura del camp length. 2 bytes. Es llegeix un array de dos caràcters i es volquen en una variable
	//de tipus short per facilitar el tractament.
	read(sock, length, 2 * sizeof(char));
	frame.length = (short) (length[0] - '0');
	frame.length = (short) ((frame.length << 8) & 0xFF00);
	frame.length = (short) (frame.length | (length[1] - '0'));

	//Lectura del camp data segons la llargada indicada al camp length. Mida variable
	frame.data = malloc(sizeof(char) * (frame.length + 1));
	memset(frame.data, '\0', sizeof(char) * (frame.length + 1));
	read(sock, frame.data, (size_t) frame.length);
	frame.data[frame.length] = '\0';

	return frame;
}


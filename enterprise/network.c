#include "network.h"

int createSocket(char *ip, int port) {
	char aux[LENGTH];
	struct sockaddr_in addr;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0) {
		sprintf(aux, "Error a l'establir connexió. 1\n");
		write(1, aux, strlen(aux));
		return -1;
//		exit(EXIT_FAILURE);         //TODO: Alliberar recursos
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (bind(sock, (void *) &addr, sizeof(addr)) < 0) {
		sprintf(aux, "Error a l'establir connexió. 2\n%s\n", strerror(errno));
		write(1, aux, strlen(aux));
		return -1;
//		exit(EXIT_FAILURE);         //TODO: Alliberar recursos
	}

	sprintf(aux, "[Creating socket] -> %d\n", port);
	debug(aux);
	return sock;
}


void debugFrame(Frame frame) {
	char aux[LENGTH];
	if (DEBUG) {
		sprintf(aux, "|%hd|%s|%hd|%s|\n", frame.type, frame.header, frame.length, frame.data);
		print(aux);
	}
}

void sendFrame(int sock, Frame frame) {
	char aux[10];
	char length[2];

	sprintf(aux, "%d", frame.type);
	write(sock, aux, sizeof(char));
	write(sock, frame.header, HEADER_SIZE * sizeof(char));
//	sprintf(aux, "%i", frame.length);
//	write(sock, aux, sizeof(short));
	length[0] = (char) ((((frame.length >> 8)) & 0x00FF) + '0');
	length[1] = (char) ((frame.length & 0x00FF) + '0');
	write(sock, length, 2 * sizeof(char));
	write(sock, frame.data, strlen(frame.data));
}

Frame readFrame(int sock) {
	Frame frame;
	char length[2];

	read(sock, &frame.type, sizeof(char));
	frame.type -= '0';

	read(sock, frame.header, HEADER_SIZE * sizeof(char));

//	read(sock, &frame.length, sizeof(short));
//	frame.length -= '0';

	read(sock, length, 2 * sizeof(char));
	frame.length = (short) (length[0] - '0');
	frame.length = (short) ((frame.length << 8) & 0xFF00);
	frame.length = (short) (frame.length | (length[1] - '0'));

	frame.data = malloc(sizeof(char) * (frame.length + 1));
	memset(frame.data, '\0', sizeof(char) * (frame.length + 1));
	read(sock, frame.data, (size_t) frame.length);

	char aux[LENGTH];
	sprintf(aux, "[SIZEOF] -> %d\n", (int) sizeof(frame.data));
	debug(aux);
	sprintf(aux, "[LENGTH] -> %d\n", (int) frame.length);
	debug(aux);
	return frame;
}

Frame createFrame(char type, char * header, char * data){
	Frame frame;
	frame.type = type;
	memset(frame.header, '\0', HEADER_SIZE * sizeof(char));
	strcpy(frame.header, header);

	if (data == NULL){
		frame.length = 0;
		frame.data = NULL;
	} else {
		frame.data = malloc(sizeof(char) * strlen(data));
		strcpy(frame.data, data);
		frame.length = (short) strlen(frame.data);
	}
	return frame;
}

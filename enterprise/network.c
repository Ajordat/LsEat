#include "network.h"

int createSocket(char *ip, int port) {
	char aux[LENGTH];
	struct sockaddr_in addr;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0) {
		sprintf(aux, "Error a l'establir connexió. 1\n");
		write(1, aux, strlen(aux));
		exit(EXIT_FAILURE);         //TODO: Alliberar recursos
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (bind(sock, (void *) &addr, sizeof(addr)) < 0) {
		sprintf(aux, "Error a l'establir connexió. 2\n%s\n", strerror(errno));
		write(1, aux, strlen(aux));
		exit(EXIT_FAILURE);         //TODO: Alliberar recursos
	}
	print("[Creating socket] -> ");
	printi(port);
	return sock;
}


void printFrame(Frame frame) {
	char aux[LENGTH];

	sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
	write(1, aux, strlen(aux));
}

void sendFrame(int sock, Frame frame) {
	char aux[10];

	sprintf(aux, "%d", frame.type);
	write(sock, aux, sizeof(char));
	write(sock, frame.header, 10 * sizeof(char));
	sprintf(aux, "%d", frame.length);
	write(sock, aux, sizeof(short));
	write(sock, frame.data, strlen(frame.data));
}

Frame readFrame(int sock) {
	Frame frame;

	read(sock, &frame.type, sizeof(char));
	frame.type -= '0';

	read(sock, frame.header, 10 * sizeof(char));

	read(sock, &frame.length, sizeof(short));
	frame.length -= '0';

	frame.data = malloc(sizeof(char) * (frame.length + 1));
	memset(frame.data, '\0', sizeof(char) * (frame.length + 1));
	read(sock, frame.data, (size_t) frame.length);

	return frame;
}

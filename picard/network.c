#include <sys/socket.h>
#include "network.h"


void getSocket(char * ip, int port) {
    char aux[LENGTH];
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0) {
        sprintf(aux, "Error a l'establir connexió.\n");
        write(1, aux, strlen(aux));
        exit(EXIT_FAILURE);         //TODO: Alliberar recursos
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t) port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (void *) &addr, sizeof(addr)) < 0) {
        sprintf(aux, "Error a l'establir connexió.\n");
        write(1, aux, strlen(aux));
        exit(EXIT_FAILURE);         //TODO: Alliberar recursos
    }

}

void resolveEnterprise(Frame frame) {

	int i, j;
	char *aux;

	j = 0;
	enterprise.name = malloc(sizeof(char));
	for(i = 0; frame.data[j] != '&'; i++) {
		enterprise.name[i] = frame.data[j];
		realloc(enterprise.name, sizeof(char) * (i+1));
		j++;
	}
	enterprise.name[i] = '\0';
	j++; //saltem &

	enterprise.ip = malloc(sizeof(char));
	for(i = 0; frame.data[j] != '&'; i++) {
		enterprise.ip[i] = frame.data[j];
		realloc(enterprise.ip, sizeof(char) * (i+1));
		j++;
	}
	enterprise.ip[i] = '\0';
	j++; //saltem &

	aux = malloc(sizeof(char));
	for(i = 0; frame.data[j] != '&'; i++) {
		aux[i] = frame.data[j];
		realloc(aux, sizeof(char) * (i+1));
		j++;
	}
	aux[i] = '\0';
	enterprise.port = atoi(aux);
	free(aux);

}


void establishConnection(char * name) {
    Frame frame;
    char aux[LENGTH];

    frame.type = 0x01;
    memset(frame.header, '\0', 10);
    sprintf(frame.header, "PIC_NAME");

    frame.data = malloc(sizeof(name));
//    frame.data = malloc(sizeof(config.name)+1);
    strcpy(frame.data, name);
//    frame.data[strlen(config.name)] = '\0';

    frame.length = (short) strlen(frame.data);
    sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
    debug(aux);

    sendFrame(frame);

    free(frame.data);

    frame = readFrame();

	resolveEnterprise(frame);

    printFrame(frame);
    free(frame.data);
    close(sock);

    //TODO: CONNEXIÓ AMB ENTERPRISE

}

void sendFrame(Frame frame) {
    char aux[100];

    sprintf(aux, "%d", frame.type);
    write(sock, aux, sizeof(char));
    write(sock, frame.header, 10 * sizeof(char));
    sprintf(aux, "%d", frame.length);
    write(sock, aux, sizeof(short));
    sprintf(aux, "%s", frame.data);
    write(sock, aux, strlen(aux));
    debug("sendFrame()\n");
}

Frame readFrame() {
    Frame frame;

    read(sock, &frame.type, sizeof(char));
    frame.type -= '0';
    read(sock, frame.header, 10 * sizeof(char));
    read(sock, &frame.length, sizeof(short));
    frame.length -= '0';
    frame.data = malloc(sizeof(char) * (frame.length + 1));
    read(sock, frame.data, (size_t) frame.length);
    frame.data[frame.length] = '\0';

    return frame;
}

void printFrame(Frame frame) {
    char aux[LENGTH + 15];

    sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
    write(1, aux, strlen(aux));
}
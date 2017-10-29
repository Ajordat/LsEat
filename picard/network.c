#include <sys/socket.h>
#include "network.h"


void getSocket(char *ip, int port) {
	char aux[LENGTH];
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0) {
		sprintf(aux, "Error a l'establir connexió.\n%s\n", strerror(errno));
		write(1, aux, strlen(aux));
		exit(EXIT_FAILURE);         //TODO: Alliberar recursos
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (connect(sock, (void *) &addr, sizeof(addr)) < 0) {
		sprintf(aux, "Error a l'establir connexió. 2\n%s\n", strerror(errno));
		write(1, aux, strlen(aux));
		exit(EXIT_FAILURE);         //TODO: Alliberar recursos
	}

}

void resolveEnterprise(Frame frame) {
	int i, j, port;
	char *aux, *ip;


	for (j = 0; frame.data[j] != '&'; j++);
	j++;

	ip = malloc(sizeof(char));
	for (i = 0; frame.data[j] != '&'; j++) {
		ip[i] = frame.data[j];
		i++;
		ip = realloc(ip, sizeof(char) * (i + 1));
	}
	ip[i] = '\0';
	j++; //saltem &

	aux = malloc(sizeof(char));
	for (i = 0; frame.data[j]; j++) {
		aux[i] = frame.data[j];
		i++;
		aux = realloc(aux, sizeof(char) * (i + 1));
	}
	aux[i] = '\0';
	port = atoi(aux);
	free(aux);

	printf("%s-%d\n", ip, port);    //TODO: PRINTF

	getSocket(ip, port);    //Actualment, si no pot establir la connexió es tanca el programa.
	free(ip);                //S'ha de canviar per recollir l'error i alliberar els recursos i tancar,
	//o bé intentar recuperar-se de l'error havent de tornar a demanar a Data (opció correcta)

}

void connectEnterprise(char *name, int money) {
	Frame frame;
	char aux[LENGTH];

	debug("[MAKING FRAME]\n");
	frame.type = 0x01;
	memset(frame.header, '\0', 10);
	sprintf(frame.header, "PIC_INF");

	myItoa(money, aux);
	frame.data = malloc(sizeof(name) + 1 + strlen(aux) * sizeof(char));
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
	debug("[DONE]\n");

	printFrame(frame);
	free(frame.data);
}

void establishConnection(char *name, int money) {
	Frame frame;
	char aux[LENGTH];

	frame.type = 0x01;
	memset(frame.header, '\0', 10);
	sprintf(frame.header, "PIC_NAME");

	frame.data = malloc(sizeof(name));
//	frame.data = malloc(sizeof(config.name)+1);
	strcpy(frame.data, name);
//	frame.data[strlen(config.name)] = '\0';

	frame.length = (short) strlen(frame.data);
	sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
	debug(aux);

	sendFrame(frame);

	free(frame.data);

	frame = readFrame();


	printFrame(frame);
	close(sock);

	//TODO: CONNEXIÓ AMB ENTERPRISE
	debug("[STARTING ENTERPRISE CONNECTION]\n");
	resolveEnterprise(frame);
	debug("[DONE]\n");
	free(frame.data);
	connectEnterprise(name, money);    //HARDCODED

}

void sendFrame(Frame frame) {
	char aux[100];
	char length[2];

	sprintf(aux, "%d", frame.type);
	write(sock, aux, sizeof(char));

	write(sock, frame.header, 10 * sizeof(char));

	length[0] = (char) ((((frame.length >> 8)) & 0x00FF) + '0');
	length[1] = (char) ((frame.length & 0x00FF) + '0');
	write(sock, length, 2 * sizeof(char));
//	sprintf(aux, "%i", frame.length);
//	write(sock, aux, sizeof(short));

//	write(sock, &frame.length, sizeof(short));

	sprintf(aux, "%s", frame.data);

	write(sock, aux, strlen(aux));

	debug("sendFrame()\n");
}

Frame readFrame() {
	Frame frame;
	char length[2];

	debug("readFrame()\n");

	read(sock, &frame.type, sizeof(char));
	frame.type -= '0';

	read(sock, frame.header, 10 * sizeof(char));

//	read(sock, &frame.length, sizeof(short));
//	frame.length -= '0';
	read(sock, length, 2 * sizeof(char));
	frame.length = (short) (length[0] - '0');
	frame.length = (short) ((frame.length << 8) & 0xFF00);
	frame.length = (short) (frame.length | (length[1] - '0'));


	frame.data = malloc(sizeof(char) * (frame.length + 1));
	memset(frame.data, '\0', sizeof(char) * (frame.length + 1));
	read(sock, frame.data, (size_t) frame.length);
	frame.data[frame.length] = '\0';
	/*read(sock, &frame.type, sizeof(char));
	frame.type -= '0';

	memset(frame.header, '\0', 10);
	read(sock, frame.header, 10 * sizeof(char));

	read(sock, &frame.length, sizeof(short));

	frame.data = malloc(sizeof(char) * (frame.length + 1));
	memset(frame.data, '\0', sizeof(char) * (frame.length + 1));
	read(sock, frame.data, (size_t) frame.length);
	frame.data[frame.length] = '\0';*/

	return frame;
}

void printFrame(Frame frame) {
	char aux[LENGTH + 15];
	debug("printFrame()\n");
//	memset(aux, '\0', LENGTH + 15);
	sprintf(aux, "|%d|%s|%d|%s|\n", frame.type, frame.header, frame.length, frame.data);
	print(aux);
}
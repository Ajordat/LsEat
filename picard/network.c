#include "network.h"


/**
 * Funció per enviar a un Enterprise una sol·licitud per connectar-s'hi. Crea una trama, l'envia i en rep la resposta.
 *
 * @param name		Nom del picard
 * @param money		Diners del picard
 * @return			0 si la sol·licitud ha anat bé. Altrament, !0
 */
char connectEnterprise(char *name, int money) {
	Frame frame;
	char aux[LENGTH], *data;

	debug("[MAKING FRAME]\n");
	myItoa(money, aux);
	data = malloc((strlen(name) + 2 + strlen(aux)) * sizeof(char));
	strcpy(data, name);
	data[strlen(data) + 1] = '\0';
	data[strlen(data)] = '&';
	strcat(data, aux);

	frame = createFrame(CODE_CONNECT, "PIC_INF", data);
	debugFrame(frame);

	debug("[SENDING]\n");
	writeFrame(frame);

	debug("[SENT]\n");
	free(frame.data);
	free(data);

	debug("[READING FRAME]\n");
	frame = readFrame();
	debugFrame(frame);

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

	frame = createFrame(CODE_CONNECT, "PIC_NAME", name);
	debugFrame(frame);

	writeFrame(frame);

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

	frame = createFrame(CODE_DISCONNECT, "PIC_NAME", name);
	debugFrame(frame);

	writeFrame(frame);

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
 * @param frame 	Trama a enviar
 */
void writeFrame(Frame frame) {
	sendFrame(sock, frame);
}

/**
 * Funció per rebre una trama.
 *
 * @return 		Trama llegida
 */
Frame readFrame() {
	return receiveFrame(sock);
}

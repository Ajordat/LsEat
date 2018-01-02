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
	destroyFrame(&frame);
	free(data);

	debug("[READING FRAME]\n");
	frame = readFrame();
	debugFrame(frame);

	destroyFrame(&frame);

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

	destroyFrame(&frame);

	frame = readFrame();

	return frame;
}

/**
 * Funció per desconnectar el client Picard del servidor Enterprise.
 *
 * @param name 	Nom del Picard
 */
char disconnect(char *name) {
	Frame frame;
	char resp;

	frame = createFrame(CODE_DISCONNECT, "PIC_NAME", name);
	debugFrame(frame);

	resp = writeFrame(frame);

	destroyFrame(&frame);

	if (!resp)
		return 0;

	frame = readFrame();
	if (frame.type == FRAME_NULL) {
		return 0;
	}

	debugFrame(frame);
	destroyFrame(&frame);

	if (!strcmp(frame.header, "CONOK"))
		print("[Desconnecta Enterprise OK]\n");
	else
		print("[Desconnecta Enterprise KO]\n");
	return 1;
}

/**
 * Funció per enviar una trama.
 *
 * @param frame 	Trama a enviar
 */
char writeFrame(Frame frame) {
	return sendFrame(sock, frame);
}

/**
 * Funció per rebre una trama.
 *
 * @return 		Trama llegida
 */
Frame readFrame() {
	return receiveFrame(sock);
}

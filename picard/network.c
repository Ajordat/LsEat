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

	frame = createFrame(CODE_CONNECT, HEADER_INIT_CONN_ENT, data);
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

	return (char) strcmp(frame.header, HEADER_CONN_OK);
}

/**
 * Funció per demanar a Data un Enterprise al que connectar-se.
 *
 * @param name	Nom del picard
 * @return		Resposta de Data
 */
Frame establishConnection(char *name) {
	Frame frame;

	frame = createFrame(CODE_CONNECT, HEADER_INIT_CONN_DATA, name);
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

	frame = createFrame(CODE_DISCONNECT, HEADER_END_CONN, name);
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

	if (!strcmp(frame.header, HEADER_CONN_OK))
		print(MSG_DISCONN_ENT_OK);
	else
		print(MSG_DISCONN_ENT_KO);
	return 1;
}

char sendRequestDish(Command cmd, char *data, Menu *dishes) {
	Frame frame;
	char aux[LENGTH];
	int i, resp;
	size_t length;

	frame = createFrame(CODE_REQUEST, HEADER_REQ_DISH, data);
	resp = writeFrame(frame);
	destroyFrame(&frame);

	if (!resp)
		return 0;

	frame = readFrame();

	if (frame.type == FRAME_NULL)
		return 0;

	if (!strcasecmp(frame.header, HEADER_ORDER_OK)) {

		print(MSG_ORDER_OK);
		sprintf(aux, " %s (x%d): Plat demanat!\n", cmd.plat, cmd.unitats);
		print(aux);

		for (i = 0; i < dishes->quantity; i++) {
			if (!strcasecmp(dishes->menu[i].name, cmd.plat)) {
				dishes->menu[i].stock += cmd.unitats;
				break;
			}
		}
		if (i == dishes->quantity) {
			dishes->quantity++;
			dishes->menu = realloc(dishes->menu, sizeof(Dish) * dishes->quantity);
			length = strlen(cmd.plat) + 1;
			dishes->menu[i].name = malloc(length);
			memcpy(dishes->menu[i].name, cmd.plat, length);
			dishes->menu[i].stock = cmd.unitats;
		}

	} else if (!strcasecmp(frame.header, HEADER_ORDER_KO)) {

		print(MSG_ORDER_KO);

		switch (atoi(frame.data)) {    // NOLINT
			case DATA_DISH_NOT_STOCK:
				for (i = 0; frame.data[i] != '&'; i++);
				resp = atoi(frame.data + i + 1);    // NOLINT

				if (resp)
					sprintf(aux, " %s (x%d): Només queden %d unitats.\n", cmd.plat, cmd.unitats, resp);
				else
					sprintf(aux, " %s (x%d): No queda cap unitat", cmd.plat, cmd.unitats);
				print(aux);
				break;
			case DATA_DISH_NOT_FOUND:
				sprintf(aux, " No s'ha trobat el plat \"%s\".\n", cmd.plat);
				print(aux);
				break;
			default:
				sprintf(aux, " Error a la trama. S'ha rebut [%s]\n", frame.data);
				print(aux);
		}
	}

	destroyFrame(&frame);
	return 1;
}

char sendRemoveDish(char *data, Command cmd) {
	Frame frame;
	char aux[LENGTH];
	int resp;

	frame = createFrame(CODE_REMOVE, HEADER_DEL_DISH, data);
	resp = writeFrame(frame);
	destroyFrame(&frame);

	if (!resp)
		return -1;

	frame = readFrame();

	if (frame.type == FRAME_NULL)
		return -1;

	if (!strcasecmp(frame.header, HEADER_ORDER_OK)) {
		sprintf(aux, MSG_ORDER_OK " %s (x%d): Plat descartat!\n", cmd.plat, cmd.unitats);
		print(aux);
		destroyFrame(&frame);
		return 0;
	}

	if (!strcasecmp(frame.header, HEADER_ORDER_KO)) {
		memset(aux, '\0', LENGTH);
		sprintf(aux, MSG_ORDER_KO " %s\n", frame.data);
		print(aux);
	}

	destroyFrame(&frame);
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

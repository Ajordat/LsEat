#include "logica.h"

char checkProgramArguments(int argc) {
	char aux[LENGTH];

	if (argc != 3) {
		sprintf(aux,
				"El format de la crida és incorrecte, ha de ser:\n\tenterprise\t<config_file.dat>\t<menu_file.dat>\n");
		print(aux);
		return 1;
	}
	return 0;
}

void readConfigFile(char *filename) {
	int file;
	char msg[LENGTH], *aux;

	file = open(filename, O_RDONLY);
	if (file <= 0) {
		sprintf(msg, "Error a l'obrir el fitxer %s.\n", filename);
		print(msg);
		exit(EXIT_FAILURE);
	}

	config.name = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.refresh = atoi(aux);
	free(aux);

	config.ip_data = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.port_data = atoi(aux);
	free(aux);

	config.ip_picard = readFileDescriptor(file);

	aux = readFileDescriptor(file);
	config.port_picard = atoi(aux);
	free(aux);

	close(file);

	sprintf(msg, "|%s/%d/%s/%d/%s/%d|\n", config.name, config.refresh,
			config.ip_data, config.port_data,
			config.ip_picard, config.port_picard);
	debug(msg);
}

void readMenuFile(char *filename) {
	int file, index;
	char msg[LENGTH], *aux;
	Dish dish;

	file = open(filename, O_RDONLY);
	if (file <= 0) {
		sprintf(msg, "Error a l'obrir el fitxer %s.\n", filename);
		print(msg);
		exit(EXIT_FAILURE);     //Falta alliberar recursos
	}

	menu.menu = NULL;
	menu.quantity = index = 0;
	dish.name = NULL;

	while (1) {

		dish.name = readFileDescriptor(file);

		if (dish.name == NULL) {
			close(file);
			menu.quantity = index;
			return;
		}

		dish.name[strlen(dish.name) - 1] = '\0';

		sprintf(msg, "[(%s)]\n", dish.name);
		debug(msg);

		aux = readFileDescriptor(file);
		dish.stock = atoi(aux);
		free(aux);

		aux = readFileDescriptor(file);
		dish.price = atoi(aux);
		free(aux);

		menu.menu = realloc(menu.menu, sizeof(Dish) * (index + 1));
		menu.menu[index].name = dish.name;
		menu.menu[index].stock = dish.stock;
		menu.menu[index].price = dish.price;

		index++;
	}
}

void listenSocket(int sock) {
	struct sockaddr_in addr;
	socklen_t addr_len;
	char aux[LENGTH];
	int new_sock;

	listen(sock, MAX_REQUESTS);

	addr_len = sizeof(addr);
	while (1) {

		if ((new_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
			sprintf(aux, "Error a l'establir connexió. 3\n");
			write(1, aux, strlen(aux));
			freeResources();
			exit(EXIT_FAILURE);
		}

		attendPetition(new_sock);

//		close(new_sock);

	}

}

void attendPetition(int sock) {
	Frame frame;

	frame = readFrame(sock);

	switch (frame.type) {
		case CODE_CONNECT:
			printFrame(frame);
			connectPicard(sock, frame);
			break;

		default:
			break;

	}
}

void connectPicard(int sock, Frame frame) {
	Picard picard;
	char *name, *money;
	int ref, i;
	char aux[LENGTH];

	name = frame.data;

	debug("[READING FRAME]\n");
	for (ref = 0; name[ref] != '&'; ref++);

	money = malloc(sizeof(char) * (frame.length - ref + 1));
	memset(money, '\0', sizeof(char) * (frame.length - ref + 1));
	debug("[READING MONEY]\n");
	for (i = ref + 1; i < frame.length; i++)
		money[i - ref - 1] = name[i];


	debug("[DONE]\n");
	sprintf(aux, "[MONEY] -> |%s|\n", money);
	debug(aux);

	picard.money = atoi(money);
	free(money);
	name[ref] = '\0';
	picard.name = malloc(sizeof(char) * (strlen(name) + 1));
	memset(picard.name, '\0', sizeof(char) * (strlen(name) + 1));
	strcpy(picard.name, name);
	debug("[DONE]\n");


	printFrame(frame);

	//frame = getEnterpriseConnection();
	debug("[SENDING FRAME]\n");
	frame.type = 0x09;
	sendFrame(sock, frame);
	debug("[SENT]\n");

	free(frame.data);
	free(picard.name);    //TODO: Eliminar free
}


void freeResources() {
	int i = 0;

	free(config.name);
	free(config.ip_picard);
	free(config.ip_data);

	for (i = 0; i < menu.quantity; i++)
		free(menu.menu[i].name);

	free(menu.menu);

	close(sock_picard);

	//TODO: Close sock_data
}

void controlSigint() {
	freeResources();
	exit(EXIT_SUCCESS);
}

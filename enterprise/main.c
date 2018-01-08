#include <stdlib.h>
#include <signal.h>

#include "logica.h"


int main(int argc, char **argv) {
	char aux[LENGTH];

	if (!checkProgramArguments(argc)) {
		print(MSG_WRONG_ARGS);
		exit(EXIT_FAILURE);
	}

	readConfigFile(argv[1]);

	sprintf(aux, MSG_WELCOME, config.name);
	print(aux);

	if (!connectData()) {
		print(MSG_CONN_DATA_KO);
		freeResources(NOT_CONNECTED);
		exit(EXIT_FAILURE);
	}

	print(MSG_CONN_DATA_OK);

	readMenuFile(argv[2]);

	print(MSG_LOADED_MENU);

	sock_picard = createServerSocket(config.ip_picard, config.port_picard);

	if (sock_picard < 0) {
		freeResources(CONNECTED);
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, controlSigint);

	createUpdateThread();

	listenSocket(sock_picard);

	return EXIT_SUCCESS;
}

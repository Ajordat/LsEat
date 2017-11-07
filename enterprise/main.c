#include <stdlib.h>
#include <signal.h>

#include "logica.h"


int main(int argc, char **argv) {
	char aux[LENGTH];

	checkProgramArguments(argc);

	readConfigFile(argv[1]);

	sprintf(aux, MSG_WELCOME, config.name);
	print(aux);

	readMenuFile(argv[2]);

	print(MSG_LOADED_MENU);

	sock_picard = createSocket(config.ip_picard, config.port_picard);

	if (sock_picard < 0) {
		freeResources();
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, controlSigint);

	listenSocket(sock_picard);

	return EXIT_SUCCESS;
}

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

	if(!connectData()){
		print("No s'ha pogut establir la connexió amb Data.\n");
		freeResources();
		exit(EXIT_FAILURE);
	}

	print("Connexió amb Data realitzada correctament.\n");


	print(MSG_LOADED_MENU);

	sock_picard = createServerSocket(config.ip_picard, config.port_picard);

	if (sock_picard < 0) {
		freeResources();
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, controlSigint);

	updateThread();

	listenSocket(sock_picard);

	return EXIT_SUCCESS;
}

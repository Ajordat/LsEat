#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <signal.h>

#include "logica.h"


int main(void) {
	char aux[LENGTH];

	readConfigFile(FILE_CONFIG);

	sock_picard = createSocket(config.ip, config.port_picard);
	if (sock_picard < 0) {
		sprintf(aux, MSG_PORT_PIC_ERR);
		print(aux);
		freeResources();
		exit(EXIT_FAILURE);
	}

	sock_enterprise = createSocket(config.ip, config.port_enterprise);
	if (sock_enterprise < 0) {
		sprintf(aux, MSG_PORT_ENT_ERR);
		print(aux);
		freeResources();
		exit(EXIT_FAILURE);
	}


	signal(SIGINT, controlSigint);

	print(MSG_EXECUTING);
	listenSocket(sock_picard);			//TODO: En un thread

//    listenSocket(sock_enterprise);	//TODO: En un thread

	return EXIT_SUCCESS;
}

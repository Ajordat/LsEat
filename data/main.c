#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <signal.h>

#include "logica.h"


int main(void) {

	readConfigFile(FILE_CONFIG);

	sock_picard = createSocket(config.ip, config.port_picard);
	if (sock_picard < 0) {
		print(MSG_PORT_PIC_ERR);
		print(MSG_CONFIG_ERR);
		print(strerror(errno));
		freeResources();
		exit(EXIT_FAILURE);
	}

	sock_enterprise = createSocket(config.ip, config.port_enterprise);
	if (sock_enterprise < 0) {
		print(MSG_PORT_ENT_ERR);
		print(MSG_CONFIG_ERR);
		print(strerror(errno));
		freeResources();
		exit(EXIT_FAILURE);
	}

	minheap = HEAP_init();

	signal(SIGINT, controlSigint);

	print(MSG_EXECUTING);

	listenServerSockets();	//Both picard and enterprise

	return EXIT_SUCCESS;
}

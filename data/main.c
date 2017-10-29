//#include <unistd.h>       //fork, sleep, pause, write
//#include <pthread.h>      //pthread_t, pthread_create, pthread_join
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <signal.h>
//#include <string.h>       //strlen
//#include <stdio.h>        //sprintf
//#include <fcntl.h>        //O_RDONLY, O_WRONLY, O_CREAT, O_APPEND

#include "logica.h"


int main(void) {
	char aux[LENGTH];

	readConfigFile(FILE_CONFIG);

	sock_picard = createSocket(config.ip, config.port_picard);
	if (sock_picard < 0) {
		sprintf(aux, "Error al obrir el port pels Picards.\n");
		print(aux);
		freeResources();
		exit(EXIT_FAILURE);
	}

	sock_enterprise = createSocket(config.ip, config.port_enterprise);
	if (sock_enterprise < 0) {
		sprintf(aux, "Error al obrir el port pels Picards.\n");
		print(aux);
		freeResources();
		exit(EXIT_FAILURE);
	}


	signal(SIGINT, controlSigint);

	print("Executant Data...\n");
	listenSocket(sock_picard);

//    listenSocket(sock_enterprise);

	return EXIT_SUCCESS;
}

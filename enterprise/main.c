//#include <unistd.h>       //fork, sleep, pause, write
//#include <pthread.h>      //pthread_t, pthread_create, pthread_join
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <signal.h>
//#include <string.h>       //strlen
//#include <stdio.h>        //sprintf
//#include <fcntl.h>        //O_RDONLY, O_WRONLY, O_CREAT, O_APPEND

#include "logica.h"

int main(int argc, char **argv) {

	checkProgramArguments(argc);

	readConfigFile(argv[1]);

	readMenuFile(argv[2]);

	sock_picard = createSocket(config.ip_picard, config.port_picard);

	signal(SIGINT, controlSigint);

	listenSocket(sock_picard);

	return EXIT_SUCCESS;
}

#include <stdlib.h>
#include <signal.h>

#include "logica.h"

int main(int argc, char **argv) {
	char aux[LENGTH];

	if (checkProgramArguments(argc)) {
		exit(EXIT_FAILURE);
	}

	readConfigFile(argv[1]);

	welcomeMessage();

	sprintf(aux, "|%s - %d - %s - %d|\n", config.name, config.money, config.ip, config.port);
	debug(aux);

	signal(SIGINT, controlSigint);

	shell();

	return EXIT_SUCCESS;
}

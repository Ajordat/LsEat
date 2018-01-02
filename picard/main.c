#include <stdlib.h>
#include <signal.h>

#include "shell.h"

/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint() {
	debug("\nSIGINT REBUT");
	if (sock > 0) {
		disconnect(config.name);
		close(sock);
	}
	freeResources();
	freeHistory();
	exit(EXIT_SUCCESS);
}


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

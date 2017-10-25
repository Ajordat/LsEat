//#include <unistd.h>       //fork, sleep, pause, write
//#include <pthread.h>      //pthread_t, pthread_create, pthread_join
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <signal.h>
//#include <string.h>       //strlen
//#include <stdio.h>        //sprintf
//#include <fcntl.h>        //O_RDONLY, O_WRONLY, O_CREAT, O_APPEND

#include "logica.h"

int main(int argc, char **argv) {
    char aux[LENGTH];

    if (argc != 2) {
        sprintf(aux, "El format de la crida és incorrecte:\n\tpicard <config_file.dat>\n");
        write(1, aux, strlen(aux));
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

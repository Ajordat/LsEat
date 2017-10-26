//#include <unistd.h>       //fork, sleep, pause, write
//#include <pthread.h>      //pthread_t, pthread_create, pthread_join
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
//#include <string.h>       //strlen
//#include <stdio.h>        //sprintf
//#include <fcntl.h>        //O_RDONLY, O_WRONLY, O_CREAT, O_APPEND

#include "logica.h"

int main(void) {

    readConfigFile(FILE_CONFIG);

    return EXIT_SUCCESS;
}

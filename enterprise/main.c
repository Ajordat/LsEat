//#include <unistd.h>       //fork, sleep, pause, write
//#include <pthread.h>      //pthread_t, pthread_create, pthread_join
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
//#include <string.h>       //strlen
//#include <stdio.h>        //sprintf
//#include <fcntl.h>        //O_RDONLY, O_WRONLY, O_CREAT, O_APPEND

#include "logica.h"

void printDish(Dish dish){
    char aux[LENGTH];
    sprintf(aux, "|%s - %d - %d|\n", dish.name, dish.stock, dish.price);
    print(aux);
}
int main(int argc, char **argv) {

    checkProgramArguments(argc);

    readConfigFile(argv[1]);

    readMenuFile(argv[2]);
    int i;
    for(i = 0; i < menu.quantity; i++){
        printDish(menu.menu[i]);
    }

    return EXIT_SUCCESS;
}

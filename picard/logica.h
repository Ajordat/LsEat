#ifndef _LOGICA_H_
#define _LOGICA_H_


#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <sys/fcntl.h>
#include <signal.h>

#include "types.h"
#include "utils.h"
#include "network.h"


char **history;
Config config;
Menu dishes;


char checkProgramArguments(int argc);

void welcomeMessage();

void readConfigFile(char *filename);

char menuOptions(Command cmd);

char initConnection();

char requestMenu();

void showOrder();

char requestDish(Command cmd);

char removeDish(Command cmd);

char requestPayment();

char recoverConnection();

void controlSigint();

void freeResources();


#endif

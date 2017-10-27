#ifndef _LOGICA_H_
#define _LOGICA_H_


#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <sys/fcntl.h>

#include "utils.h"
#include "network.h"


#define ERR_UNK_CMD     (char)  0xFF
#define ERR_N_PARAMS    (char)  0XFE


typedef struct {
    char *name;
    int money;
    char *ip;
    int port;
} Config;

typedef struct {
    char code;
    int unitats;
    char *plat;
} Command;

Config config;


char checkProgramArguments(int argc);

void welcomeMessage();

void readConfigFile(char *filename);

void shell();

void controlSigint();

void freeResources();


#endif

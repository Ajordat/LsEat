#ifndef _LOGICA_H_
#define _LOGICA_H_


#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <sys/fcntl.h>

#include "utils.h"
#include "network.h"


#define ERR_UNK_CMD     (char)  0xFF
#define ERR_N_PARAMS    (char)  0XFE

#define CMD_CONNECT		"CONNECTA"
#define CMD_DISCONNECT	"DESCONNECTA"
#define	CMD_SHOW		"MOSTRA"
#define CMD_MENU		"MENU"
#define CMD_REQUEST		"DEMANA"
#define CMD_REMOVE		"ELIMINA"
#define CMD_PAY			"PAGAR"

#define	MSG_PIC_DATA_OK		"[Connexió amb Data OK]\n"
#define	MSG_PIC_DATA_KO		"[Connexió amb Data KO]\n"
#define	MSG_PIC_ENT_OK		"[Connexió amb Enterprise OK]\n"
#define	MSG_PIC_ENT_KO		"[Connexió amb Enterprise KO]\n"
#define	MSG_CONEX_OK		"Connexió realitzada!\n"

#define	HEADER_PIC_DATA_OK		"ENT_INF"



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

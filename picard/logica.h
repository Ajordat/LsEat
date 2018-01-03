#ifndef _LOGICA_H_
#define _LOGICA_H_


#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <sys/fcntl.h>

#include "utils.h"
#include "network.h"
//#include "shell.h"


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
#define	HEADER_MENU_REQ			"SHW_MENU"
#define	HEADER_MENU_END			"END_MENU"
#define	HEADER_REQ_DISH			"NEW_ORD"
#define	HEADER_DEL_DISH			"DEL_ORD"
#define	HEADER_ORDER_OK			"ORDOK"
#define	HEADER_ORDER_KO			"ORDKO"

#define	MONEDA		"€"


typedef struct {
	char code;
	int unitats;
	char *plat;
} Command;

typedef struct {
    char *name;
    int money;
    char *ip;
    int port;
} Config;

typedef struct {
	char* name;
	int price;
	int stock;
} Dish;

typedef struct {
	Dish *menu;
	int quantity;
} Menu;


char **history;
Config config;
Menu dishes;


char checkProgramArguments(int argc);

void welcomeMessage();

void readConfigFile(char *filename);

char initConnection();

void requestMenu();

void requestDish(Command cmd);

void removeDish(Command cmd);

void controlSigint();

void freeResources();


#endif

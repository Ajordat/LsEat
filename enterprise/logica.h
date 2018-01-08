#ifndef _ENTERPRISE_LOGICA_H_
#define _ENTERPRISE_LOGICA_H_


#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "utils.h"
#include "network.h"


#define	MSG_WELCOME			"Benvingut al restaurant %s\n"
#define	MSG_LOADED_MENU		"Carregat Menu!\n"
#define MSG_FILE_ERR		"Error a l'obrir el fitxer %s.\n"
#define	MSG_WAITING			"Esperant clients...\n"
#define MSG_CONEX_ERR		"Error a l'establir connexió.\n"
#define	MSG_CONN_PIC		"Connectant %s\n"
#define	MSG_DISC_PIC		"Desconnectant %s\n"

#define	CONNECTED		1
#define	NOT_CONNECTED	0


typedef struct {
	char *name;
	int refresh;
	char *ip_data;
	int port_data;
	char *ip_picard;
	int port_picard;
} Config;

typedef struct {
	char *name;
	int stock;
	int price;
} Dish;

typedef struct {
	Dish *menu;
	int quantity;
} Menu;

typedef struct {
	char *name;
	int money;
} Picard;


Config config;
Menu menu;
int sock_data;
int sock_picard;
//int fiUpdate;
pthread_t update;
pthread_mutex_t mutUsers;
pthread_mutex_t mutMenu;
int nUsers;


char connectData();

char checkProgramArguments(int argc);

void readConfigFile(char *filename);

void readMenuFile(char *filename);

void listenSocket(int sock);

void *attendPetition(void *sock);

char connectPicard(int sock, Frame frame, Picard *picard);

void disconnectPicard(int sock, Frame frame);

void freeResources(char connected);

void controlSigint();

void createUpdateThread();


#endif

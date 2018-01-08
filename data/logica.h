#ifndef _ENTERPRISE_LOGICA_H_
#define _ENTERPRISE_LOGICA_H_

#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "network.h"
#include "MinHeap.h"


#define FILE_CONFIG			"configData.dat"

#define	MSG_CONFIG_ERR		ANSI_COLOR_RED "[Config ERR] " ANSI_COLOR_RESET
#define MSG_FILE_ERR		MSG_CONFIG_ERR "Error a l'obrir el fitxer %s.\n"
#define	MSG_PORT_PIC_ERR	MSG_CONFIG_ERR "Error al obrir el port pels Picards.\n"
#define	MSG_PORT_ENT_ERR	MSG_CONFIG_ERR "Error al obrir el port pels Enterprise.\n"
#define	MSG_EXECUTING		"Executant Data...\n"
#define	MSG_WAITING			"Esperant clients...\n"

#define MSG_CONNEX_IN		ANSI_COLOR_GREEN "[Petició de connexió] " ANSI_COLOR_RESET
#define MSG_UPDATE_IN		ANSI_COLOR_GREEN "[Petició d'actualització]\n" ANSI_COLOR_RESET
#define MSG_DISC_IN			ANSI_COLOR_GREEN "[Petició de desconnexió]\n" ANSI_COLOR_RESET

#define	MSG_CONNECT_PIC		"Petició de Picard\n"
#define	MSG_CONNECT_ENT		"Petició d'Enterprise\n"
#define MSG_CONNECT_UNK		"Petició d'origen desconnegut\n"
#define	MSG_WELCOME_CLIENT	"Origen: %s\n"
#define	MSG_CONN_PIC_OK		ANSI_COLOR_CYAN "[Connect OK] " ANSI_COLOR_RESET "Assignat a %s!\n"
#define	MSG_CONN_PIC_KO		ANSI_COLOR_RED "[Connect KO] " ANSI_COLOR_RESET "No hi ha cap Enterprise disponible\n"
#define	MSG_CONN_ENT_OK		ANSI_COLOR_CYAN "[Connect OK] " ANSI_COLOR_RESET "Enterprise afegit!\n"
#define	MSG_CONN_ENT_KO		ANSI_COLOR_RED "[Connect KO] " ANSI_COLOR_RESET "Ja hi ha un Enterprise a aquesta direcció\n"

#define	MSG_UPDATE_OK		ANSI_COLOR_CYAN "[Update OK] " ANSI_COLOR_RESET "%s actualitzat: %d clients\n"
#define	MSG_UPDATE_KO		ANSI_COLOR_RED "[Update KO] " ANSI_COLOR_RESET "No es té registre de la seva connexió\n"

#define	MSG_DISC_OK			ANSI_COLOR_CYAN "[Disconnect OK] " ANSI_COLOR_RESET "Enterprise desconnectat!\n"
#define	MSG_DISC_KO			ANSI_COLOR_RED "[Disconnect KO] " ANSI_COLOR_RESET "No es té registre de la seva connexió\n"



typedef struct {
    char *ip;
    int port_picard;
    int port_enterprise;
} Config;


Config config;

int sock_picard;
int sock_enterprise;
MinHeap minheap;


void readConfigFile(char *filename);

void attendPetition(int sock);

void listenServerSockets();

void connectSocket(int sock, Frame frame);

Frame getEnterpriseConnection();

void freeResources();

void controlSigint();


#endif

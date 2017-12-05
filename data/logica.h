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

#define	MSG_WAITING			"Esperant clients...\n"
#define MSG_FILE_ERR		"Error a l'obrir el fitxer %s.\n"
#define	MSG_CONNECT_PIC		"Connectant %s\n"
#define	MSG_DISCONNECT_PIC	"Desconnectant %s\n"
#define	MSG_PORT_PIC_ERR	"Error al obrir el port pels Picards.\n"
#define	MSG_PORT_ENT_ERR	"Error al obrir el port pels Enterprise.\n"
#define	MSG_EXECUTING		"Executant Data...\n"


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

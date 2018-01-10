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


/**
 * Llegeix el fitxer de configuració que conté la seva ip i els dos ports pels Picards i els Enterprise.
 *
 * @param filename 	Nom del fitxer de configuració
 */
void readConfigFile(char *filename);

/**
 * Funció per executar les peticions dels clients a partir del camp type de la trama.
 *
 * @param sock 	Socket de la connexió amb el client
 */
void attendPetition(int sock);

/**
 * Funció d'escolta del socket. Bloqueja l'execució esperant que un client es connecti al port de Picard o d'Enterprise.
 * No és dedicat.
 *
 * @param sock 	Socket a escoltar
 */
void listenServerSockets();

/**
 * Funció per tractar les peticions de connexió, tant de Picard com d'enterprise.
 *
 * @param sock 		Socket de connexió amb el client
 * @param frame 	Trama rebuda del client amb la seva informació.
 */
void connectSocket(int sock, Frame frame);

/**
 * Genera la trama de resposta al client amb la direcció ip i port d'un Enterprise, si tot ha anat bé.
 *
 * @return 	Trama a respondre al client
 */
Frame getEnterpriseConnection();

/**
 * Funció per alliberar els recursos del programa.
 */
void freeResources();

/**
 * Funció per a capturar el signal SIGINT i alliberar els recursos abans d'aturar l'execució.
 */
void controlSigint();


#endif

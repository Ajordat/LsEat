#ifndef _ENTERPRISE_NETWORK_H_
#define _ENTERPRISE_NETWORK_H_


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>

#include "utils.h"


#define CODE_CONNECT	(char)	0x01
#define CODE_DISCONNECT	(char)	0x02
#define CODE_SHOWMENU	(char)	0x03
#define CODE_REQUEST	(char)	0x04
#define CODE_REMOVE		(char)	0x05
#define CODE_PAYMENT	(char)	0x06
#define CODE_UPDATE		(char)	0x07

#define	HEADER_PIC_CONN_OK			"CONOK"
#define	HEADER_PIC_ENT_DISC_OK		"CONOK"
#define	HEADER_SHW_MENU_DISH		"DISH"
#define	HEADER_SHW_MENU_END_DISH	"END_MENU"
#define	HEADER_REQ_OK				"ORDOK"
#define	HEADER_REQ_KO				"ORDKO"
#define	HEADER_REM_OK				"ORDOK"
#define	HEADER_REM_KO				"ORDKO"
#define HEADER_PAY_OK				"PAYOK"
#define	HEADER_PAY_KO				"PAYKO"


#define DATA_DISH_NOT_STOCK		1
#define DATA_DISH_NOT_FOUND		"2"
#define	DATA_REM_KO				"No es pot cancelar aquesta quantitat d'unitats."
#define	DATA_REM_NOT_FOUND		"No s'ha trobat el plat demanat."
#define	DATA_PAY_EMPTY			"No has demanat res."
#define	DATA_PAY_CANT_AFFORD	"Has excedit el teu pressupost: La comanda és de %d%s i tens %d%s."

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define	MSG_CONFIG_OK			ANSI_COLOR_CYAN "[Config OK] " ANSI_COLOR_RESET
#define	MSG_CONFIG_KO			ANSI_COLOR_RED "[Config KO] " ANSI_COLOR_RESET
#define	MSG_CONN_OK				ANSI_COLOR_CYAN "[Conn OK] " ANSI_COLOR_RESET
#define	MSG_CONN_KO				ANSI_COLOR_RED "[Conn KO] " ANSI_COLOR_RESET
#define	MSG_WELCOME				"Benvingut al restaurant %s\n"
#define	MSG_LOADED_MENU			MSG_CONFIG_OK "Menú carregat!\n"
#define MSG_FILE_ERR			MSG_CONFIG_KO "Error a l'obrir el fitxer %s.\n"
#define	MSG_CREATE_UPD			MSG_CONFIG_OK "Thread per updates creat\n"
#define	MSG_WAITING				"Esperant clients...\n"
#define MSG_CONN_ERR			MSG_CONN_KO "Error a l'establir connexió. %s\n"
#define	MSG_CONN_DATA_KO		MSG_CONN_KO "No s'ha pogut establir la connexió amb Data.\n"
#define	MSG_CONN_DATA_OK		MSG_CONN_OK "Connexió amb Data realitzada correctament.\n"
#define	MSG_WRONG_ARGS			MSG_CONFIG_KO "El format de la crida és incorrecte, ha de ser:\n\tenterprise\t<config_file.dat>\t<menu_file.dat>\n"
#define	MSG_PIC_DOWN			MSG_CONN_KO "Picard ha caigut.\n"

#define	MSG_CONN_IN				ANSI_COLOR_GREEN "[Petició de connexió]\n" ANSI_COLOR_RESET
#define	MSG_CONN_PIC			MSG_CONN_OK "Connectant %s\n"

#define	MSG_DISC_IN				ANSI_COLOR_GREEN "[Petició de desconnexió]\n" ANSI_COLOR_RESET
#define	MSG_DISC_PIC			MSG_CONN_OK "Desconnectant %s\n"

#define	MSG_SHOW_MENU_IN		ANSI_COLOR_GREEN "[Petició de menú]\n" ANSI_COLOR_RESET

#define	MSG_REQ_IN				ANSI_COLOR_GREEN "[Petició demanar plat] " ANSI_COLOR_RESET
#define	MSG_REQ_DISH			"Buscant %s...\n"
#define	MSG_REQ_OK				ANSI_COLOR_CYAN "[Req OK] " ANSI_COLOR_RESET "Plat trobat: Afegides %d unitats\n"
#define	MSG_REQ_KO				ANSI_COLOR_RED "[Req KO] " ANSI_COLOR_RESET "No hi ha unitats\n"
#define	MSG_REQ_NOT_FOUND		ANSI_COLOR_RED "[Req KO] " ANSI_COLOR_RESET "Plat no trobat\n"

#define	MSG_REM_IN				ANSI_COLOR_GREEN "[Petició d'eliminar plat] " ANSI_COLOR_RESET
#define	MSG_REM_DISH			"Buscant %s...\n"
#define	MSG_REM_OK				ANSI_COLOR_CYAN "[Req OK] " ANSI_COLOR_RESET "Plat trobat: Eliminades %d unitats\n"
#define	MSG_REM_KO				ANSI_COLOR_RED "[Req KO] " ANSI_COLOR_RESET "No hi ha unitats\n"
#define	MSG_REM_NOT_FOUND		ANSI_COLOR_RED "[Req KO] " ANSI_COLOR_RESET "Plat no trobat\n"

#define	MSG_PAY_IN				ANSI_COLOR_GREEN "[Petició de pagar]\n" ANSI_COLOR_RESET
#define	MSG_PAY_OK				ANSI_COLOR_CYAN "[Pay OK] " ANSI_COLOR_RESET "Comanda cobrada: %d%s\n"
#define	MSG_PAY_CANT_AFFORD		ANSI_COLOR_RED "[Pay KO] " ANSI_COLOR_RESET "No hi ha prous diners: Costa %d%s i hi ha %d%s\n"
#define	MSG_PAY_EMPTY			ANSI_COLOR_RED "[Pay KO] " ANSI_COLOR_RESET "No s'ha demanat cap plat\n"


#define LENGTH			100
#define MAX_REQUESTS	128
#define HEADER_SIZE		10
#define FRAME_NULL		(-1)


typedef struct {
	char type;
	char header[10];
	short length;
	char *data;
} Frame;


void debugFrame(Frame);

char sendFrame(int sock, Frame);

Frame readFrame(int sock);

Frame createFrame(char type, char *header, char *data);

void destroyFrame(Frame *);


int createClientSocket(char *ip, int port);

int createServerSocket(char *ip, int port);




#endif

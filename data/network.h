#ifndef _DATA_NETWORK_H_
#define _DATA_NETWORK_H_


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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define CODE_CONNECT		(char)	0x01
#define CODE_DISCONNECT		(char)	0x02
#define CODE_UPDATE			(char)	0x07

#define	HEADER_PIC_CONN_REQ			"PIC_NAME"
#define	HEADER_ENT_CONN_REQ			"ENT_INF"
#define	HEADER_PIC_CON_OK			"ENT_INF"
#define	HEADER_PIC_CON_KO			"CONKO"
#define	HEADER_ENT_CON_OK			"CONOK"
#define	HEADER_ENT_CON_KO			"CONKO"
#define	HEADER_ENT_UPDATE_OK		"UPDATEOK"
#define	HEADER_ENT_UPDATE_KO		"UPDATEKO"
#define	HEADER_ENT_DISC_OK			"CONOK"
#define	HEADER_ENT_DISC_KO			"CONKO"

#define MSG_CONEX_ERR	"Error a l'establir connexió.\n"

#define MAX_REQUESTS	128
#define HEADER_SIZE		10
#define	IP_SIZE			20
#define FRAME_NULL		(-1)


typedef struct {
	char type;
	char header[HEADER_SIZE];
	short length;
	char *data;
} Frame;


int createSocket(char *ip, int port);

void debugFrame(Frame);

char sendFrame(int sock, Frame);

Frame readFrame(int sock);

Frame createFrame(char type, char *header, char *data);

void destroyFrame(Frame *frame);

#endif

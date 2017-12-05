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


#define CODE_CONNECT		(char)	0x01
#define CODE_DISCONNECT		(char)	0x02
#define CODE_UPDATE			(char)	0x07

#define	HEADER_PIC_DATA_CONNECT		"PIC_NAME"
#define	HEADER_DATA_PIC_CON_OK		"ENT_INF"
#define	HEADER_DATA_PIC_CON_KO		"CONKO"

#define MSG_CONEX_ERR	"Error a l'establir connexió.\n"

#define MAX_REQUESTS	128
#define HEADER_SIZE		10

#define HARDCODED_ENTERPRISE	"Enterprise A&127.0.0.1&8491"

typedef struct {
	char type;
	char header[HEADER_SIZE];
	short length;
	char *data;
} Frame;


int createSocket(char *ip, int port);

void debugFrame(Frame);

void sendFrame(int sock, Frame);

Frame readFrame(int sock);

Frame createFrame(char type, char *header, char *data);


#endif

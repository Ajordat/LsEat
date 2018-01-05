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

#define	HEADER_PIC_ENT_CONN_OK	"CONOK"
#define	HEADER_PIC_ENT_DISC_OK	"CONOK"

#define DATA_DISH_NOT_STOCK		1
#define DATA_DISH_NOT_FOUND		"2"

#define MSG_CONEX_ERR	"Error a l'establir connexió.\n"

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

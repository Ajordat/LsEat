#ifndef _PICARD_NETWORK_UTILS_H
#define _PICARD_NETWORK_UTILS_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "utils.h"


#define HEADER_SIZE		10


typedef struct {
	char type;
	char header[HEADER_SIZE];
	short length;
	char *data;
} Frame;


int createClientSocket(char *ip, int port);

void sendFrame(int socket, Frame);

Frame receiveFrame(int socket);

Frame createFrame(char type, char *header, char *data);

void debugFrame(Frame);


#endif

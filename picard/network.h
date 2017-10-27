#ifndef _PICARD_NETWORK_H_
#define _PICARD_NETWORK_H_


#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "utils.h"


#define CODE_CONNECT    (char)  0x01
#define CODE_DISCONNECT (char)  0x02
#define CODE_SHOWMENU   (char)  0x03
#define CODE_REQUEST    (char)  0x04
#define CODE_REMOVE     (char)  0x05
#define CODE_PAY        (char)  0x06


typedef struct {
    char type;
    char header[10];
    short length;
    char *data;
} Frame;

typedef struct {
	char *name;
	char *ip;
	int port;
} Enterprise;

int sock;
Enterprise enterprise;


void establishConnection(char *name);

void getSocket(char *ip, int port);

void sendFrame(Frame frame);

Frame readFrame();

void printFrame(Frame frame);

#endif

#ifndef _PICARD_NETWORK_H_
#define _PICARD_NETWORK_H_


#include "utils.h"
#include "network_utils.h"


#define CODE_CONNECT    (char)  0x01
#define CODE_DISCONNECT (char)  0x02
#define CODE_SHOWMENU   (char)  0x03
#define CODE_REQUEST    (char)  0x04
#define CODE_REMOVE     (char)  0x05
#define CODE_PAYMENT        (char)  0x06


typedef struct {
	char *name;
	char *ip;
	int port;
} Enterprise;

typedef struct {
	char *ip;
	int port;
} Socket;


int sock;

Enterprise enterprise;		//No tinc clar que sigui gaire útil això


char connectEnterprise(char *name, int money);

char disconnect(char *name);

Frame establishConnection(char *name);

char writeFrame(Frame);

Frame readFrame();


#endif

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


#define CODE_CONNECT    (char)  0x01
#define CODE_DISCONNECT (char)  0x02
#define CODE_UPDATE		(char)	0x07

#define LENGTH 100
#define MAX_REQUESTS 5


typedef struct {
    char type;
    char header[10];
    short length;
    char *data;
} Frame;


int createSocket(char *ip, int port);

void printFrame(Frame frame);

void sendFrame(int sock, Frame frame);

Frame readFrame(int sock);


#endif

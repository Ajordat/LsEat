#ifndef _ENTERPRISE_LOGICA_H_
#define _ENTERPRISE_LOGICA_H_

#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>

#include "utils.h"
#include "network.h"


typedef struct {
	char *name;
	int refresh;
	char *ip_data;
	int port_data;
	char *ip_picard;
	int port_picard;
} Config;

typedef struct {
	char *name;
	int stock;
	int price;
} Dish;

typedef struct {
	Dish *menu;
	int quantity;
} Menu;

typedef struct {
	char *name;
	int money;
} Picard;

Config config;
Menu menu;
int sock_data;
int sock_picard;

char checkProgramArguments(int argc);

void readConfigFile(char *filename);

void readMenuFile(char *filename);

void listenSocket(int sock);

void attendPetition(int sock);

void connectPicard(int sock, Frame frame);

void disconnectPicard(int sock, Frame frame);

void freeResources();

void controlSigint();

#endif

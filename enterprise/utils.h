#ifndef _ENTERPRISE_UTILS_H_
#define _ENTERPRISE_UTILS_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 			0
#define LENGTH 			100
#define INT_LENGTH		12
#define MONEDA			"€"
#define COLOR_DEBUG		"\x1b[33m"	//YELLOW
#define COLOR_RESET		"\x1b[0m"


void debug(char *msg);

void print(const char *msg);

char *readFileDescriptor(int fd);

void myItoa(int num, char *buff);


#endif

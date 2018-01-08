#ifndef _ENTERPRISE_UTILS_H_
#define _ENTERPRISE_UTILS_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define LENGTH	100
#define DEBUG	0
#define COLOR_DEBUG		"\x1b[33m"	//YELLOW
#define COLOR_RESET		"\x1b[0m"


void debug(char *msg);

void print(char *msg);

char *readFileDescriptor(int fd);


#endif

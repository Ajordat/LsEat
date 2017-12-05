#ifndef _ENTERPRISE_UTILS_H_
#define _ENTERPRISE_UTILS_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define LENGTH 100
#define DEBUG 1


void debug(char *msg);

void print(char *msg);

char *readFileDescriptor(int fd);


#endif

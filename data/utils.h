#ifndef _ENTERPRISE_UTILS_H_
#define _ENTERPRISE_UTILS_H_

#include <unistd.h>
#include <string.h>

#define DEBUG 0


void debug(char *msg);

void print(char *msg);

char *readFileDescriptor(int fd);

#endif

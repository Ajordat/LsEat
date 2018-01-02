#ifndef _ENTERPRISE_UTILS_H_
#define _ENTERPRISE_UTILS_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 		1
#define LENGTH 		100
#define INT_LENGTH	12


void debug(char *msg);

void print(char *msg);

void println(char *msg);

void printi(int num);

void printc(char msg);

char *readFileDescriptor(int fd);

void myItoa(int num, char *buff);


#endif

#ifndef _PICARD_UTILS_H_
#define _PICARD_UTILS_H_

#include <string.h>       //strlen
#include <stdio.h>        //sprintf
#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, malloc, realloc
#include <ctype.h>        //tolower

#define LENGTH 100
#define DEBUG 0


void debug(char *msg);

void print(char *msg);

void printc(char mychar);

void printi(int myint);

char *getWord(int *index, const char *string);

char endOfWord(int index, const char *string);

char checkNumber(const char *word);

char * readFileDescriptor(int fd);

void myItoa(int num, char *buff);

#endif

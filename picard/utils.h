#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>       //strlen
#include <stdio.h>        //sprintf
#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc
#include <ctype.h>        //tolower

#define DEBUG 0


void debug(char * msg);
void print(char * msg);
void printc(char mychar);
void printi(int myint);

char *getWord(int *index, const char *string);
char endOfWord(int index, const char * string);
int beginsWith(char* prefix, char* string);
int equals(char* pattern, char* string);
char checkNumber(const char *word);

#endif

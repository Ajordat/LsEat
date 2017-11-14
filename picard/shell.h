#ifndef _SHELL_H
#define _SHELL_H


#include <stdlib.h>
//#include <termio.h>
#include "logica.h"
#include "utils.h"

typedef struct {
	char code;
	int unitats;
	char *plat;
} Command;
 
//static struct termios old, new;
char **history;
int nLog;
int indexLog;


void initShell();

void appendCommand(Command cmd);

char solveCommand(const char *command);

void freeHistory();

void printHistory();

#endif

#ifndef _SHELL_H
#define _SHELL_H


#include <stdlib.h>
#include <termio.h>

#include "logica.h"
#include "utils.h"


#define	KEY_ARROW1	27
#define	KEY_ARROW2	'['
#define	KEY_UP		'A'
#define	KEY_DOWN	'B'
#define	KEY_RIGHT	'C'
#define	KEY_LEFT	'D'

#define SHELL_LENGTH	70

#define	DISCONNECT	0
#define	SIGNAL		1


int nLog;
int indexLog;
struct termios old;


void initShell();

void shell();

void appendCommand(Command);

char solveCommand(char *command);

void freeHistory(int reason);

void printHistory();


#endif

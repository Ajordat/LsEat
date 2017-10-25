#ifndef _LOGICA_H_
#define _LOGICA_H_

#include <unistd.h>       //fork, sleep, pause, write
#include <stdlib.h>       //EXIT_FAILURE, EXIT_SUCCESS, atoi, mallor, realloc

#include "utils.h"

#define LENGTH 100
#define STDIN STDIN_FILENO

#define CODE_CONNECT    (char)  0x01
#define CODE_DISCONNECT (char)  0x02
#define CODE_SHOWMENU   (char)  0x03
#define CODE_REQUEST    (char)  0x04
#define CODE_REMOVE     (char)  0x05
#define CODE_PAY        (char)  0x06

#define ERR_UNK_CMD     (char)  0xFF
#define ERR_N_PARAMS    (char)  0XFE

typedef struct {
    char code;
    int unitats;
    char *plat;
} Command;


void shell();


#endif

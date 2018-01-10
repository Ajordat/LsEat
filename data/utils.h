#ifndef _ENTERPRISE_UTILS_H_
#define _ENTERPRISE_UTILS_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define LENGTH			100
#define DEBUG			0
#define COLOR_DEBUG		"\x1b[33m"	//YELLOW
#define COLOR_RESET		"\x1b[0m"


/**
 * Funció que escriu el missatge que rep com a paràmetre si DEBUG val diferent a 0.
 * S'utilitza per depurar.
 *
 * @param msg 	Missatge a mostrar
 */
void debug(char *msg);

/**
 * Funció per printar per pantalla una cadena de caràcters.
 *
 * @param msg 	Missatge a mostrar
 */
void print(char *msg);

/**
 * Funció per llegir caràcter a caràcter d'un file descriptor i a memòria dinàmica fins a trobar un salt de línia
 * o un '\0'.
 *
 * @param fd	File descriptor
 * @return		Punter a memòria dinàmica amb el text llegit
 */
char *readFileDescriptor(int fd);


#endif

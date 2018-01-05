#ifndef _PICARD_NETWORK_H_
#define _PICARD_NETWORK_H_


#include "types.h"
#include "utils.h"
#include "network_utils.h"


int sock;

Enterprise enterprise;		//No tinc clar que sigui gaire útil això


char connectEnterprise(char *name, int money);

char disconnect(char *name);

Frame establishConnection(char *name);

char sendRequestDish(Command cmd, char *data, Menu *dishes);

char sendRemoveDish(char *data);

char writeFrame(Frame);

Frame readFrame();


#endif

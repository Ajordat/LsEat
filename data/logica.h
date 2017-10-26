#ifndef _ENTERPRISE_LOGICA_H_
#define _ENTERPRISE_LOGICA_H_

#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>

#include "utils.h"

#define LENGTH 100
#define FILE_CONFIG "configData.dat"

typedef struct {
    char *ip;
    int port_picard;
    int port_enterprise;
} Config;

Config config;

void readConfigFile(char *filename);

#endif

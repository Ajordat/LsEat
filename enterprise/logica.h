#ifndef _ENTERPRISE_LOGICA_H_
#define _ENTERPRISE_LOGICA_H_

#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>

#include "utils.h"


typedef struct {
    char *name;
    int refresh;
    char *ip_data;
    int port_data;
    char *ip_picard;
    int port_picard;
} Config;

typedef struct{
    char * name;
    int stock;
    int price;
}Dish;

typedef struct {
    Dish * menu;
    int quantity;
}Menu;

Config config;
Menu menu;

char checkProgramArguments(int argc);

void readConfigFile(char *filename);

void readMenuFile(char *filename);

#endif

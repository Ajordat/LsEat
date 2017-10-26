#include "logica.h"


void readConfigFile(char *filename) {
    int file;
    char msg[LENGTH], *aux;

    file = open(filename, O_RDONLY);
    if (file <= 0) {
        sprintf(msg, "Error a l'obrir el fitxer %s.\n", filename);
        print(msg);
        exit(EXIT_FAILURE);
    }
    print("Tot bé!\n");

    config.ip = readFileDescriptor(file);
    aux = readFileDescriptor(file);
    config.port_picard = atoi(aux);
    free(aux);
    aux = readFileDescriptor(file);
    config.port_enterprise = atoi(aux);
    free(aux);

    sprintf(msg, "|%s - %d - %s - %d - %s - %d|\n", config.name, config.refresh, config.ip_data, config.port_data,
            config.ip_picard, config.port_picard);
    debug(msg);
}



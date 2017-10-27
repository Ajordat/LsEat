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

    sock_picard = sock_enterprise = -1;

    sprintf(msg, "|%s - %d - %d|\n", config.ip, config.port_picard, config.port_enterprise);
    debug(msg);
}

void listenSocket(int sock) {
    struct sockaddr_in addr;
    socklen_t addr_len;
    char aux[LENGTH];
    int new_sock;

    listen(sock, MAX_REQUESTS);

    addr_len = sizeof(addr);
    while (1) {

        if ((new_sock = accept(sock, (void *) &addr, &addr_len)) < 0) {
            sprintf(aux, "Error a l'establir connexió. 3\n");
            write(1, aux, strlen(aux));
            freeResources();
            exit(EXIT_FAILURE);
        }

        attendPetition(new_sock);

        close(new_sock);

    }

}

void attendPetition(int sock) {
    Frame frame;

    frame = readFrame(sock);

    switch (frame.type) {
        case CODE_CONNECT:
            connectSocket(sock, frame);
            break;

        default:
            break;

    }
}

void connectSocket(int sock, Frame frame){
    char *name;
    char aux[LENGTH];

    if(frame.header[0] == 'P'){
        name = frame.data;
        sprintf(aux, "Connectant %s...\n", name);
        print(aux);
        printFrame(frame);

        frame = getEnterpriseConnection();
        sendFrame(sock, frame);

        sprintf(aux, "Desconnectant %s...\n", name);
        print(aux);

        free(name);
        free(frame.data);
    } else {
        //TODO: Tractar petició Enterprise
    }
}

char getConnectionStatus(){return CONNECT_STATUS;}

Frame getEnterpriseConnection() {
    Frame frame;

    frame.type = 0x01;

    memset(frame.header, '\0', 10);

    if (getConnectionStatus()) {
        sprintf(frame.header, "ENT_IF");
        frame.data = malloc(sizeof("Enterprise A&127.0.0.1&8491"));
        strcpy(frame.data, "Enterprise A&127.0.0.1&8491");
        frame.length = (short) strlen(frame.data);
    } else {
        sprintf(frame.header, "CONKO");
        frame.length = 0;
        frame.data = malloc(sizeof(char));
        frame.data[0] = '\0';
    }

    return frame;
}

void freeResources(){
    if (sock_picard > 0)
        close(sock_picard);
    if (sock_enterprise > 0)
        close(sock_enterprise);
    if (config.ip != NULL)
        free(config.ip);
}

void controlSigint(){
    freeResources();
    exit(EXIT_SUCCESS);
}
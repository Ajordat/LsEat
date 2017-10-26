#include "logica.h"

char checkProgramArguments(int argc){
    char aux[LENGTH];

    if (argc != 2) {
        sprintf(aux, "El format de la crida és incorrecte, ha de ser:\n\tpicard <config_file.dat>\n");
        print(aux);
        return 1;
    }
    return 0;
}

void welcomeMessage(){
    char aux[LENGTH];

    sprintf(aux, "Benvingut %s\n", config.name);
    print(aux);
    sprintf(aux, "Tens %d euros disponibles\n", config.money);
    print(aux);
    sprintf(aux, "Introdueix comandes...\n");
    print(aux);
}

char checkParameters(int index, const char *command, char code) {
    return endOfWord(index, command) ? code : ERR_N_PARAMS;
}

void readConfigFile(char *filename) {
    int file;
    char msg[LENGTH], *aux;

    file = open(filename, O_RDONLY);
    if (file <= 0) {
        sprintf(msg, "Error a l'obrir el fitxer %s.\n", filename);
        print(msg);
        exit(EXIT_FAILURE);
    }

    config.name = readFileDescriptor(file);
    aux = readFileDescriptor(file);
    config.money = atoi(aux);
    free(aux);
    config.ip = readFileDescriptor(file);
    aux = readFileDescriptor(file);
    config.port = atoi(aux);
    free(aux);

    close(file);

    sprintf(msg, "|%s - %d - %s - %d|\n", config.name, config.money, config.ip, config.port);
    debug(msg);
}


Command substractCommand(const char *command) {
    int i = 0;
    char *word;
    Command cmd;

    if(command == NULL){
        cmd.code = ERR_UNK_CMD;
        return cmd;
    }
    word = getWord(&i, command);

    if (!strcasecmp("CONNECTA", word)) {

        cmd.code = checkParameters(i, command, CODE_CONNECT);

    } else if (!strcasecmp("MOSTRA", word)) {

        free(word);
        word = getWord(&i, command);

        if (!strcasecmp("MENU", word)) {
            cmd.code = checkParameters(i, command, CODE_SHOWMENU);
        } else {
            cmd.code = ERR_UNK_CMD;
        }

    } else if (!strcasecmp("DEMANA", word)) {

        free(word);
        word = getWord(&i, command);
        if (checkNumber(word)) {
            cmd.unitats = atoi(word);
            cmd.plat = getWord(&i, command);
            if (cmd.plat[0] == '\0') {
                cmd.code = ERR_N_PARAMS;
                free(cmd.plat);
            } else {
                cmd.code = CODE_REQUEST;
            }
        } else {
            cmd.code = ERR_N_PARAMS;
        }

    } else if (!strcasecmp("ELIMINA", word)) {

        free(word);
        word = getWord(&i, command);
        if (checkNumber(word)) {
            cmd.unitats = atoi(word);
            cmd.plat = getWord(&i, command);
            if (cmd.plat[0] == '\0') {
                cmd.code = ERR_N_PARAMS;
                free(cmd.plat);
            } else {
                cmd.code = CODE_REMOVE;
            }
        } else {
            cmd.code = ERR_N_PARAMS;
        }

    } else if (!strcasecmp("PAGAR", word)) {

        cmd.code = checkParameters(i, command, CODE_PAY);

    } else if (!strcasecmp("DESCONNECTA", word)) {

        cmd.code = checkParameters(i, command, CODE_DISCONNECT);

    } else {

        cmd.code = ERR_UNK_CMD;
    }
    free(word);
    return cmd;
}

char *readCommand() {
    return readFileDescriptor(STDIN);
}

char solveCommand(const char *command) {
    Command cmd = substractCommand(command);

    switch (cmd.code) {
        case CODE_CONNECT:
            debug("Toca connectar\n");
            print("[Comanda OK]\n");
            break;
        case CODE_SHOWMENU:
            debug("Toca mostrar el menú\n");
            print("[Comanda OK]\n");
            break;
        case CODE_REQUEST:
            debug("Toca demanar\n");
            print("[Comanda OK]\n");
            free(cmd.plat);
            break;
        case CODE_REMOVE:
            debug("Toca eliminar\n");
            print("[Comanda OK]\n");
            free(cmd.plat);
            break;
        case CODE_PAY:
            debug("Toca pagar\n");
            print("[Comanda OK]\n");
            break;
        case CODE_DISCONNECT:
            debug("Toca desconnectar\n");
            freeResources();
            return 1;
        case ERR_UNK_CMD:
            print("Comanda no reconeguda\n");
            break;
        case ERR_N_PARAMS:
            print("Nombre de paràmetres incorrecte\n");
            break;
        default:
            print("Comanda no reconeguda\n");
            break;
    }
    return 0;
}

void shell() {
    char *command;
    int flag;

    do {
        print(config.name);
        print("> ");
        command = readCommand();

        flag = solveCommand(command);

        free(command);
    } while (!flag);

}

void freeResources() {
    print("\nGràcies per fer servir LsEat. Fins la propera.\n");
    free(config.name);
    free(config.ip);
}

void controlSigint() {
    debug("\nSIGINT REBUT");
    freeResources();
    exit(EXIT_SUCCESS);
}

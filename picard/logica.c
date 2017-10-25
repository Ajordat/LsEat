#include "logica.h"


char checkParameters(int index, const char *command, char code) {
    return endOfWord(index, command) ? code : ERR_N_PARAMS;
}

Command substractCommand(const char *command) {
    int i = 0;
    char *word;
    Command cmd;

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
        if(checkNumber(word)){
            cmd.unitats = atoi(word);
            cmd.plat = getWord(&i, command);
            if(cmd.plat[0] == '\0'){
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
        if(checkNumber(word)){
            cmd.unitats = atoi(word);
            cmd.plat = getWord(&i, command);
            if(cmd.plat[0] == '\0'){
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
    char *string;
    char mychar;
    int index = 0;

    string = malloc(sizeof(char));
    while (1) {
        read(STDIN, &mychar, 1);
        string[index] = mychar;
        if (mychar == '\n') {
            string[index] = '\0';
            return string;
        }
        index++;
        string = realloc(string, sizeof(string) * (index + 1));
    }
}

int solveCommand(const char *command) {
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
            print("Gràcies per fer servir LsEat. Fins la propera.\n");
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
        print("> ");
        command = readCommand();

        flag = solveCommand(command);

        free(command);
    } while (!flag);
}

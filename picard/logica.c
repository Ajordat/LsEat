#include "logica.h"



char* readCommand(){
    char * string;
    char mychar;
    int index = 0;
    string = malloc(sizeof(char));
    while(1){
        read(STDIN, &mychar, 1);
        string[index] = mychar;
        if(mychar == '\n'){
            string[index] = '\0';
            return string;
        }
        index++;
        string = realloc(string, sizeof(string)*(index+1));
    }
}

int solveCommand(char* command){

    if(equals("CONNECTA", command)){
        debug("Toca connectar\n");
        print("[Comanda OK]\n");
        return 0;
    } else if(equals("MOSTRA MENU", command)){
        debug("Toca mostrar el menú\n");
        print("[Comanda OK]\n");
        return 0;
    } else if(beginsWith("DEMANA", command)){
        debug("Toca demanar\n");
        print("[Comanda OK]\n");
        return 0;
    } else if(beginsWith("ELIMINA", command)){
        debug("Toca eliminar\n");
        print("[Comanda OK]\n");
        return 0;
    } else if (equals("PAGAR", command)) {
        debug("Toca pagar\n");
        print("[Comanda OK]\n");
        return 0;
    } else if(equals("DESCONNECTA", command)){
        debug("Toca desconnectar\n");
        print("Gràcies per fer servir LsEat. Fins la propera.\n");
        return 1;
    } else {
        print("Comanda no reconeguda\n");
        return 0;
    }
}

void shell(){
    char * command;
    int flag;
    do{
        print("> ");
        command = readCommand();
        print(command);
        printc('\n');

        flag = solveCommand(command);
        free(command);
    }while(!flag);
}

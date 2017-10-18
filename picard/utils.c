#include "utils.h"

void debug(char * msg){
    if(DEBUG) write(STDERR_FILENO, msg, strlen(msg));
}

void print(char * msg){
    write(STDOUT_FILENO, msg, strlen(msg));
}

void printc(char mychar){
    write(STDOUT_FILENO, &mychar, 1);
}

void printi(int myint){
    char aux[12];
    sprintf(aux, "%d", myint);
    write(STDOUT_FILENO, aux, strlen(aux));
}

int beginsWith(char* prefix, char* string){
    int i, j;
    if (strlen(prefix) > strlen(string)) return 0;
    for (i = 0; string[i] == ' ' || string[i] == '\t'; i++);
    if(strlen(prefix) > (strlen(string)-i)) return 0;
    for(j = 0; prefix[j]; j++, i++) if(tolower(string[i]) != tolower(prefix[j])) return 0;
    return i;
}

int equals(char* pattern, char* string){
    int i;
    if((i = beginsWith(pattern, string))){
        for(; string[i]; i++) if(string[i] != ' ' && string[i] != '\t') return 0;
        return i;
    }
    return 0;
}

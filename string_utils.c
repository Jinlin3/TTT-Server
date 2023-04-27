#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split(char*);

int main(int argc, char** argv) {

    char** tokens;
    char string[255] = "PLAY|5|JACK|";

    tokens = split(string);

    for (int i = 0; tokens[i] != NULL; i++) {
        printf("%s\n", tokens[i]);
    }

    return 0;

}

char** split(char* string) {
    
    char** tokens = malloc(sizeof(char*) * 64);
    char* token;
    int pos = 0;

    if (!tokens) {
        printf("split function has failed");
        exit(1);
    }

    token = strtok(string, "|");
    while (token != NULL) {
        tokens[pos] = token;
        pos++;
        token = strtok(NULL, "|");
    }
    tokens[pos] = NULL;
    return tokens;

}
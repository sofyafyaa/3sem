#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void split(char* string, const char* delimiters, char*** tokens, int* tokensCount) {
    char* res = strtok(string, delimiters);

    if (res != NULL)
        *tokens =(char**) calloc(sizeof(char*), 1);

    for (*tokensCount = 0; res != NULL; (*tokensCount)++) {
        *tokens = (char**)realloc(*tokens, ((*tokensCount) + 1) * sizeof(char*));
        *(*tokens + *tokensCount) = (char*) malloc(sizeof(char) * (strlen(res) + 1));
        *(*tokens + *tokensCount) = res;

        res = strtok(NULL, delimiters);
    }
}

int main() {
    char string[256];
    gets(string);

    char** tokens; //= (char**) calloc(strlen(string), sizeof(char*));
    int tokensCount;

    split(string, " ", &tokens, &tokensCount);

    for (int i = 0; i < tokensCount; i++) {
        printf("%s\n", tokens[i]);
    }

    free(tokens);

    return 0;
}
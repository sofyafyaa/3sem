#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int mypow(int a, int x) {
    if (!x) return 1;
    return x * mypow(a, x - 1);
}


void GenerateString(int n, char* string) {
    char cnt = 'a';

    for (int i = 0; i < n; i++) {
        int len = strlen(string);
        string[len] = cnt;
        strncpy(&string[len + 1], string, len);
        cnt++;
    }

}

int main() {
    int n;
    scanf("%d", &n);

    char* str;
    for (int i = 1; i < n; i++) {
        str = (char*)calloc(mypow(2, i) - 1, 1);
        GenerateString(i, str);
    }
    printf("%s\n\n", str);
    free(str);

    return 0;
}
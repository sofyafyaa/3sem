#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


int main() {
    int N;
    scanf("%d", &N);

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if(pid == 0) {
            sleep(i + 1);
            printf("%d\n", i);
            exit(0);
        }
    }
    return 0;
}
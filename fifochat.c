#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define LEN 1000

void Read(const char* fifo_file) {
    int fd = open(fifo_file, O_RDONLY);
    char mes[LEN];
    memset(mes, '0', LEN);

    while(1) {
        int size = read(fd, mes, LEN);
        if (size <= 0) {
            printf("NO MESSEGES\n");
            exit(0);
        }
        printf(">>>%s", mes);
        memset(mes, '0', LEN);
    }
}

void Write(const char* fifo_file) {
    int fd = open(fifo_file, O_WRONLY);
    char mes[LEN];

    while(1) {
        fgets(mes, LEN, stdin);
        write(fd, mes, LEN);
        memset(mes, '0', LEN);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Choose client\n");
        exit(-1);
    }
    (void)umask(0);

    int f1 = mknod("w", S_IFIFO | 0666, 0);
    int f2 = mknod("r", S_IFIFO | 0666, 0);
	
    char* fifoRead, *fifoWrite;

    int mode = atoi(argv[1]);

    if (mode == 0) {
        fifoRead = "r";
        fifoWrite = "w";
    } 
    else {
        fifoRead = "w";
        fifoWrite = "r";
    }

    pid_t pid = fork();

    if (pid == 0) {
        Write(fifoWrite);
    } else {
        Read(fifoRead);
    }
    
    return 0;
}

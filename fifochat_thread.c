#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define LEN 1000

void* Read(void* fifo_file) {
    int fd = open((char *)fifo_file, O_RDONLY);
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

void* Write(void* fifo_file) {
    int fd = open((const char*)fifo_file, O_WRONLY);
    char mes[LEN];

    while(1) {
        fgets(mes, LEN, stdin);
        write(fd, mes, LEN);
        memset(mes, '0', LEN);
    }
}

int main(int argc, char** argv) {
    (void)umask(0);

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

    pthread_t fRead, fWrite;
    char *threadR, *threadW;

    threadR = fifoRead;
    threadW = fifoWrite;

    pthread_create(&fWrite, NULL, Write, threadW);
    pthread_create(&fRead, NULL, Read, threadR);


    pthread_join(fRead, NULL);
    
    return 0;
}
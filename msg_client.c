#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sysinfo.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct mesIn_ {
    long mtype;
    long id;
    long data[2];
} mesIn;

typedef struct mesOut_ {
    long mtype;
    long data;
} mesOut;

int main(int argc, char* argv[]) {
    int msqid;
    char pathname[] = "msg_server.c";
    key_t key = ftok(pathname, 0);

    size_t lenOut = sizeof(mesOut) - sizeof(long);
    size_t lenIn = sizeof(mesIn) - sizeof(long);

    mesIn bufIn;
    mesOut bufOut;

    bufIn.id = getpid();
    bufIn.mtype = 1;

    if (argc < 2) {
        printf("Please, enter number of requests\n");
        exit(-1);
    }

    int numRequests = atoi(argv[1]);
    printf("There're %d requests, begining...\n", numRequests);

    for (int i = 0; i < numRequests; i++) {
        scanf("%ld %ld", &((bufIn.data)[0]), &((bufIn.data)[1]));

        msgsnd(msqid, (struct msgbuf*) &bufIn, lenIn, 0);

        printf("Waiting Server....\n");

        msgrcv(msqid, (struct msgbuf*) &bufOut, lenOut, bufIn.id, 0);
  
        printf("Answer: %ld * %ld = %ld\n", bufIn.data[0], bufIn.data[1], bufOut.data);
    }

    printf("End of work\n");
    return 0;
}
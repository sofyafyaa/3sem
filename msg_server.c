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

int msqid, semid; //id of queue & semaphores

int get_sem(char* pathname, int process);
void* send_mes(void* mes);

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
    char pathname[] = "msg_server.c";
    key_t key = ftok(pathname, 0); //geting server key

    int numProcess = get_nprocs();
    get_sem(pathname, numProcess);

    msqid = msgget(key, 0666 | IPC_CREAT)

    size_t lenIn = sizeof(mesIn) - sizeof(long);

    while(1) {
        mesIn* bufIn = (mesIn*) calloc (sizeof(mesIn), 1);

        msgrcv(msqid, (struct msgbuf*) bufIn, lenIn, 1, 0);

        struct sembuf buf;
        buf.sem_flg = 0;
        buf.sem_num = 0;
        buf.sem_op = -1;
        //printf("Sem: %d\n", semctl(semid, 0, GETVAL, 0));
        semop(semid, &buf, 1);
        //printf("Sem: %d\n", semctl(semid, 0, GETVAL, 0));

        printf("Recieved message:\n type: %ld\t id: %ld\t num1: %ld\t num2: %ld\n",
                bufIn->mtype, bufIn->id, (bufIn->data)[0], (bufIn->data)[1]);

        pthread_t thid;
        pthread_create(&thid, NULL, send_mes, (void*) bufIn);

    return 0;
}

int get_sem(char* pathname, int process) {
    key_t key = ftok(pathname, 0);

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Error: can't create semaphores");
        exit(-1);
    }

    union semun {
        int val;
        struct semid_ds *buf;
        ushort* array;
    } args;
    args.val = process;
    semctl(semid, 0, SETVAL, args);

    return semid;
}

void* send_mes(void* mes) {
    mesIn* bufIn = (mesIn*) mes;
    mesOut bufOut;
    size_t lenOut = sizeof(mesOut) - sizeof(long);

    bufOut.data = (bufIn->data)[0] * (bufIn->data)[1];
    bufOut.mtype = bufIn->id;

    printf("Sending message:\ntype: %ld\t ans: %ld\n", bufOut.mtype, bufOut.data);

    msgsnd(msqid, (struct msgbuf*) (&bufOut), lenOut, 0);

    free(mes);

    struct sembuf buf;
    buf.sem_flg = 0;
    buf.sem_num = 0;
    buf.sem_op = 1;
    //printf("Sem: %d\n", semctl(semid, 0, GETVAL, 0));
    semop(semid, &buf, 1);
    //printf("Sem: %d\n", semctl(semid, 0, GETVAL, 0));

    return NULL;
}
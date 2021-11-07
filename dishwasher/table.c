#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sys/msg.h>

typedef struct message_{
    long type;
    char data;
    int inf;
} message;

int init_semaphore(char* pathname, int val) {
    key_t key = ftok(pathname, 0);
    int semid;

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Error: can't create semaphores");
        exit(-1);
    }

    union semun {
        int val;
        struct semid_ds *buf;
        ushort* array;
    } args;
    args.val = val;
    semctl(semid, 0, SETVAL, args);

    return semid;
}

int create_mes(char* pathname) {
    msgctl((int) pathname, IPC_RMID, (struct msqid_ds *) NULL);
    key_t key = ftok(pathname, 0);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    return msgid;
}

int main(int argc, char* argv[]) {
    char pathname[] = "data_table";

    size_t tablesz;
    printf("Enter table size: ");
    scanf("%ld", &tablesz);

    init_semaphore(pathname, tablesz);
    int msqid = create_mes(pathname);

    printf("Washer and dryer can start work\n");

    return 0;
}

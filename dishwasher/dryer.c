#include <sys/sem.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>

const int numTypes = 50;

typedef struct message_{
    long type;
    char data;
    int inf;
} message;

int get_mes(char* pathname) {
    key_t key = ftok(pathname, 0);
    int msgid;
    if ((msgid = msgget(key, 0666 | IPC_CREAT)) < 0){
        printf("Error: can't get msqid\n");
        exit(-1);
    }
    return msgid;
}

int get_sem(char* pathname) {
    key_t key = ftok(pathname, 0);
    int semid;

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("Error: can't create semaphore\n");
		exit(-1);
	}
	return semid;
}

void get_speeds(char* pathname, int* speeds) {
    FILE* file = fopen(pathname, "r");
    int i = 0;

    while (fscanf(file, "%c-", &i) == 1 && fscanf(file, "%d ", speeds + i) == 1)
        printf("%c - %d\n", i, speeds[i]);
    printf("\n\n");
}

char dry_dishes(int semid, int msqid) {
    message mesrcv;
    size_t messz = sizeof(message) - sizeof(long);

    if (msgrcv(msqid, (struct msgbuf*) &mesrcv, messz, 1, 0) < 0) {
        return -1;
    }

    struct sembuf buf;
    buf.sem_flg = 0;
    buf.sem_num = 0;
    buf.sem_op = 1;
    semop(semid, &buf, 1);

    printf("%c get from table\n", mesrcv.data);
    return mesrcv.data;
}

int main() {
    int* speeds = (int*)calloc(numTypes, sizeof(int)); //if speed = 0, there're no dishes of this type
    get_speeds("data_speeds", speeds); //time(wash) = time(dry)

    int semid = get_sem("data_table");
    int msqid = get_mes("data_table");

    char dish;
    while(1) {
        dish = dry_dishes(semid, msqid);
        sleep(speeds[dish]);
        printf("%c almost dryed!\n\n", dish);
    }
    return 0;
}
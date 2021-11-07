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
}

int wash_dishes(char elem, int semid, int msqid) {
    message mes;
    mes.type = 1;
    mes.data = elem;
    mes.inf = 1;
    size_t messz = sizeof(message) - sizeof(long);

    if (semctl(semid, 0, GETVAL, 0) == 0)
        printf("Table is full, waiting dryer...\n\n");

    struct sembuf buf;
    buf.sem_flg = 0;
    buf.sem_num = 0;
    buf.sem_op = -1;
    semop(semid, &buf, 1);

    if (msgsnd(msqid, (struct msgbuf*) &mes, messz, 0) < 0) {
        return 0;
    }

    printf("%c washed!\n", elem);

    return 1;
}

int main() {
    int* speeds = (int*)calloc(numTypes, sizeof(int)); //if speed = 0, there're no dishes of this type
    get_speeds("data_speeds", speeds); //time(wash) = time(dry)

    int semid = get_sem("data_table");
    int msqid = get_mes("data_table");


    FILE* file = fopen("data_dishes", "r");

    fseek(file, 0L, SEEK_END);
    size_t filesz = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);

    char* queue = (char*)calloc(filesz, sizeof(char));
    fread(queue, sizeof(char), filesz - 1, file);
    printf("queue: %s\n\n", queue);

    for (int i = 0; i < filesz; i++) {
        int delay = speeds[queue[i]];
        if (delay) { //if there is type of dishes
            sleep(delay);
            if (wash_dishes(queue[i], semid, msqid))
                printf("%c put on the table\n\n", queue[i]);
        }
    }

    printf("Washer ended\n");
    free(queue);
    return 0;
}
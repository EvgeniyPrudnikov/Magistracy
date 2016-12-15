#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>


struct shared_data
{
    char text[2048];
};

int main() {

    int shmid = shmget((key_t)1234, sizeof(struct shared_data), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        return 1;
    }

    void *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == NULL)
    {
        fprintf(stderr, "shmat failed\n");
        return 1;
    }

    // create sems
    int semId = semget((key_t) 123, 2, 0666 | IPC_CREAT);
    if (semId == -1)
    {
        fprintf(stderr, "semget failed\n");
        return 1;
    }

    // init sems
    if(semctl( semId, 0, SETVAL, 0) == -1)
    {
        fprintf(stderr, "semctl failed for 0 sem\n");
        return 1;
    }
    if(semctl( semId, 1, SETVAL, 0) == -1)
    {
        fprintf(stderr, "semctl failed for 1 sem\n");
        return 1;
    }

    struct shared_data *sharedData = (struct shared_data *)sharedMemory;

    struct sembuf* operation = (struct sembuf*)malloc(sizeof(struct sembuf));

    printf("Server running...\n");
    while (1)
    {
        operation->sem_num = 1;
        operation->sem_op = -1;
        operation->sem_flg = 0;

        printf("Waiting for a client...\n");
        if (semop( semId, operation, 1 ) == -1) {
            fprintf(stderr, "semop failed\n");
            return 1;
        }

        printf("Client: %s", sharedData->text);

        operation->sem_num = 0;
        operation->sem_op = -1;
        operation->sem_flg = 0;

        if (semop( semId, operation, 1 ) == -1) {
            fprintf(stderr, "semop failed\n");
            return 1;
        }
    }
}
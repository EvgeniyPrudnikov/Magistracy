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

    int shmid = shmget((key_t) 1234, sizeof(struct shared_data), 0666);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        return 1;
    }

    void *sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == NULL)
    {
        fprintf(stderr, "shmat failed\n");
        
    }

    int semId = semget((key_t) 123, 2, 0666);
    if (semId == -1)
    {
        fprintf(stderr, "semget failed\n");
        return 1;
    }

    struct sembuf operations[2];

    while(1)
    {

        operations[0].sem_num = 0;
        operations[0].sem_op = 0;
        operations[0].sem_flg = 0;

        operations[1].sem_num = 0;
        operations[1].sem_op = 1;
        operations[1].sem_flg = 0;


        printf("Waiting for a server.\n");
        if (semop(semId, operations, 2) == -1) {
            fprintf(stderr, "semop failed\n");
            return 1;
        }

        struct shared_data *sharedData = (struct shared_data *) sharedMemory;

        printf("Enter text: ");
        fgets(sharedData->text, BUFSIZ, stdin);

        operations[0].sem_num = 1;
        operations[0].sem_op = 1;
        operations[0].sem_flg = 0;

        if (semop(semId, operations, 1) == -1) {
            fprintf(stderr, "semop failed\n");
            return 1;
        }
    }
}
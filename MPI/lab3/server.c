#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>

struct shared_data
{
    char text[2048];
};

char SEM_NAME[] = "mySem";

int main()
{

    int shmid;
    key_t key;
    sem_t *mutex;

    //name the shared memory segment
    key = 99999;

    //create & initialize semaphore
    mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED)
    {
        perror("unable to create semaphore");
        sem_unlink(SEM_NAME);
        return 1;
    }

    //create the shared memory segment with this key
    shmid = shmget(key, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("failure in shmget");
        return 1;
    }

    //attach this segment to virtual memory
    void *sharedMemory = shmat(shmid, NULL, 0);

    if (sharedMemory == NULL)
    {
        perror("shmat");
        return 1;
    }

    struct shared_data *sharedData = (struct shared_data *) sharedMemory;

    //start writing into memory
    printf("Waiting for clients\n");
    while (1)
    {
        sem_trywait(mutex);

        if (sharedData->text[0] != '\0')
        {
            printf("Client: %s", sharedData->text);
            sharedData->text[0] = '\0';
        }
        sem_post(mutex);
    }
}
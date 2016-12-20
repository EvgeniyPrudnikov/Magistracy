#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

struct shared_data
{
    char text[2048];
};

char SEM_NAME[] = "vik";

int main()
{
    char ch;
    int shmid;
    key_t key;
    sem_t *mutex;

    //name the shared memory segment
    //key = ftok("file",4684654);
    int l;
    sem_close(mutex);
    sem_getvalue(mutex,&l);
    //create & initialize existing semaphore
    mutex = sem_open(SEM_NAME, 0, 0644, 0);
    if (mutex == SEM_FAILED)
    {
        perror("reader:unable to execute semaphore");
        sem_close(mutex);
        exit(-1);
    }

    //create the shared memory segment with this key
    shmid = shmget(99999, sizeof(struct shared_data), 0666);
    if (shmid < 0)
    {
        perror("reader:failure in shmget");
        exit(-1);
    }

    //attach this segment to virtual memory
    void *sharedMemory = shmat(shmid, NULL, 0);


    while (1)
    {
        sem_trywait(mutex);

        struct shared_data *sharedData = (struct shared_data *) sharedMemory;

        printf("Enter text: ");
        fgets(sharedData->text, BUFSIZ, stdin);
        sem_post(mutex);
    }
}
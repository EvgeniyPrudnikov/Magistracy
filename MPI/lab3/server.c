#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <zconf.h>
#include <time.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t exit_sig = 0;

void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
        exit_sig = 1;
}

struct shared_data
{
    char text[2048];
};

char SEM_NAME[] = "Sem";

int main()
{


    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sig_handler;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);


    int shmid;
    key_t key;
    sem_t *mutex;

    //name the shared memory segment
    key = 99996;

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

    //start read from memory
    printf("Waiting for clients\n");
    while (1)
    {
        sem_wait(mutex);
        if (sharedData->text[0] != '\0')
        {
            printf("Client: %s", sharedData->text);
            sharedData->text[0] = '\0';
        }
        sem_post(mutex);
        usleep(100);

        if (exit_sig)
        {
            sem_close(mutex);
            return 0;
        }
    }
}

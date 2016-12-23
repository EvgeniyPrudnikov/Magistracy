#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>


volatile sig_atomic_t exit_sig = 0;

void sig_handler(int signo)
{
    if (signo == SIGTERM || signo == SIGINT)
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

    //create & initialize existing semaphore
    mutex = sem_open(SEM_NAME, 0, 0644, 0);
    if (mutex == SEM_FAILED)
    {
        perror("reader:unable to execute semaphore");
        sem_close(mutex);
        return 1;
    }

    //create the shared memory segment with this key
    shmid = shmget(key, sizeof(struct shared_data), 0666);
    if (shmid < 0)
    {
        perror("reader:failure in shmget");
        return 1;
    }

    //attach this segment to virtual memory
    void *sharedMemory = shmat(shmid, NULL, 0);

    if (sharedMemory == NULL)
    {
        perror("shmat");
        return 1;
    }

    while (1)
    {
        sem_wait(mutex);
        struct shared_data *sharedData = (struct shared_data *) sharedMemory;
        printf("Enter text: ");
        fgets(sharedData->text, BUFSIZ, stdin);
        sem_post(mutex);
        usleep(100);

        if (exit_sig)
        {
            sem_close(mutex);
            return 0;
        }
    }
}
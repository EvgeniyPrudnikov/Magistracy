#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int a = -5;
int b = 5;



double f(double x)
{
    return x * x;
}


int main(int argc, char *argv[])
{
    sem_t mutex;
    int pid;
    int left, right;

    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments!");
        return 1;
    }

    int numberOfProcesses = atoi(argv[1]);
    int n = atoi(argv[2]);

    double step = (b - a) / (double) n;

    int pipefdls[2];
    if (pipe(pipefdls) == -1)
    {
        perror("pipe");
        return 1;
    }

    int cntStepsPerProc = ((b - a) % numberOfProcesses) ?
                            (b - a) / numberOfProcesses + 1 : (b - a) / numberOfProcesses;




    /* create, initialize semaphore */
    if( sem_init(&mutex,1,1) < 0)
    {
        perror("semaphore initilization");
        exit(0);
    }

    for (int procNum = 0; procNum < numberOfProcesses; procNum++)
    {
        left = a + procNum * cntStepsPerProc;
        right = (a + (procNum + 1) * cntStepsPerProc < b) ? a + (procNum + 1) * cntStepsPerProc : b;

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 1;
        } else if (pid == 0)
        {
            break;
        }
    }

    if (pid == 0)
    {
        close(pipefdls[0]);

        double sum = 0.;

        for (double i = left; i < right; i += step)
        {
            sum += f(i);
        }
        sum = (step / 2) * (f(left) + 2 * sum + f(right));

        sem_wait(&mutex);
        write(pipefdls[1], &sum, sizeof(double));
        sem_post(&mutex);

        return 0;

    } else
    {
        close(pipefdls[1]);

        double singleResult;
        double result = 0;

        for (int i = 0; i < numberOfProcesses; i++)
        {
            sem_wait(&mutex);
            read(pipefdls[0], &singleResult, sizeof(double));
            sem_post(&mutex);
            result += singleResult;

        }

        printf("%f\n", result);
        wait(NULL);
    }
    sem_close(&mutex);
    return 0;
}

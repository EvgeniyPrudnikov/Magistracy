#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>

int a = -5;
int b = 5;

double f(double x)
{
    return x * x;
}


int main(int argc, char *argv[])
{

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

    int semId = semget((key_t) 123, 2, 0666);
    if (semId == -1)
    {
        fprintf(stderr, "semget failed\n");
        return 1;
    }

    struct sembuf operations[2];



    int cntStepsPerProc = ((b - a) % numberOfProcesses) ?
                            (b - a) / numberOfProcesses + 1 : (b - a) / numberOfProcesses;

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

        operations[0].sem_num = 0;
        operations[0].sem_op = 0;
        operations[0].sem_flg = 0;

        operations[1].sem_num = 0;
        operations[1].sem_op = 1;
        operations[1].sem_flg = 0;

        write(pipefdls[1], &sum, sizeof(double));

        operations[0].sem_num = 1;
        operations[0].sem_op = 1;
        operations[0].sem_flg = 0;

    } else
    {
        close(pipefdls[1]);

        double singleResult;
        double result = 0;

        operations[0].sem_num = 0;
        operations[0].sem_op = 0;
        operations[0].sem_flg = 0;

        operations[1].sem_num = 0;
        operations[1].sem_op = 1;
        operations[1].sem_flg = 0;

        for (int i = 0; i < numberOfProcesses; i++)
        {
            read(pipefdls[0], &singleResult, sizeof(double));
            result += singleResult;
        }

        operations[0].sem_num = 1;
        operations[0].sem_op = 1;
        operations[0].sem_flg = 0;

        printf("%f", result);
        wait(NULL);
    }

    return 0;
}

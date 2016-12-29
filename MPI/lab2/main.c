#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double a = 1;
double b = 101;
//double step;
double result;

pthread_mutex_t mutex;

struct thdata {
    double left;
    double right;
    long numn;
};

double f (double x)
{
    return x*x;
}

void* worker(void *ptrData)
{
    struct thdata *data = (struct thdata *)ptrData;

    double step = (data->right - data->left)/data->numn;
    double sum = 0.;

    for (double i = data->left + step; i < data->right; i+=step)
    {
        sum+= f(i);
    }
    sum = (step/2) * (f(data->left) + 2*sum + f(data->right));

    if (pthread_mutex_lock(&mutex)) {
        perror("pthread_mutex_lock");
    }

    result += sum;

    if (pthread_mutex_unlock(&mutex)) {
        perror("pthread_mutex_unlock");
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{

    int numberOfThreads = atoi(argv[1]);
    long n = atol(argv[2]);

    result = 0;

    pthread_t threads[numberOfThreads];

    struct thdata thr_data[numberOfThreads];

    if (pthread_mutex_init(&mutex, NULL)) {
        perror("pthread_mutex_init");
        return 1;
    }

    double cntStepsPerThread = ((b - a) / (double)numberOfThreads);

    for (int i = 0; i < numberOfThreads; ++i)
    {
        thr_data[i].numn = n;
        thr_data[i].left = a + i * cntStepsPerThread;
        thr_data[i].right = (a + (i + 1) * cntStepsPerThread < b) ? a + (i + 1) * cntStepsPerThread : b;

        if((pthread_create(&threads[i],NULL,worker,&thr_data[i])))
        {
            perror("pthread_create");
            return 2;
        }
    }

    for (int j = 0; j <numberOfThreads; ++j)
    {
        if (pthread_join(threads[j], NULL)) {
            perror("pthread_join");
            return 2;
        }
    }

    printf("%f",result);

    if (pthread_mutex_destroy(&mutex)) {
        perror("pthread_mutex_destroy");
        return 2;
    }


    return 0;
}
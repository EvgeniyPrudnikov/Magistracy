#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long a = -5;
long b = 5;
double step;
double result;

pthread_mutex_t mutex;

struct thdata {
    long left;
    long right;
    double step;
};

double f (double x)
{
    return x*x;
}

void* worker(void *ptrData)
{
    struct thdata *data = (struct thdata *)ptrData;

    double sum = 0.;
    for (double i = data->left; i < data->right; i+=data->step)
    {
        sum+= f(i);
    }
    sum = (step/2) * (f(data->left) + 2*sum + f(data->right));

    pthread_mutex_lock(&mutex);
    result += sum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{

    int numberOfThreads = atoi(argv[1]);
    int n = atoi(argv[2]);

    step = (b-a)/(double)n;

    result = 0;

    pthread_t threads[numberOfThreads];
    int rc;
    struct thdata thr_data[numberOfThreads];

    pthread_mutex_init(&mutex,NULL);

    long cntStepsPerThread = ((b - a) % numberOfThreads) ?
                            (b - a) / numberOfThreads + 1 : (b - a) / numberOfThreads;

    for (int i = 0; i < numberOfThreads; ++i)
    {
        thr_data[i].step = step;
        thr_data[i].left = a + i * cntStepsPerThread;
        thr_data[i].right = (a + (i + 1) * cntStepsPerThread < b) ? a + (i + 1) * cntStepsPerThread : b;

        if((rc = pthread_create(&threads[i],NULL,worker,&thr_data[i])))
        {
            fprintf(stderr, "can not create thread, rc %d\n", rc);
            return 2;
        }
    }

    for (int j = 0; j <numberOfThreads; ++j)
    {
        pthread_join(threads[j],NULL);
    }

    printf("%f",result);


    return 0;
}
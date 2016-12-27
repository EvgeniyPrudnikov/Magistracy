#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zconf.h>

void InitMatrix(int *matr, int n)
{
    srand((unsigned) time(NULL));
    int c = 1;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            matr[i * n + j] = c++;
        }
    }
}

void PrintMatrix(int *matr, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%d ", matr[i * n + j]);
        }
        printf("\n");
    }
    printf("\n");
}


void PrintTape(int *T_local, int m, int n)
{
    for (int v = 0; v < m; v++)
    {
        for (int u = 0; u < n; u++)
        {
            printf("%d ", T_local[v * n + u]);
        }
        printf("\n");
    }
    printf("\n");
}


void Mult(const int *__restrict a, const int *__restrict b, int *__restrict c, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                c[i * m + j] += a[i * n + k] * b[j + k * m];
            }
        }
    }
}


int main(int argc, char **argv)
{
    int N = atoi(argv[1]);

    int num_of_procs;
    int proc_rank;
    MPI_Status stat;
    int *gdispls, *reccount;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);


    int max_rank = num_of_procs - 1;
    int rows_per_p = N / num_of_procs;

    int *A;
    int *B;
    int *C;


    int *A_local = (int *) malloc(N * rows_per_p * sizeof(int));
    int *B_local = (int *) malloc(N * rows_per_p * sizeof(int));
    int *C_local = (int *) calloc((size_t) rows_per_p * rows_per_p, sizeof(int));

    MPI_Datatype acol, acoltype, bcol, bcoltype;

    if (proc_rank == 0)
    {
        A = (int *) malloc(N * N * sizeof(int));
        B = (int *) malloc(N * N * sizeof(int));
        C = (int *) calloc((size_t) N * N, sizeof(int));

        InitMatrix(A, N);
        InitMatrix(B, N);

        MPI_Type_vector(N, 1, N, MPI_INT, &acol);
        MPI_Type_commit(&acol);
        MPI_Type_create_resized(acol, 0, 1 * sizeof(int), &acoltype);
        MPI_Type_commit(&acoltype);

        /*gdispls = (int *) calloc(num_of_procs, sizeof(int));
        gdispls[0] = 0;
        gdispls[1] = N;
        reccount = (int *) calloc(num_of_procs, sizeof(int));
        reccount[0] = rows_per_p*rows_per_p;
        reccount[1] = rows_per_p*rows_per_p;
         */
    }

    MPI_Type_vector(N, 1, rows_per_p, MPI_FLOAT, &bcol);
    MPI_Type_commit(&bcol);
    MPI_Type_create_resized(bcol, 0, 1 * sizeof(int), &bcoltype);
    MPI_Type_commit(&bcoltype);


    MPI_Scatter(B, rows_per_p, acoltype, B_local, rows_per_p, bcoltype, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, N * rows_per_p, MPI_INT, A_local, N * rows_per_p, MPI_INT, 0, MPI_COMM_WORLD);

    Mult(A_local, B_local, C_local, rows_per_p, N);

    //MPI_Gatherv(C_local, rows_per_p * rows_per_p, MPI_INT, C, reccount, gdispls, MPI_INT, 0, MPI_COMM_WORLD);

    if (proc_rank == 0){
        PrintMatrix(C,N);
    }

    for (int i = 0; i < num_of_procs - 1; ++i)
    {

        int next_proc = proc_rank + 1;
        int prev_proc = proc_rank - 1;

        MPI_Send(B_local, N * rows_per_p, MPI_INT, proc_rank == max_rank ? 0 : next_proc, i,
                 MPI_COMM_WORLD);

        MPI_Recv(B_local, N * rows_per_p, MPI_INT, proc_rank == 0 ? max_rank : prev_proc, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &stat);

        /*if (proc_rank == 1)
        {
            PrintTape(B_local, rows_per_p, N);
        }*/
    }


    MPI_Finalize();
    return 0;
}

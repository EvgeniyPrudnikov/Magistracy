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


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int rows_per_p = N / num_of_procs; // 2

    int *A_local = (int *) calloc(N * rows_per_p, sizeof(int));
    int *B_local = (int *) calloc(N * rows_per_p, sizeof(int));
    int *C_local = (int *) calloc(rows_per_p * rows_per_p, sizeof(int));


    MPI_Datatype acol, acoltype, bcol, bcoltype;

    if (proc_rank == 0)
    {

        int *A = (int *) malloc(N * N * sizeof(int));
        int *B = (int *) malloc(N * N * sizeof(int));
        int *C = (int *) calloc(N * N, sizeof(int));

        InitMatrix(A, N);
        InitMatrix(B, N);


        Mult(A,B,C,N,N);
        PrintMatrix(C, N);


        MPI_Type_vector(N, 1, N, MPI_INT, &acol);
        MPI_Type_commit(&acol);
        MPI_Type_create_resized(acol, 0, 1 * sizeof(int), &acoltype);


        MPI_Type_vector(N, 1, rows_per_p, MPI_INT, &bcol);

        MPI_Type_commit(&bcol);
        MPI_Type_create_resized(bcol, 0, 1 * sizeof(int), &bcoltype);
        MPI_Type_commit(&bcoltype);


        MPI_Scatter(B, rows_per_p, acoltype, B_local, rows_per_p, bcoltype, 0, MPI_COMM_WORLD);
        MPI_Scatter(A, N * rows_per_p, MPI_INT, A_local, N * rows_per_p, MPI_INT, 0, MPI_COMM_WORLD);

        printf("\n");

    } else
    {
        MPI_Type_vector(N, 1, rows_per_p, MPI_FLOAT, &bcol);

        MPI_Type_commit(&bcol);
        MPI_Type_create_resized(bcol, 0, 1 * sizeof(int), &bcoltype);
        MPI_Type_commit(&bcoltype);

        MPI_Scatter(NULL, rows_per_p, acoltype, B_local, rows_per_p, bcoltype, 0, MPI_COMM_WORLD);

        MPI_Scatter(NULL, N * rows_per_p, MPI_INT, A_local, N * rows_per_p, MPI_INT, 0, MPI_COMM_WORLD);
        PrintTape(B_local, rows_per_p, N);
        PrintTape(A_local, rows_per_p, N);

        Mult(A_local,B_local,C_local,rows_per_p,N);

        PrintMatrix(C_local,rows_per_p);

    }
    MPI_Finalize();
    return 0;
}

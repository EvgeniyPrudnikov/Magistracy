
#define ROOT 0

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zconf.h>


void InitMatrix(double *matr, int n)
{
    int c = 1;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            matr[i * n + j] = c;
        }
    }
}

void PrintMatrix(double *matr, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%10.2f ", matr[i * n + j]);
        }
        printf("\n");
    }
    printf("\n");
}


void PrintTape(double *T_local, int m, int n)
{
    for (int v = 0; v < m; v++)
    {
        for (int u = 0; u < n; u++)
        {
            printf("%8.2f ", T_local[v * n + u]);
        }
        printf("\n");
    }
    printf("\n");
}


void Mult(const double *__restrict a, const double *__restrict b, double *__restrict c, int disp, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0, d = disp * m; j < m, d < m * (disp + 1); ++j, ++d)
        {
            for (int k = 0; k < n; ++k)
            {
                c[i * n + d] += a[i * n + k] * b[j + k * m];
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


    int MAX_RANK = num_of_procs - 1;
    int rows_cols_per_p = N / num_of_procs;

    double *Matrix_A;
    double *Matrix_B;
    double *Matrix_C;

    double *Matrix_A_local = (double *) malloc(N * rows_cols_per_p * sizeof(double));
    double *Matrix_B_local = (double *) malloc(N * rows_cols_per_p * sizeof(double));
    double *Matrix_C_local = (double *) calloc((size_t) rows_cols_per_p * N, sizeof(double));

    MPI_Datatype column_t, column_t_res;

    if (proc_rank == ROOT)
    {
        Matrix_A = (double *) malloc(N * N * sizeof(double));
        Matrix_B = (double *) malloc(N * N * sizeof(double));
        Matrix_C = (double *) calloc((size_t) N * N, sizeof(double));

        srand((unsigned) time(NULL));
        InitMatrix(Matrix_A, N);
        InitMatrix(Matrix_B, N);

        printf("Matrix A:\n");
        PrintMatrix(Matrix_A, N);
        printf("Matrix B:\n");
        PrintMatrix(Matrix_B, N);

        MPI_Type_vector(N, 1, N, MPI_DOUBLE, &column_t);
        MPI_Type_commit(&column_t);
        MPI_Type_create_resized(column_t, 0, 1 * sizeof(double), &column_t_res);
        MPI_Type_commit(&column_t_res);
    }

    MPI_Scatter(Matrix_B, rows_cols_per_p, column_t_res, Matrix_B_local, N * rows_cols_per_p, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Scatter(Matrix_A, N * rows_cols_per_p, MPI_DOUBLE, Matrix_A_local, N * rows_cols_per_p, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    Mult(Matrix_A_local, Matrix_B_local, Matrix_C_local, proc_rank, rows_cols_per_p, N);


    int next_proc = proc_rank == MAX_RANK ? ROOT : proc_rank + 1;
    int prev_proc = proc_rank == ROOT ? MAX_RANK : proc_rank - 1;
    int proc_disp = prev_proc;

    for (int i = 0; i < num_of_procs - 1; ++i)
    {
        MPI_Sendrecv_replace(Matrix_B_local, N * rows_cols_per_p, MPI_DOUBLE, next_proc, i, prev_proc, i, MPI_COMM_WORLD, &stat);

        Mult(Matrix_A_local, Matrix_B_local, Matrix_C_local, proc_disp, rows_cols_per_p, N);

        proc_disp = proc_disp == ROOT ? MAX_RANK : proc_disp - 1;
    }

    MPI_Gather(Matrix_C_local, N * rows_cols_per_p, MPI_DOUBLE, Matrix_C, N * rows_cols_per_p, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    if (proc_rank == ROOT)
    {
        printf("Result matrix C:\n");
        PrintMatrix(Matrix_C, N);
    }

    MPI_Finalize();
    return 0;
}

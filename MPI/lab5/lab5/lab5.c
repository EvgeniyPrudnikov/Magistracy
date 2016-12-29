#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


void PrintMatrix(int *matr, int n,int m)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            printf("%2i", matr[i * m + j]);
        }
        printf("\n");
    }
    printf("\n");
}


const int block_size = 2;

int main(int argc, char **argv)
{
    int N1 = atoi(argv[1]);
    int N2 = atoi(argv[2]);

    int num_of_procs;
    int proc_rank;
    MPI_Status stat;
    MPI_Request req;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int rows_per_p = N1 / num_of_procs; // 2
    int num_of_blk_per_p = N2 / block_size; //5

    int *Matrix;
    int *p_tape = (int *) calloc(N2 * rows_per_p, sizeof(int));


    if (proc_rank == 0)
    {
        Matrix = (int *) calloc(N1 * N2, sizeof(int));

        for (int k = 0; k < num_of_blk_per_p; k++)
        {
            for (int i = 1; i < rows_per_p; i++)
            {
                for (int j = k * block_size; j < (k + 1) * block_size; j++)
                {
                    p_tape[i * N2 + j] = p_tape[(i - 1) * N2 + j] + 1;
                }
            }
            MPI_Isend(&(p_tape[(rows_per_p - 1) * N2]) + k * block_size, block_size, MPI_INT, proc_rank + 1, k,
                      MPI_COMM_WORLD, &req);
        }

    } else if (proc_rank == num_of_procs - 1)
    {
        for (int k = 0; k < num_of_blk_per_p; k++)
        {
            int buff_res[block_size];
            MPI_Recv(buff_res, block_size, MPI_INT, proc_rank - 1, k, MPI_COMM_WORLD, &stat);

            for (int i = 0; i < rows_per_p; i++)
            {
                for (int j = k * block_size, c = 0; j < (k + 1) * block_size, c < block_size; j++, c++)
                {
                    if (i == 0)
                    {
                        p_tape[i * N2 + j] = buff_res[c] + 1;
                    } else
                    {
                        p_tape[i * N2 + j] = p_tape[(i - 1) * N2 + j] + 1;
                    }
                }
            }
        }

    } else
    {
        for (int k = 0; k < num_of_blk_per_p; k++)
        {
            int buff_res[block_size];
            MPI_Recv(buff_res, block_size, MPI_INT, proc_rank - 1, k, MPI_COMM_WORLD, &stat);

            for (int i = 0; i < rows_per_p; i++)
            {
                for (int j = k * block_size, c = 0; j < (k + 1) * block_size, c < block_size; j++, c++)
                {
                    if (i == 0)
                    {
                        p_tape[i * N2 + j] = buff_res[c] + 1;

                    } else
                    {
                        p_tape[i * N2 + j] = p_tape[(i - 1) * N2 + j] + 1;
                    }
                }
            }
            MPI_Isend(&p_tape[(rows_per_p - 1) * N2] + k * block_size, block_size, MPI_INT, proc_rank + 1, k,
                      MPI_COMM_WORLD, &req);
        }
    }

    MPI_Gather(p_tape, N2 * rows_per_p, MPI_INT, Matrix, N2 * rows_per_p, MPI_INT, 0, MPI_COMM_WORLD);

    if (proc_rank == 0)
    {
        PrintMatrix(Matrix, N1, N2);
    }

    MPI_Finalize();
    return 0;
}

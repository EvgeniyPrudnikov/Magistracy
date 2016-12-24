#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
    int N = atoi(argv[1]);

    int num_of_procs;
    int proc_rank;
    MPI_Status stat;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    /*int **p_tape = (int **)calloc(rows_per_p, sizeof(int));
    if (!p_tape)
    {
        printf("bad alloc");
        return 2;
    }
    for (int i = 0; i < rows_per_p; i++)
    {
        p_tape[i] = (int *)calloc(N2, sizeof(int));

        if (!p_tape[i])
        {
            printf("bad alloc");
            return 2;
        }
    }
*/
    if (proc_rank == 0)
    {

    }
    else if (proc_rank == num_of_procs - 1)
    {
        for (int k = 0; k < num_of_blk_per_p; k++)
        {
            int buff_res[block_size];
            MPI_Recv(buff_res, block_size, MPI_INT, proc_rank - 1, k, MPI_COMM_WORLD, &stat);

            for (int i = 0; i < rows_per_p; i++)
            {
                for (int j = k*block_size, c = 0; j < (k + 1)*block_size, c < block_size; j++, c++)
                {
                    if (i == 0)
                    {
                        p_tape[i][j] = buff_res[c] + 1;
                    }
                    else
                    {
                        p_tape[i][j] = p_tape[i - 1][j] + 1;
                    }
                }
            }
        }

        for (int v = 0; v < rows_per_p; v++)
        {
            for (int u = 0; u < N2; u++)
            {
                printf("%d ", p_tape[v][u]);
            }
            printf("\n");
        }
        printf("\n");
    }
    else
    {
        for (int k = 0; k < num_of_blk_per_p; k++)
        {
            int buff_res[block_size];
            MPI_Recv(buff_res, block_size, MPI_INT, proc_rank - 1, k, MPI_COMM_WORLD, &stat);

            for (int i = 0; i < rows_per_p; i++)
            {
                for (int j = k*block_size, c = 0; j < (k + 1)*block_size, c < block_size; j++, c++)
                {
                    if (i == 0)
                    {
                        p_tape[i][j] = buff_res[c] + 1;
                    }
                    else
                    {
                        p_tape[i][j] = p_tape[i - 1][j] + 1;
                    }
                }
            }
            MPI_Isend(p_tape[rows_per_p - 1] + k*block_size, block_size, MPI_INT, proc_rank + 1, k, MPI_COMM_WORLD, &req);
        }

        for (int v = 0; v < rows_per_p; v++)
        {
            for (int u = 0; u < N2; u++)
            {
                printf("%d ", p_tape[v][u]);
            }
            printf("\n");
        }
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}

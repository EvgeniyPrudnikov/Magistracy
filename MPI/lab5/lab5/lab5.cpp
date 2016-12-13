#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int N1 = 10;
const int N2 = 10;

const int block_size = 2;

int main(int argc, char** argv)
{
	int num_of_procs;
	int proc_rank;
	MPI_Status stat;
	MPI_Request req;

	printf("lols");

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);



	int rows_per_p = N1 / num_of_procs; // 2
	int num_of_blk_per_p = N2 / block_size; //5

	int **p_tape = (int **)malloc(rows_per_p * sizeof(int));

	for (int i = 0; i < rows_per_p; i++)
	{
		p_tape[i] = (int *)malloc(N2 * sizeof(int));
	}


	if (proc_rank == 0)
	{
		for (int k = 0; k < num_of_blk_per_p; k++)
		{
			for (int i = 1; i < rows_per_p; i++)
			{
				for (int j = 0; j < block_size; j++)
				{
					p_tape[i][j] = p_tape[i - 1][j] + 1;
				}
			}
			MPI_Send(p_tape[rows_per_p - 1] + k*block_size * sizeof(int), block_size, MPI_INT, proc_rank + 1, proc_rank + 1, MPI_COMM_WORLD /*, &req*/);
		}

		for (int v = 0; v < rows_per_p; v++)
		{
			for (int u = 0; u < N2; u++)
			{
				printf("%d ", p_tape[v][u]);
			}
			printf("\n");
		}
	}
	else if (proc_rank == num_of_procs - 1)
	{
		for (int k = 0; k < num_of_blk_per_p; k++)
		{
			int buff[block_size];
			MPI_Recv(buff, block_size, MPI_INT, proc_rank - 1, proc_rank - 1, MPI_COMM_WORLD, &stat);

			for (int i = 0; i < rows_per_p; i++)
			{
				for (int j = 0; j < block_size; j++)
				{
					if (i == 0)
					{
						p_tape[i][j] = buff[j] + 1;
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

	}
	else
	{
		for (int k = 0; k < num_of_blk_per_p; k++)
		{
			int buff[block_size];
			MPI_Recv(buff, block_size, MPI_INT, proc_rank - 1, proc_rank - 1, MPI_COMM_WORLD, &stat);

			for (int i = 0; i < rows_per_p; i++)
			{
				for (int j = 0; j < block_size; j++)
				{
					if (i == 0)
					{
						p_tape[i][j] = buff[j] + 1;
					}
					else
					{
						p_tape[i][j] = p_tape[i - 1][j] + 1;
					}
				}
			}

			MPI_Send(p_tape[rows_per_p - 1] + k*block_size * sizeof(int), block_size, MPI_INT, proc_rank + 1, proc_rank + 1, MPI_COMM_WORLD/*, &req*/);
		}

		for (int v = 0; v < rows_per_p; v++)
		{
			for (int u = 0; u < N2; u++)
			{
				printf("%d ", p_tape[v][u]);
			}
			printf("\n");
		}
	}

	free(p_tape);

	MPI_Finalize();
	//getchar();
	return 0;
}

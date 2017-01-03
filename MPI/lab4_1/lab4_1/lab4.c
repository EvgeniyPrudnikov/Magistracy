#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{

	int world_size;
	int world_rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Status stat;

	if (world_rank == 0)
	{

		char **buff = (char**)malloc((world_size - 1) * sizeof(char*));

		for (int i = 0; i < world_size - 1; i++)
		{
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			int n;
			MPI_Get_count(&stat, MPI_CHAR, &n);
			buff[stat.MPI_SOURCE -1] = (char *)malloc(n * sizeof(char));
			MPI_Recv(buff[stat.MPI_SOURCE - 1], n, MPI_CHAR, stat.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
		}

		for (int i = 0; i < world_size - 1; i++)
		{
			printf(buff[i]);
		}
	}
	else
	{
		char buf[25];
		int len = sprintf(buf, "Hello from %i\n", world_rank);
		MPI_Send(buf, len + 1, MPI_CHAR, 0, world_rank, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

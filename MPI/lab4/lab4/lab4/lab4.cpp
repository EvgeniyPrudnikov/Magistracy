#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	// Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	MPI_Status stat;

	if (world_rank == 0)
	{
		for (int i = 1; i < world_size; i++)
		{
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			int n;
			MPI_Get_count(&stat, MPI_CHAR, &n);
			char* buf = new char [n];
			MPI_Recv(buf, n, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			printf(buf);
		}
	}
	else
	{
		char buf[25];
		int len = sprintf(buf, "Hello from %d\n", world_rank);
		MPI_Send(buf, len + 1, MPI_CHAR, 0, world_rank, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

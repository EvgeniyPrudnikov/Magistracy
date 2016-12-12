#include <iostream>
#include "mpi.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	int rank, size;

	MPI_Init(&argc, &argv);						/* starts MPI */

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */
	MPI_Status stat;

	printf("lol");

	if (rank == 0)
	{
		printf("from 0");
		for (int i = 1; i < size; i++)
		{
			char buf[50];
			//MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			MPI_Recv(buf, 50, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			printf(buf);
		}
	}
	else 
	{
		for (int i = 1; i < size; i++)
		{
			char buf[25];
			int len = sprintf(buf, "Hello from %d", i);
			MPI_Send(buf, len + 1, MPI_CHAR, 0, i, MPI_COMM_WORLD);
			printf("sended from %d", i);
		}
	}

	MPI_Finalize();
	return 0;
}
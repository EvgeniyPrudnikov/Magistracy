#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

int procNum;
int procRank;

int gridSize;
int gridCoords[2]; // координыты процессора в решетке

MPI_Comm gridComm; // коммуникатор сетки
MPI_Comm colComm; // коммуникатор столбцов
MPI_Comm rowComm; // коммуникатор строк


				  // Делим матрицы A и B между процессорами
void distributeData(double *a, double *b, double *aBlock, double *bBlock, int n, int blockSize) 
{
	double *aRow = new double[blockSize * n];
	double *bRow = new double[blockSize * n];

	if (gridCoords[1] == 0) 
	{
		MPI_Scatter(a, blockSize * n, MPI_INT, aRow, blockSize * n, MPI_INT, 0, colComm);
		MPI_Scatter(b, blockSize * n, MPI_INT, bRow, blockSize * n, MPI_INT, 0, colComm);
	}

	for (int i = 0; i < blockSize; i++) 
	{
		MPI_Scatter(&aRow[i * n], blockSize, MPI_INT, &(aBlock[i * blockSize]), blockSize, MPI_INT, 0, rowComm);
		MPI_Scatter(&bRow[i * n], blockSize, MPI_INT, &(bBlock[i * blockSize]), blockSize, MPI_INT, 0, rowComm);
	}
}

// Собираем матрицу С
void collectData(double *c, double *cBlock, int n, int blockSize) 
{
	double *cRow = new double[blockSize * n];
	for (int i = 0; i < blockSize; i++) 
	{
		MPI_Gather(&cBlock[i * blockSize], blockSize, MPI_INT, &cRow[i * n], blockSize, MPI_INT, 0, rowComm);
	}

	if (gridCoords[1] == 0) 
	{
		MPI_Gather(cRow, blockSize * n, MPI_INT, c, blockSize * n, MPI_INT, 0, colComm);
	}
}

// Умножение матриц
void multiplyMatrix(double *a, double *b, double *c, int n) 
{
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++) 
		{
			for (int k = 0; k < n; k++) 
			{
				c[i * n + j] += a[i * n + k] * b[k * n + j];
			}
		}
	}
}

void fillMatrix(double *matrix, int n) 
{
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++) 
		{
			matrix[i * n + j] = rand() % 100;
		}
	}
}


/*в начале каждой итерации iter алгоритма для каждой строки процессной решетки выбирается процесс, который будет рассылать свой блок матрицы А*/
void ABlockCommunication(int iter, double *pAblock, double *pMatrixAblock, int BlockSize) 
{
	// определяем ведущий процессор
	int Pivot = (gridCoords[0] + iter) % gridSize;
	// копируем передаваемый блок в буфер
	if (gridCoords[1] == Pivot) 
	{
		for (int i = 0; i < BlockSize * BlockSize; i++)
			pAblock[i] = pMatrixAblock[i];
	}
	// раскидываем блок
	MPI_Bcast(pAblock, BlockSize * BlockSize, MPI_DOUBLE, Pivot, rowComm);
}

void shiftB(double *b, int blockSize) 
{
	int next = gridCoords[0] == gridSize - 1 ? 0 : gridCoords[0] + 1;
	int prev = gridCoords[0] == 0 ? gridSize - 1 : gridCoords[0] - 1;

	MPI_Status Status;
	MPI_Sendrecv_replace(b, blockSize * blockSize, MPI_DOUBLE, next, 0, prev, 0, colComm, &Status);
}


void calculate(double *a, double *tempA, double *b, double *c, int blockSize) 
{
	for (int i = 0; i < gridSize; i++)
	{
		ABlockCommunication(i, a, tempA, blockSize);
		multiplyMatrix(a, b, c, blockSize);
		shiftB(b, blockSize);
	}
}



// функция инициализации памяти и данных
void ProcessInitialization(double *&pAMatrix, double *&pBMatrix, double *&pCMatrix, double *&pAblock, double *&pBblock,
	double *&pCblock, double *&pTemporaryAblock, int &Size, int &BlockSize)
{
	MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	BlockSize = Size / gridSize;
	pAblock = new double[BlockSize * BlockSize];
	pBblock = new double[BlockSize * BlockSize];
	pCblock = new double[BlockSize * BlockSize];
	pTemporaryAblock = new double[BlockSize * BlockSize];
	for (int i = 0; i < BlockSize * BlockSize; i++) 
	{
		pCblock[i] = 0;
	}
	if (procRank == 0) {
		pAMatrix = new double[Size * Size];
		pBMatrix = new double[Size * Size];
		pCMatrix = new double[Size * Size];
		fillMatrix(pAMatrix, Size);
		fillMatrix(pBMatrix, Size);
	}
}


/*функция создает коммуникатор в виде двумерной квадратной решетки, определяет координаты каждого процесса в этой решетке*/
void CreateGridCommunicators() 
{
	int DimSize[2]; // число процессоров в каждом измерении сетки
	int Periodic[2]; // =1, если размерность должна быть динамична
	int Subdims[2]; // =1, если размерность должна быть фиксированна
	DimSize[0] = gridSize;
	DimSize[1] = gridSize;
	Periodic[0] = 0;
	Periodic[1] = 0;
	// создание коммуникатора
	/*2 - число измерений
	DimSize - -	 массив, в котором задается число процессов вдоль каждого измерения
	Periodic - логический массив размера ndims для задания граничных условий
	1 - логическая переменная, указывает, производить перенумерацию процессов (true) или нет (false)
	GridComm - новый коммуникатор*/
	MPI_Cart_create(MPI_COMM_WORLD, 2, DimSize, Periodic, 1, &gridComm);
	//определение координат каждого процессора в решетке
	MPI_Cart_coords(gridComm, procRank, 2, gridCoords);
	// создание коммуникатора для строк 
	Subdims[0] = 0; // фиксация размерности
	Subdims[1] = 1; // наличие данной размерности в подсетке
	MPI_Cart_sub(gridComm, Subdims, &rowComm);
	// создание коммуникатора для столбцов
	Subdims[0] = 1;
	Subdims[1] = 0;
	// создание множества коммуникаторов для каждой строки и каждого столбца решетки в отдельности
	MPI_Cart_sub(gridComm, Subdims, &colComm);
}


void main(int argc, char *argv[]) 
{
	double *pAMatrix; // первая матрица
	double *pBMatrix; // вторая матрица
	double *pCMatrix; // результат
	int Size = 4; // размер матрицы
	int BlockSize; // размер матричного блока на процессоре
	double *pAblock; // начальный блок матрицы А
	double *pBblock; // начальный блок матрицы В
	double *pCblock; // результирующий блок
	double *pMatrixAblock;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
	gridSize = sqrt((double)procNum);


	if (procNum != gridSize * gridSize) 
	{
		if (procRank == 0) 
		{
			printf("Number of processes must be a perfect square \n");
		}
	}
	else 
	{
		if (procRank == 0)
			printf("Parallel matrix multiplication program\n");
		// создание процессорной сетки
		CreateGridCommunicators();
		// выделение памяти и инициализация матриц
		ProcessInitialization(pAMatrix, pBMatrix, pCMatrix, pAblock, pBblock, pCblock, pMatrixAblock, Size, BlockSize);

		if (procRank == 0) 
		{
			for (int i = 0; i < Size; i++) 
			{
				for (int j = 0; j < Size; j++)
					printf("\nAMatrix = %10.2f", pAMatrix[i * Size + j]);
				printf("\n");
			}
		}

		if (procRank == 0) 
		{
			for (int i = 0; i < Size; i++) 
			{
				for (int j = 0; j < Size; j++)
					printf("\nBMatrix = %10.2f", pBMatrix[i * Size + j]);
				printf("\n");
			}
		}


		distributeData(pAMatrix, pBMatrix, pMatrixAblock, pBblock, Size, BlockSize);

		calculate(pAblock, pMatrixAblock, pBblock, pCblock, BlockSize);

		collectData(pCMatrix, pCblock, Size, BlockSize);

		if (procRank == 0) 
		{
			for (int i = 0; i < Size; i++) 
			{
				for (int j = 0; j < Size; j++)
					printf("\nCMatrix = %10.2f", pCMatrix[i * Size + j]);
				printf("\n");
			}
		}
		MPI_Finalize();
	}
}
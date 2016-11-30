#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>


using namespace std;

void printBuf(vector< int> & buf, int n, int m);
void CreateTestFile();
void Mult(vector<int> &a, vector<int> &  b, vector<int> &c, int size_i, int size_j);


int main()
{

	CreateTestFile();

	const auto startTime = std::clock();
	int n;
	int m;

	ifstream infileA("input.bin", ios::in | ios::binary);
	ifstream infileB("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out | ios::binary);

	infileA.read(reinterpret_cast<char *>(&n), 4);
	infileA.read(reinterpret_cast<char *>(&m), 4);

	int v = 1;
	outfile.write(reinterpret_cast<char *>(&n), 4);
	outfile.write(reinterpret_cast<char *>(&v), 4);

	int size_1 = 1;
	int block_size = 2;

	vector<int> bufferA;
	vector<int> bufferB;
	vector<int> bufferC;

	int repeat = n % block_size == 0 ? n / block_size : n / block_size + 1;

	infileB.seekg(n*n + 16);

	for (int i = 0; i < repeat; i++)
	{
		int size_i = n < (i + 1) * block_size ? n - i * block_size : block_size;

		bufferC.resize(size_i * size_1, 0);
		

		

		for (int j = 0; j < repeat; j++)
		{
			int size_j = n < (j + 1) * block_size ? n - j * block_size : block_size;
			bufferA.resize(size_i * size_j, 0);
			for (int ib = 0; ib < size_i; ib++)
			{
				vector<char> line(size_j, 0);

				infileA.read(&line[0], size_j);

				for (int kb = 0; kb < size_j; ++kb)
				{
					bufferA[ib * size_j + kb] = (int)line[kb];
				}

				if (ib != size_i - 1) infileA.seekg(n - size_j, ios::cur);				
			}
			if (-n*(size_i - 1) != 0)
				infileA.seekg(-n*(size_i - 1), ios::cur);
			cout << "buff A:" << endl;
			printBuf(bufferA, size_i, size_j);

			bufferB.resize(size_j * size_1, 0);
			for (int kb = 0; kb < size_j; kb++)
			{
				char line;
				infileB.read(&line, size_1);
				bufferB[kb] = (int)line;
			}
			cout << "buf B:" << endl;
			printBuf(bufferB, size_j, size_1);

			Mult(bufferA, bufferB, bufferC, size_i, size_j);

			vector<int>().swap(bufferA);
			vector<int>().swap(bufferB);

		}
		// write

		for (int ib = 0; ib < size_i; ib++)
		{
			char line;
			line = (char)(bufferC[ib] % 256);
			outfile.write(&line, size_1);
		}

		vector<int>().swap(bufferC);

		//seek A to next row
		//seel B to start
		infileA.seekg(-n*(size_i - 1), ios::cur);
		infileB.seekg(-n*(size_i - 1), ios::cur);
	}


	vector<int>().swap(bufferA);
	vector<int>().swap(bufferB);
	vector<int>().swap(bufferC);

	infileB.close();
	infileA.close();
	outfile.close();



	cout << endl;
	ifstream infile1("output.bin", ios::in | ios::binary);

	int z;
	int x;
	infile1.read(reinterpret_cast<char *>(&z), 4);
	infile1.read(reinterpret_cast<char *>(&x), 4);
	vector<char> buffer1(z * x, 0);

	infile1.read(&buffer1[0], buffer1.size());
	for (int i = 0; i < z; ++i)
	{
		for (int j = 0; j < x; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)buffer1[i * x + j] << " ";
		}
		cout << endl;
	}

	const auto endTime = std::clock();
	std::cout << endl << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	getchar();

	return 0;
}

void Mult(vector<int> &a, vector<int> &  b, vector<int> &c, int size_i, int size_j)
{
	for (int ib = 0; ib < size_i; ++ib)
	{
		for (int jb = 0; jb < size_j; ++jb)
		{
			c[ib] += a[ib * size_j + jb] * b[jb];
		}
	}
}

void CreateTestFile()
{
	const int n = 6;
	const int m = 6;
	const int v = 1;
	//srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrixA(n * m);
	vector<unsigned char> matrixB(n * v);
	int c = 1;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrixA[i * m + j] = j;
			c++;
		}
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < v; ++j)
		{
			matrixB[i * v + j] = i;
			c++;
		}
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)matrixA[i * m + j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < v; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)matrixB[i * v + j] << " ";
		}
		cout << endl;
	}

	ofstream out("input.bin", ios::out | ios::binary);

	out.write((char *)&n, sizeof(int));
	out.write((char *)&m, sizeof(int));
	copy(matrixA.begin(), matrixA.end(), std::ostreambuf_iterator<char>(out));

	out.write((char *)&n, sizeof(int));
	out.write((char *)&m, sizeof(int));
	copy(matrixB.begin(), matrixB.end(), std::ostreambuf_iterator<char>(out));

	out.close();
}

void printBuf(vector< int> & buf, int n, int m)
{
	cout << endl;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)buf[i * m + j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
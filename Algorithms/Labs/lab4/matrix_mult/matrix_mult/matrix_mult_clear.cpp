//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;

void Mult(vector<int> &a, vector<int> &  b, vector<int> &c, int size_i, int size_j, int size_k);


int main()
{
	int n;
	int m;

	ifstream infileA("input.bin", ios::in | ios::binary);
	ifstream infileB("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out | ios::binary);

	infileA.read(reinterpret_cast<char *>(&n), 4);
	infileA.read(reinterpret_cast<char *>(&m), 4);

	outfile.write(reinterpret_cast<char *>(&n), 4);
	outfile.write(reinterpret_cast<char *>(&m), 4);


	int block_size = 128;

	vector<int> bufferA;
	vector<int> bufferB;
	vector<int> bufferC;

	int repeat = n % block_size == 0 ? n / block_size : n / block_size + 1;

	infileB.seekg(n*n + 16);

	for (int i = 0; i < repeat; i++)
	{
		int size_i = n < (i + 1) * block_size ? n - i * block_size : block_size;

		for (int j = 0; j < repeat; j++)
		{
			int size_j = n < (j + 1) * block_size ? n - j * block_size : block_size;
			bufferC.resize(size_i * size_j, 0);

			for (int k = 0; k < repeat; k++)
			{
				int size_k = n < (k + 1) * block_size ? n - k * block_size : block_size;
				bufferA.resize(size_i * size_k, 0);

				for (int ib = 0; ib < size_i; ib++)
				{
					vector<char> line(size_k,0);

					infileA.read(&line[0], size_k);

					for (int kb = 0; kb < size_k; ++kb)
					{
						bufferA[ib * size_k + kb] = (int)line[kb];
					}

					if (ib!=size_i - 1) infileA.seekg(n - size_k, ios::cur);
				}
				if (-n*(size_i - 1) !=0)
					infileA.seekg(-n*(size_i - 1), ios::cur);


				bufferB.resize(size_k*size_j, 0);

				for (int kb = 0; kb < size_k; kb++)
				{
					vector<char> line(size_j, 0);

					infileB.read(&line[0], size_j);

					for (int jb = 0; jb < size_j; ++jb)
					{
						bufferB[kb * size_j + jb] = (int)line[jb];
					}

					infileB.seekg(n - size_j, ios::cur);
				}
				Mult(bufferA, bufferB, bufferC, size_i, size_j, size_k);

			}
			bufferA.clear();
			bufferB.clear();

			// write

			for (int ib = 0; ib < size_i; ib++)
			{
				vector<char> line(size_j,0);
				for (int jb = 0; jb < size_j; ++jb)
				{
					line[jb] = (char)(bufferC[ib * size_j + jb] % 256);
				}

				outfile.write(&line[0], size_j);
				if (ib != size_i - 1) outfile.seekp(n - size_j, ios::cur);
			}
			outfile.seekp(-n * (size_i - 1), ios::cur);

			bufferC.clear();
			// seek a start
			// b seek next column

			infileA.seekg(-n, ios::cur);
			infileB.seekg((-n * n) + size_j , ios::cur);
		}

		//seek A to next row
		infileA.seekg(n*size_i, ios::cur);
		//seek B to start
		infileB.seekg(-n, ios::cur);
		//seek out to next row
		outfile.seekp(n*(size_i-1), ios::cur);

	}


	infileB.close();
	infileA.close();
	outfile.close();
	return 0;
}

void Mult(vector<int> &a, vector<int> &  b,vector<int> &c, int size_i,int size_j,int size_k)
{
	for (int ib = 0; ib < size_i; ++ib)
	{
		for (int jb = 0; jb < size_j; ++jb)
		{
			for (int kb = 0; kb < size_k; ++kb)
			{
				c[ib * size_j + jb] += a[ib * size_k + kb] * b[kb * size_j + jb];
			}
		}
	}
}

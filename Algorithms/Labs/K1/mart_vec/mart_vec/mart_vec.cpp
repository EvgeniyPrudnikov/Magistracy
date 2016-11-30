//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void Mult(vector<int> &a, vector<int> &  b, vector<int> &c, int size_i, int size_j);

int main()
{

	int n;
	int m;

	ifstream infileA("input.bin", ios::in | ios::binary);
	ifstream infileB("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out | ios::binary);

	infileA.read(reinterpret_cast<char *>(&n), 4);
	infileA.read(reinterpret_cast<char *>(&m), 4);


	int size_1 = 1;
	int block_size = 192;

	vector<int> bufferA;
	vector<int> bufferB;
	vector<int> bufferC;

	int repeat1 = n % block_size == 0 ? n / block_size : n / block_size + 1;
	int repeat2 = m % block_size == 0 ? m / block_size : m / block_size + 1;

	infileB.seekg(n*m + 8);

	for (int i = 0; i < repeat1; i++)
	{
		int size_i = n < (i + 1) * block_size ? n - i * block_size : block_size;

		bufferC.resize(size_i * size_1, 0);

		for (int j = 0; j < repeat2; j++)
		{
			int size_j = m < (j + 1) * block_size ? m - j * block_size : block_size;

			bufferA.resize(size_i * size_j, 0);

			for (int ib = 0; ib < size_i; ib++)
			{
				vector<char> line(size_j, 0);

				infileA.read(&line[0], size_j);

				for (int kb = 0; kb < size_j; ++kb)
				{
					bufferA[ib * size_j + kb] = (int)line[kb];
				}

				if (ib != size_i - 1) infileA.seekg(m - size_j, ios::cur);
			}
			if (-m*(size_i - 1) != 0)
				infileA.seekg(-m*(size_i - 1), ios::cur);


			bufferB.resize(size_j * size_1, 0);
			for (int kb = 0; kb < size_j; kb++)
			{
				char line = 0;
				infileB.read(&line, size_1);
				bufferB[kb] = (int)line;
			}

			Mult(bufferA, bufferB, bufferC, size_i, size_j);

			vector<int>().swap(bufferA);
			vector<int>().swap(bufferB);

		}
		// write

		for (int ib = 0; ib < size_i; ib++)
		{
			//char line;
			char line = bufferC[ib] % 256;
			outfile.write(&line, size_1);
		}

		vector<int>().swap(bufferC);

		//seek A to next row
		//seel B to start
		infileA.seekg(m*(size_i - 1), ios::cur);
		infileB.seekg(-m, ios::cur);

	}

	vector<int>().swap(bufferA);
	vector<int>().swap(bufferB);
	vector<int>().swap(bufferC);

	infileB.close();
	infileA.close();
	outfile.close();

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

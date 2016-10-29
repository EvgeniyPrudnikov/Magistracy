// matrix_mult.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

void printBuf(vector< int> & buf, int n, int m);
void CreateTestFile();
void Mult(vector<int> &a, vector<int> &  b, vector<int> &c, int size_i, int size_j, int size_k);


int main()
{
	const auto startTime = std::clock();

	int n;
	int m;

	CreateTestFile();

	ifstream infileA("input.bin", ios::in | ios::binary);
	ifstream infileB("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out | ios::binary);

	infileA.read(reinterpret_cast<char *>(&n), 4);
	infileA.read(reinterpret_cast<char *>(&m), 4);

	outfile.write(reinterpret_cast<char *>(&n), 4);
	outfile.write(reinterpret_cast<char *>(&m), 4);


	int block_size = 2;

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
				//cout << "b";
				//printBuf(bufferB, size_k, size_j);
				//infileB.seekg(n - size_j, ios::cur);

				Mult(bufferA, bufferB, bufferC, size_i, size_j, size_k);
				//cout << (int)infileB.tellg() - 16 << endl;

			}
			bufferA.clear();
			bufferB.clear();

			// write
			cout << (int)outfile.tellp() - 8 << endl;
			for (int ib = 0; ib < size_i; ib++)
			{
				vector<char> line(size_j,0);
				for (int jb = 0; jb < size_j; ++jb)
				{
					line[jb] = (char)(bufferC[ib * size_j + jb] /*% 256*/);
				}

				outfile.write(&line[0], size_j);
				if (ib != size_i - 1) outfile.seekp(n - size_j, ios::cur);
			}
			cout << (int)outfile.tellp() - 8 << endl;
			outfile.seekp(-n * (size_i - 1), ios::cur);
			cout << (int)outfile.tellp() - 8 << endl;

			bufferC.clear();
			// seek a start
			// b seek next column
			//cout << "seek a to start b to next col" << endl;
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

void CreateTestFile()
{
	const int n = 4;
	const int m = 4;
	//srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrixA(n * m);
	vector<unsigned char> matrixB(n * m);
	int c = 1;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrixA[i * m + j] = i;
			c++;
		}
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrixB[i * m + j] =j;
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
		for (int j = 0; j < m; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)matrixB[i * m + j] << " ";
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

void printBuf(vector< int> & buf,int n,int m)
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
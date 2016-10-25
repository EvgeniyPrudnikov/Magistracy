// matrix_mult.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

void printBuf(vector< char> & buf, int p);
void CreateTestFile();
void MultSwappedLoops(vector<char> &a, vector<char> &  b, vector<char> & c, int n);


int main()
{
	const auto startTime = std::clock();

	int n;
	int m;

	CreateTestFile();

	ifstream infile("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out | ios::binary);

	infile.read(reinterpret_cast<char *>(&n), 4);
	infile.read(reinterpret_cast<char *>(&m), 4);

	outfile.write(reinterpret_cast<char *>(&n), 4);
	outfile.write(reinterpret_cast<char *>(&m), 4);


	int p = 2;
	int q = 2;

	vector<char> bufferA(p * q, 0);
	vector<char> bufferB(p * q, 0);
	vector<char> bufferC(p * q, 0);

	int u = n % p == 0 ? n / p : n / p + 1;
	int v = m % p == 0 ? m / p : m / p + 1;

	for (int k = 0; k < u; ++k)
	{
		for (int l = 0; l < v; ++l)
		{
			for (int i = 0; i < p; ++i)
			{
				infile.read((char *)&bufferA[i * p], p);
				if (i != p - 1) infile.seekg(m - p, ios::cur);
			}
			infile.seekg(-m * (p - 1), ios::cur);
			printBuf(bufferA, p);



			infile.seekg(m*n -p + 8, ios::cur);
			for (int i = 0; i < p; ++i)
			{
				infile.read((char *)&bufferB[i * p], p);
				if (i != p - 1) infile.seekg(m - p, ios::cur);
			}
			infile.seekg(-m * (p - 1), ios::cur);
			infile.seekg((-m*n) - 8, ios::cur);



			printBuf(bufferB, p);

			MultSwappedLoops(bufferA, bufferB, bufferC, p);

			printBuf(bufferC, p);
			for (int i = 0; i < p; ++i)
			{
				outfile.write((char *)&bufferC[i * p], p);
				outfile.seekp(n - p, ios::cur);
			}
			//outfile.seekp(-m * (p - 1), ios::cur);
		}
		if (m % p == 0) infile.seekg(m, ios::cur);
		outfile.seekp((-n * m) + p, ios::cur);
	}
	
	infile.close();
	outfile.close();

	cout << endl;
	ifstream infile1("output.bin", ios::in | ios::binary);

	int z;
	int x;
	infile1.read(reinterpret_cast<char *>(&z), 4);
	infile1.read(reinterpret_cast<char *>(&x), 4);
	vector<unsigned char> buffer1(z * x, 0);

	infile1.read((char *)&buffer1[0], buffer1.size());
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

void MultSwappedLoops(vector<char> &a, vector<char> &  b, vector<char> & c, int n)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			c[i * n + j] = 0;
		}

		for (int k = 0; k < n; ++k)
		{
			for (int j = 0; j < n; ++j)
			{
				c[i * n + j] += a[i * n + k] * b[k * n + j];
			}
		}
	}
}



void CreateTestFile()
{
	const int n = 4;
	const int m = 4;
	//srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrix(n * m);
	int c = 1;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i * m + j] = j;
			c++;
		}
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)matrix[i * m + j] << " ";
		}
		cout << endl;
	}

	ofstream out("input.bin", ios::out | ios::binary);

	out.write((char *)&n, sizeof(int));
	out.write((char *)&m, sizeof(int));
	copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

	out.write((char *)&n, sizeof(int));
	out.write((char *)&m, sizeof(int));
	copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

	out.close();
}

void printBuf(vector< char> & buf, int p)
{
	cout << endl;

	for (int i = 0; i < buf.size() / p; i++)
	{
		for (int j = 0; j < buf.size() / p; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int)buf[i * p + j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
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
void AddMartr(vector<char> &c, vector<char> &  b, int n);
vector<char> MultSwappedLoops(vector<char> &a, vector<char> &  b, int n);


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


	int p = 2;
	int q = 2;

	vector<char> bufferA(p * q, 0);
	vector<char> bufferB(p * q, 0);
	vector<char> bufferC(p * q, 0);

	int u = n % p == 0 ? n / p : n / p + 1;
	int v = m % p == 0 ? m / p : m / p + 1;

	infileB.seekg(n*m + 16);

	for (int i = 0; i < u; i++)
	{
		for (int j = 0; j < u; j++)
		{
			for (int k = 0; k < u; k++)
			{

				for (int b = 0; b < p; b++)
				{
					infileA.read(&bufferA[b*p], p);

					if (b != p - 1) infileA.seekg(m - p, ios::cur);
				}
				cout << "a";
				printBuf(bufferA, p);
				infileA.seekg(-m*(p - 1), ios::cur);


				for (int b = 0; b < p; b++)
				{
					infileB.read(&bufferB[b*p], p);

					if (b != p - 1) infileB.seekg(m - p, ios::cur);
				}
				cout << "b";
				printBuf(bufferB, p);
				infileB.seekg(m - p, ios::cur);

				AddMartr(bufferC, MultSwappedLoops(bufferA, bufferB, p), p);
				//printBuf(bufferC, p);
			}

			// write
			for (int b = 0; b < p; b++)
			{
				outfile.write(&bufferC[b*p], p);
				if (b != p - 1) outfile.seekp(m - p, ios::cur);
			}
			outfile.seekp(-m * (p - 1), ios::cur);


			fill(bufferC.begin(), bufferC.end(), 0);
			// seek a start
			// b seek next column
			cout << "seek a to start b to next col";
			infileA.seekg(-m*(p-1), ios::cur);
			infileB.seekg((-n * m) + p, ios::cur);
		}

		cout << "seek a to next row";
		//seek A to next row
		infileA.seekg(m*p, ios::cur);
		infileB.seekg((-n * m), ios::cur);
		//outfile.seekp()

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

vector<char> MultSwappedLoops(vector<char> &a, vector<char> &  b, int n)
{
	vector<char> c(n*n, 0);

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
	return c;
}

void AddMartr(vector<char> &c, vector<char> &  b, int n)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			c[i*n + j] += b[i*n + j];
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
			matrixA[i * m + j] = c;
			c++;
		}
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrixB[i * m + j] =c;
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
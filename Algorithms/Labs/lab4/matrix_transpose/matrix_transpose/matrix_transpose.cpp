// matrix_transpose.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

void CreateTestFile();
void printBuf(vector<unsigned  char> & buf);
void Transpose( vector<unsigned char > & buf, int row, int col );


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

	int p = 3;
	int q = 3;

	vector<unsigned char> buffer(p * q, 0);

    for (int k = 0; k < n/p; ++k)
    {
        for (int u = 0; u < m / p; ++u)
        {
            for (int i = 0; i < p; ++i)
            {

                infile.read((char *) &buffer[i * p], p);
                infile.seekg(m - p, ios::cur);
            }
            infile.seekg(-m * p + p, ios::cur);

            Transpose(buffer, p, q);

            for (int i = 0; i < p; ++i)
            {

                outfile.write((char *) &buffer[i * p], p);
                outfile.seekp(n - p, ios::cur);
            }
        }
        infile.seekg(m * (p - 1), ios::cur);
        outfile.seekp((-n * m) + p, ios::cur);
    }

	infile.close();
	outfile.close();

    cout<<endl;

	ifstream infile1("output.bin", ios::in | ios::binary);
	vector<unsigned char> buffer1(135, 0);
	infile1.read((char *)&buffer1[0],buffer1.size());


	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			cout<<(int)buffer1[i*n+j]<<" ";
		}
		cout<<endl;
	}

    const auto endTime = std::clock();
    //std::cerr << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	//getchar();
	return 0;
}

void Transpose( vector<unsigned char > & buf, int row, int col )
{
	for ( int i = 0; i < row; i++ )
	{
		for ( int j = i + 1; j < col; j++ )
		{
			swap(buf[ i * col + j ],buf[ j * col + i ]);
		}
	}
}

void printBuf(vector<unsigned  char> & buf)
{
	cout<<endl;

	for (int i = 0; i < buf.size()/3; i++)
	{
		for (int j = 0; j < buf.size()/3; ++j)
		{
			cout << (int) buf[i * 3 + j] << " ";
		}
		cout << endl;
	}

	cout<<endl;
}

void CreateTestFile()
{
	const int n = 15;
	const int m = 9;
	//srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrix(n*m);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i*m + j] = rand() % 256;
		}
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << (int) matrix[i*m + j] << " ";
		}
		cout << endl;
	}

	ofstream out("input.bin", ios::out | ios::binary);

	out.write((char *)&n, sizeof(int));
	out.write((char *)&m, sizeof(int));
	copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

	out.close();
}
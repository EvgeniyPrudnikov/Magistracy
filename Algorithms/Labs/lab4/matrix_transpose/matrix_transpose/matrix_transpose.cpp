// matrix_transpose.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

void CreateTestFile();
void printBuf(vector< char> & buf,int );
void Transpose( vector<char > & buf, int row, int col );


int main()
{

    const auto startTime = std::clock();

    int n;
    int m;

    //CreateTestFile();

    ifstream infile("input.bin", ios::in | ios::binary);
    ofstream outfile("output.bin", ios::out | ios::binary);

    infile.read(reinterpret_cast<char *>(&n), 4);
    infile.read(reinterpret_cast<char *>(&m), 4);

    int p = 512;
    int q = 512;

    vector<char> buffer(p * q, 0);

    int u = n % p == 0 ? n / p : n / p + 1;
    int v = m % p == 0 ? m / p : m / p + 1;

    for (int k = 0; k < u; ++k)
    {
        for (int l = 0; l < v; ++l)
        {

            if ((m - l * p) < p)
            {

                for (int i = 0; i < p; ++i)
                {

                    infile.read(&buffer[i * p], m - l * p);
                    if (i != p - 1) infile.seekg(m - (m - l * p), ios::cur);
                }

                Transpose(buffer, p, q);

                for (int i = 0; i < m - l * p; ++i)
                {
                    outfile.write(&buffer[i * p], p);
                    if (i != p - 1) outfile.seekp(n - p, ios::cur);
                }
            } else if ((n - k * p) < p)
            {

                for (int i = 0; i < n - k * p; ++i)
                {
                    infile.read(&buffer[i * p], p);
                }
                infile.seekg(-m * (p - 1) + m, ios::cur);

                Transpose(buffer, p, q);

                for (int i = 0; i < p; ++i)
                {
                    outfile.write(&buffer[i * p], n - k * p);
                    outfile.seekp(n - (n - k * p), ios::cur);
                }
            } else
            {
                for (int i = 0; i < p; ++i)
                {

                    infile.read(&buffer[i * p], p);
                    if (i != p - 1) infile.seekg(m - p, ios::cur);
                }
                infile.seekg(-m * (p - 1), ios::cur);

                Transpose(buffer, p, q);

                for (int i = 0; i < p; ++i)
                {
                    outfile.write(&buffer[i * p], p);
                    outfile.seekp(n - p, ios::cur);
                }
            }
        }
        if (m % p == 0) infile.seekg(m, ios::cur);
        outfile.seekp((-n * m) + p, ios::cur);
    }

    infile.close();
    outfile.close();
/*
	cout << endl;

	ifstream infile1("output.bin", ios::in | ios::binary);
	vector<unsigned char> buffer1(m * n, 0);
	infile1.read((char *) &buffer1[0], buffer1.size());


	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int) buffer1[i * n + j] << " ";
		}
		cout << endl;
	}
*/
    const auto endTime = std::clock();
    std::cout << endl << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    //getchar();
    return 0;
}

void Transpose( vector<char > & buf, int row, int col )
{
	for ( int i = 0; i < row; i++ )
	{
		for ( int j = i + 1; j < col; j++ )
		{
			swap(buf[ i * col + j ],buf[ j * col + i ]);
		}
	}
}

void printBuf(vector< char> & buf, int p)
{
	cout<<endl;

	for (int i = 0; i < buf.size()/p; i++)
	{
		for (int j = 0; j < buf.size()/p; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int) buf[i * p + j] << " ";
		}
		cout << endl;
	}

	cout<<endl;
}

void CreateTestFile()
{
	const int n = 4097;
	const int m = 3200;
	srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrix(n * m);
	int c = 1;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i * m + j] = rand() % 256;
			c++;
		}
	}
/*
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int) matrix[i * m + j] << " ";
		}
		cout << endl;
	}
*/
	ofstream out("input.bin", ios::out | ios::binary);

	out.write((char *) &n, sizeof(int));
	out.write((char *) &m, sizeof(int));
	copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

	out.close();
}
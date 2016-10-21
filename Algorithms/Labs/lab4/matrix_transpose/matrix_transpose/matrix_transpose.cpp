// matrix_transpose.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

void CreateTestFile();


int main()
{

	int n;
	int m;

	//CreateTestFile();
	
	ifstream infile("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out | ios::binary);

	infile.read(reinterpret_cast<char*>(&n), 4);
	infile.read(reinterpret_cast<char*>(&m), 4);

	cout <<endl<< n <<endl;
	cout << m << endl;

	//vector<unsigned char> matr (n*m,0);

	vector<unsigned char> buffer(300 * 300, 0);
	while (!infile.eof())
	{
		
	}



	infile.close();
	outfile.close();

	cout << endl<< "done." << endl;
	getchar();
	return 0;
}


void printBuf(vector<unsigned  char> & buf)
{
	for (size_t i = 0; i < buf.size(); i++)
	{
		cout << (int) buf[i] << " ";
	}
	cout << endl;
}

void CreateTestFile()
{
	const int n = 1024;
	const int m = 1024;
	srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrix(n*m);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i*m + j] = rand() % 256;
		}
	}
/*
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << (int) matrix[i*m + j] << " ";
		}
		cout << endl;
	}
	*/
	ofstream out("input.bin", ios::out | ios::binary);

	out.write((char *)&n, sizeof(int));
	out.write((char *)&m, sizeof(int));
	copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

	out.close();
}
// matrix_transpose.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>


using namespace std;

int main()
{
	const int n = 5;
	const int m = 5;
	srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrix(n*m);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i*m + j] = rand() % 256;
		}
	}

	ofstream out("input.bin", ios::out | ios::binary);
	
	out.write((char *)&n, sizeof( int));
	out.write((char *)&m, sizeof(int));
	copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

	out.close();
	cout << endl<< "done." << endl;
	getchar();
	return 0;
}

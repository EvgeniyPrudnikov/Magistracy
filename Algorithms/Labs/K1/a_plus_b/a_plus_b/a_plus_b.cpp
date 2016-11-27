// a_plus_b.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

void CreateTestFile();

int main()
{

	CreateTestFile();

	int a;
	int b;
	int c;

	ifstream infile("input.bin", ios::in | ios::binary);
	ofstream outfile("output.bin", ios::out| ios::binary);

	infile.read((char *)&a, sizeof(int));
	infile.read((char *)&b, sizeof(int));

	c = a + b;

	outfile.write((char *)&c, sizeof(int));

	infile.close();
	outfile.close();

	int lol;

	ifstream infile2("output.bin", ios::in | ios::binary);
	infile2.read((char *)&lol, sizeof(int));

	cout << lol;

	getchar();
    return 0;
}



void CreateTestFile()
{

	int a = -1250000;
	int b = 1250000;


	int n = 13;
	srand(static_cast<unsigned int>(time(NULL)));

	vector<int> array{13, 18, 18, 4, 4, 23, 23, 8, 8, 6, 6, 2, 2, 9, 9, 12, 12, 22, 22, 5, 5, 1, 1, 7, 7, 13};

	/*
	for (int i = 0; i < n * 2 - 1; i += 2)
	{
		cout << setiosflags(ios::fixed | ios::left) << setprecision(2) << setw(1) << array[i] << " " << array[i + 1]
			<< endl;
	}
	*/
	ofstream out("input.bin", ios::out | ios::binary | ios::trunc);

	//out.write((char *)&n, sizeof(int));

	//out.write((char *) &array[0], n * 2 * sizeof(int));

	out.write((char*)&a, 4);
	out.write((char*)&b, 4);

	out.close();
	vector<int>().swap(array);
}



//#include "stdafx.h"
#include <iostream>
#include <fstream>


using namespace std;


int main()
{

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

    return 0;
}


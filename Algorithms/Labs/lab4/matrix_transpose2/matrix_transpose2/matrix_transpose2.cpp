#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

void CreateTestFile();
void printBuf(vector< char> & buf, int);
void Transpose(vector< char > & buf, int row, int col);
void Transpose2(vector< char > & bufA, vector< char > & bufB, int row, int col);


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

	outfile.write(reinterpret_cast<char *>(&m), 4);
	outfile.write(reinterpret_cast<char *>(&n), 4);



	int p = 5;
	int q = 5;

	int u = n / p;
	int v = m / p;

	if (u > 0 && v > 0)
	{
		vector<char> buffer(p * q, 0);

		for (int k = 0; k < u; ++k)
		{
			for (int l = 0; l < v; ++l)
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

			//if (k != u - 1) 
			//{
				infile.seekg(m*(p-1), ios::cur);
				outfile.seekp((-n * m) + p, ios::cur);
			//}
			
		}

		buffer.clear();
	}
	//if (n%p != 0)
	{
		
		//cout << endl << (int)infile.tellg() - 8 << endl;
		//cout << endl << (int) outfile.tellp() - 8 << endl;

		int bl_size_n = n - u*p;
		int bl_size_m = ceil(static_cast<float>(p*p) / bl_size_n);
		vector<char> bufferA(bl_size_n * bl_size_m);
		vector<char> bufferB(bl_size_m * bl_size_n);

		u = ceil(static_cast<float>(m) / bl_size_m);
		cout << endl << (int)outfile.tellp() - 8 << endl;
		//cout << endl << (int)infile.tellg() - 8 << endl;
		if (n / p > 0 && u!=0) 
		{
			infile.seekg(m* (n - bl_size_n) - m, ios::cur);
			outfile.seekp(-m*n + n - bl_size_n, ios::cur);
		}
		//cout << endl << (int)infile.tellg() - 8 << endl;
		for (int l = 0; l < u; ++l)
		{
			if (m < (l + 1) * bl_size_m)
			{
				bl_size_m = m - l * bl_size_m;
			}
			cout << endl << (int)infile.tellg() - 8 << endl;
			for (int i = 0; i < bl_size_n; ++i)
			{
				infile.read(&bufferA[i * bl_size_m], bl_size_m);
				if (i != bl_size_n - 1)
					infile.seekg(m - bl_size_m, ios::cur);
			}

			if ((-m * bl_size_n + m) != 0)
				infile.seekg(-m * bl_size_n + m, ios::cur);

			Transpose2(bufferA, bufferB, bl_size_n, bl_size_m);

			for (int i = 0; i < bl_size_m; ++i)
			{
				outfile.write(&bufferB[i * bl_size_n], bl_size_n);
				if (n - bl_size_n != 0) outfile.seekp(n - bl_size_n, ios::cur);
			}
		}

		bufferA.clear();
		bufferB.clear();
	}

	{

		//cout << endl << (int)infile.tellg() - 8 << endl;
		//cout << endl << (int)outfile.tellp() - 8 << endl;
		int n_new = n - (n - (n / p) * p);
		int bl_size_m = m - v*p;
		int bl_size_n = ceil(static_cast<float>(p*p) / bl_size_m);
		vector<char> bufferA(bl_size_n * bl_size_m);
		vector<char> bufferB(bl_size_m * bl_size_n);

		v = ceil(static_cast<float>(n_new) / bl_size_n);


		if (m / p > 0 && v!=0)
		{
			infile.seekg(8 + bl_size_m*bl_size_m, ios::cur);
			outfile.seekp(-m*n_new + n_new - bl_size_m, ios::cur);
		}


		//cout << endl << (int)infile.tellg() - 8 << endl;
		//cout << endl << (int)outfile.tellp() - 8 << endl;

		for (int k = 0; k < v; ++k)
		{
			if (n_new < (k + 1) * bl_size_n)
			{
				bl_size_n = n_new - k * bl_size_n;
			}
			//cout << endl << (int)infile.tellg() - 8 << endl;
			for (int i = 0; i < bl_size_n; ++i)
			{
				infile.read(&bufferA[i * bl_size_m], bl_size_m);
				if (i != bl_size_n -1)
					infile.seekg(m - bl_size_m, ios::cur);
			}

			//cout << endl << (int)infile.tellg() - 8 << endl;
			//if ((-n * bl_size_m + n) != 0)
			//	infile.seekg(-n * bl_size_m + n, ios::cur);
			//cout << endl << (int)infile.tellg() - 8 << endl;

			Transpose2(bufferA, bufferB, bl_size_n, bl_size_m);
			
			for (int i = 0; i < bl_size_m; ++i)
			{

				outfile.write(&bufferB[i * bl_size_n], bl_size_n);

				outfile.seekp(n_new - bl_size_n , ios::cur);

			}
			cout << endl << (int)outfile.tellp() - 8 << endl;
			outfile.seekp(-n_new*(bl_size_m) + n_new - bl_size_n, ios::cur);
			cout << endl << (int)outfile.tellp() - 8 << endl;
		}


		bufferA.clear();
		bufferB.clear();
	}

	infile.close();
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

void Transpose(vector< char > & buf, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = i + 1; j < col; j++)
		{
			swap(buf[i * col + j], buf[j * col + i]);
		}
	}
}

void Transpose2(vector< char > & bufA, vector< char > & bufB, int row, int col)
{
	for (int k = 0; k < row; ++k)
	{
		for (int l = 0; l < col; ++l)
		{
			bufB[l * row + k] = bufA[k * col + l];
		}
	}
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

void CreateTestFile()
{
	const int n = 9;
	const int m = 10;
	//srand(static_cast<unsigned int>(time(NULL)));


	vector<unsigned char> matrix(n * m);
	int c = 1;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i * m + j] = c;
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

	out.close();
}

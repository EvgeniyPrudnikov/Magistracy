#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <math.h>

using namespace std;

void CreateTestFile();
void printBuf(vector< char> & buf,int );
void Transpose( vector< char > & buf, int row, int col );
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

	int u = n % p == 0 ? n / p : n / p + 1;
	int v = m % p == 0 ? m / p : m / p + 1;

	if (n < p)
	{
		
		int size_f = p*p;
		int size_m = ceil((float)size_f/n);
		int size_n = n;
		vector<char> bufferA(size_n*size_m);
		vector<char> bufferB(size_m*size_n);


		for (int l = 0; l < v; ++l)
		{
			if (m < (l + 1) * size_m)
			{
				size_m = m - l * size_m;
			}

			for (int i = 0; i < size_n; ++i)
			{
				infile.read(&bufferA[i * size_m], size_m);
				if (i != size_n - 1)
					infile.seekg(m - size_m, ios::cur);
			}

			if ((-m * size_n + m) != 0)
				infile.seekg(-m * size_n + m, ios::cur);

			Transpose2(bufferA, bufferB, size_n, size_m);

			for (int i = 0; i < size_m; ++i)
			{
				outfile.write(&bufferB[i * size_n], size_n);
			
				//if ((m) != 0)
				//if (size_m != 0) outfile.seekp(n - size_n, ios::cur);
			}
		}


		bufferA.clear();
		bufferB.clear();
	} 
	else if (m<p)
	{
		
		int size_f = p*p;
		int size_m = m;
		int size_n = ceil((float)size_f / m);
		vector<char> bufferA(size_n*size_m);
		vector<char> bufferB(size_m*size_n);


		for (int l = 0; l < u; ++l)
		{
			if (n < (l + 1) * size_n)
			{
				size_n = n - l * size_n;
			}

			for (int i = 0; i < size_n; ++i)
			{
				infile.read(&bufferA[i * size_m], size_m);
				//if (i != n - 1)
				//	infile.seekg(n - size_n, ios::cur);
			}

			//if ((-m * size_n + m) != 0)
			//	infile.seekg(-m * size_n + m, ios::cur);

			Transpose2(bufferA, bufferB, size_n, size_m);

			for (int i = 0; i < size_m; ++i)
			{
				outfile.write(&bufferB[i * size_n], size_n);
				if (i != size_m - 1) outfile.seekp(n - size_n, ios::cur);

			}
			if ((-m * size_n + m) != 0)
				outfile.seekp(-n * size_m + n, ios::cur);
		}


		bufferA.clear();
		bufferB.clear();

	}
	else
	{
		vector<char> buffer(p * q, 0);

		for (int k = 0; k < u; ++k)
		{
			for (int l = 0; l < v; ++l)
			{
				if ((m - l * p) < p && (n - k * p) < p)
				{
					for (int i = 0; i < n - k * p; ++i)
					{
						infile.read(&buffer[i * p], m - l * p);
						if (i != p - 1) infile.seekg(m - (m - l * p), ios::cur);
					}

					Transpose(buffer, p, p);

					for (int i = 0; i < m - l * p; ++i)
					{
						outfile.write(&buffer[i * p], n - k * p);
						outfile.seekp(n - (n - k * p), ios::cur);
					}

				}
				else  if ((m - l * p) < p)
				{
					for (int i = 0; i < p; ++i)
					{
						infile.read(&buffer[i * p], m - l * p);
						if (i != p - 1) infile.seekg(m - (m - l * p), ios::cur);
					}

					Transpose(buffer, p, p);

					for (int i = 0; i < m - l * p; ++i)
					{
						outfile.write(&buffer[i * p], p);
						if (i != p - 1) outfile.seekp(n - p, ios::cur);
					}
				}
				else if ((n - k * p) < p)
				{
					//cout << (int)infile.tellg() - 8 << endl;
					for (int i = 0; i < n - k * p; ++i)
					{
						infile.read(&buffer[i * p], p);
						if (i != n - k * p - 1) infile.seekg(m - p, ios::cur);
					}
					
					if (-m * (n - k * p) + m !=0)
						infile.seekg(-m * (n - k * p) + m, ios::cur);

					Transpose(buffer, p, p);

					for (int i = 0; i < p; ++i)
					{
						outfile.write(&buffer[i * p], n - k * p);
						outfile.seekp(n - (n - k * p), ios::cur);
					}
				}
				else
				{
					for (int i = 0; i < p; ++i)
					{
						infile.read(&buffer[i * p], p);
						if (i != p - 1) infile.seekg(m - p, ios::cur);
					}
					infile.seekg(-m * (p - 1), ios::cur);

					Transpose(buffer, p, p);

					for (int i = 0; i < p; ++i)
					{
						outfile.write((&buffer[i * p]), p);
						outfile.seekp(n - p, ios::cur);
					}
				}
			}
			if (m % p == 0) infile.seekg(m * (p-1), ios::cur);
			outfile.seekp((-n * m) + p, ios::cur);
		}
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
    const int n = 10;
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
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int) matrix[i * m + j] << " ";
		}
		cout << endl;
	}
	
    ofstream out("input.bin", ios::out | ios::binary);

    out.write((char *) &n, sizeof(int));
    out.write((char *) &m, sizeof(int));
    copy(matrix.begin(), matrix.end(), std::ostreambuf_iterator<char>(out));

    out.close();
}

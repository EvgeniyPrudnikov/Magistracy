//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

void CreateTestFile();
void printBuf(vector< char> & buf,int );
void Transpose(vector<char> & buf, const unsigned h, const unsigned w);
void printBuf2(vector<char> & buf, int beg, int size);


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
    int q = 2;

    vector<char> buffer(p * q, 0);

    int u = n % q == 0 ? n / q : n / q + 1;
    int v = m % p == 0 ? m / p : m / p + 1;

	for (int k = 0; k < u; ++k)
	{
		for (int l = 0; l < v; ++l)
		{
			if ((m - l * p) < p && (n - k * q) < q)
			{
				for (int i = 0; i < n - k * q; ++i)
				{
					infile.read(&buffer[i * p], m - l * p);
					if (i != n - k * q - 1) infile.seekg(m - (m - l * p), ios::cur);
				}

				Transpose(buffer, q, p);

				for (int i = 0; i < m - l * p; ++i)
				{
					outfile.write(&buffer[i * q], n - k * q);
					outfile.seekp(n - (n - k * q), ios::cur);
				}

			}
			else  if ((m - l * p) < p)
			{
				for (int i = 0; i < q; ++i)
				{
					infile.read(&buffer[i * p], m - l * p);
					if (i != q - 1) infile.seekg(m - (m - l * p), ios::cur);
				}

				Transpose(buffer, q, p);

				for (int i = 0; i < m - l * p; ++i)
				{
					outfile.write(&buffer[i * q], q);
					if (i != m - l * p - 1) outfile.seekp(n - q, ios::cur);
				}
			}
			else if ((n - k * q) < q)
			{

				for (int i = 0; i < n - k * q; ++i)
				{
					infile.read(&buffer[i * p], p);
					if (i != n - k * q - 1) infile.seekg(m - p, ios::cur);
				}

				infile.seekg(-m * (n - k * q) + m, ios::cur);

				Transpose(buffer, q, p);

				for (int i = 0; i < p; ++i)
				{
					outfile.write(&buffer[i * q], n - k * q);
					outfile.seekp(n - (n - k * q), ios::cur);
				}
			}
			else
			{
				for (int i = 0; i < q; ++i)
				{
					infile.read(&buffer[i * p], p);
					if (i != q - 1) infile.seekg(m - p, ios::cur);
				}
				infile.seekg(-m * (q - 1), ios::cur);

                Transpose(buffer, q, p);

				for (int i = 0; i < p; ++i)
				{
					outfile.write(&buffer[i * q], q);
                    //printBuf2(buffer,i*q,q);
					outfile.seekp(n - q, ios::cur);
				}
			}
		}
		if (m % p == 0) infile.seekg(m, ios::cur);
		outfile.seekp((-n * m) + q, ios::cur);
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

	infile1.read((char *) &buffer1[0], buffer1.size());
	for (int i = 0; i < z; ++i)
	{
		for (int j = 0; j < x; ++j)
		{
			cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int) buffer1[i * x + j] << " ";
		}
		cout << endl;
	}

    const auto endTime = std::clock();
    std::cout << endl << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    getchar();
    return 0;
}

void Transpose(vector<char> & buf, const unsigned h, const unsigned w)
{
    for (unsigned start = 0; start <= w * h - 1; ++start)
    {
        unsigned next = start;
        unsigned i = 0;
        do
        {
            ++i;
            next = (next % h) * w + next / h;
        } while (next > start);

        if (next >= start && i != 1)
        {
            const char tmp = buf[start];
            next = start;
            do
            {
                i = (next % h) * w + next / h;
                buf[next] = (i == start) ? tmp : buf[i];
                next = i;
            } while (next > start);
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

void printBuf2(vector<char> & buf, int beg, int size)
{

    for (int i = beg; i <beg + size ; ++i)
    {
        cout << setiosflags(ios::fixed | ios::left) << setw(3) << (int) buf[i] << " ";
    }

    cout<<endl;
}

void CreateTestFile()
{
    const int n = 10;
    const int m = 3;
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
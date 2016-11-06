// external_sorting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>


using namespace std;

const int block_size_M = 6;
const int block_size_B = 2;

void CreateTestFile();

void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end);


int main(int argc, char *argv[])
{

    const auto startTime = std::clock();

    CreateTestFile();

	long N;

    ifstream infile("input.bin", fstream::in | ios::binary);
    ofstream tempfile("temp.bin",  fstream::out | ios::binary);
    ofstream outfile("output.bin", fstream::out | ios::binary);


    infile.read(reinterpret_cast<char *>(&N), 8);
    tempfile.write(reinterpret_cast<char *>(&N), 8);
    outfile.write(reinterpret_cast<char *>(&N), 8);

    long init_offset = 4;

    vector<long> bufferM(block_size_M);

    int runs = N / block_size_M;
    int read_size_M = block_size_M * sizeof(long);
    int read_size_B = block_size_B * sizeof(long);

    for (int j = 0; j < runs; ++j)
    {
        infile.read((char *) &bufferM[0], read_size_M);

        sort(bufferM.begin(), bufferM.end());

        tempfile.write((char *) &bufferM[0], read_size_M);
    }

    bufferM.clear();

    tempfile.close();

	cout << endl;
	ifstream infile1("temp.bin", ios::in | ios::binary);

	int z;
	infile1.read(reinterpret_cast<char *>(&z), 4);
	vector<long> buffer1(z, 0);
	infile1.read((char *)&buffer1[0], buffer1.size() * sizeof(long));
	for (int i = 0; i < z; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << buffer1[i] << " ";
	}
	infile1.close();
	outfile.flush();
	outfile.close();

	ofstream outfile1("output.bin", ios::out | ios::binary);
    ifstream tempfile1("temp.bin", ios::in | ios::binary);
	ifstream tempfile2("temp.bin", ios::in | ios::binary);

    mergeRuns(tempfile1, tempfile2, outfile1, init_offset, init_offset + read_size_M, init_offset + 2 * read_size_M);


	infile.close();
	tempfile1.close();
	tempfile2.close();
	outfile1.flush();
	outfile1.close();
	

	cout << endl;
	ifstream infile2("output.bin", ios::in | ios::binary);

	int z2 = 24;
	//infile1.read(reinterpret_cast<char *>(&z2), 4);
	vector<long> buffer2(z2, 0);
	infile1.read((char *)&buffer1[0], buffer1.size() * sizeof(long));
	for (int i = 0; i < z2; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << buffer1[i] << " ";
	}
	infile2.close();
   
	
    const auto endTime = std::clock();
    std::cout << endl << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    getchar();

    return 0;
}


void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end)
{

	//cout << (int)infile1.tellg() - 4 << endl;
	//cout << (int)infile2.tellg() - 4 << endl;

    int read_size_B = block_size_B * sizeof(long);

    long read_blk1_offset = run1_start;
    long read_blk2_offset = run2_start;

    vector<long> bufferBr1(block_size_B);
    vector<long> bufferBr2(block_size_B);

	infile1.seekg(run1_start, ios::cur);
	//cout << (int)infile1.tellg() - 4 << endl;
    infile1.read((char *) &bufferBr1[0], read_size_B);

    infile2.seekg(run2_start, ios::cur);

    infile2.read((char *) &bufferBr2[0], read_size_B);


    vector<long> bufferBw(block_size_B);

    int p1 = 0, p2 = 0, po = 0;

    while (read_blk1_offset < run2_start && read_blk2_offset < run2_end)
    {
        if (bufferBr1[p1] < bufferBr2[p2])
        {
            bufferBw[po] = bufferBr1[p1];
            p1++;

            if (p1 > block_size_B - 1)
            {
                read_blk1_offset += read_size_B;
                if (read_blk1_offset < run2_start)
                {
                    infile1.read((char *) &bufferBr1[0], read_size_B);
                    p1 = 0;
                }
            }
        } else
        {
            bufferBw[po] = bufferBr2[p2];
            p2++;
            if (p2 > block_size_B - 1)
            {
                read_blk2_offset += read_size_B;
                if (read_blk2_offset < run2_end)
                {
                    infile2.read((char *) &bufferBr2[0], read_size_B);
                    p2 = 0;
                }
            }
        }

        po++;
        if (po > block_size_B - 1)
        {
            outfile.write((char *) &bufferBw[0], read_size_B);
            po = 0;
        }
    }

    if (read_blk1_offset == run2_start)
    {
        outfile.write((char *) &bufferBr2[0], read_size_B);

        for (int i = read_blk2_offset + read_size_B; i < run2_end; i += read_size_B)
        {
            infile2.read((char *) &bufferBr2[0], read_size_B);
            outfile.write((char *) &bufferBr2[0], read_size_B);
        }
    } else if (read_blk2_offset == run2_end)
    {
        outfile.write((char *) &bufferBr1[0], read_size_B);

        for (int i = read_blk1_offset + read_size_B; i < run2_start; i += read_size_B)
        {
            infile1.read((char *) &bufferBr1[0], read_size_B);
            outfile.write((char *) &bufferBr1[0], read_size_B);
        }
    }
}


void CreateTestFile()
{
    const long n = 24;
    //srand(static_cast<unsigned int>(time(NULL)));


    vector<long> array(n);
    int c = n;
    for (int i = 0; i < n; ++i)
    {
        array[i] = rand() % n;
        c--;
    }

    for (int i = 0; i < n; ++i)
    {
        cout << setiosflags(ios::fixed | ios::left)<< array[i] << " ";
    }

    ofstream out("input.bin", ios::out | ios::binary);


    out.write((char *) &n, sizeof(long));

    out.write((char *) &array[0], n * sizeof(long));

    out.close();
}


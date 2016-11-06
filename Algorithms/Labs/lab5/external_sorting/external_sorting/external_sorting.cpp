// external_sorting.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <iterator>

using namespace std;

const int block_size_M = 6;
const int block_size_B = 2;

void CreateTestFile();

void mergeRuns(fstream &infile1, fstream &infile2, fstream &outfile, int run1_start, int run2_start, int run2_end);


int main(int argc, char *argv[])
{

    const auto startTime = std::clock();

    int N;

    CreateTestFile();

    fstream infile("input.bin", fstream::in | ios::binary);
    fstream tempfile("temp.bin", fstream::in | fstream::out | ios::binary);
    fstream outfile("output.bin", fstream::out | fstream::in | ios::binary);

    infile.read(reinterpret_cast<char *>(&N), 4);
    tempfile.write(reinterpret_cast<char *>(&N), 4);
    outfile.write(reinterpret_cast<char *>(&N), 4);

    int init_offset = 8;

    vector<int> bufferM(block_size_M);

    int runs = N / block_size_M;
    int read_size_M = block_size_M * sizeof(int);
    int read_size_B = block_size_B * sizeof(int);

    for (int j = 0; j < runs; ++j)
    {
        infile.read((char *) &bufferM[0], read_size_M);

        sort(bufferM.begin(), bufferM.end());

        tempfile.write((char *) &bufferM[0], read_size_M);
    }

    bufferM.clear();

    tempfile.close();
    fstream tempfile1("temp.bin", fstream::in | fstream::out | ios::binary);

    mergeRuns(tempfile1, tempfile1, outfile, init_offset, init_offset + read_size_M, init_offset + 2 * read_size_M);


    infile.close();
    outfile.close();
    tempfile.close();


    cout << endl;
    ifstream infile1("output.bin", ios::in | ios::binary);

    int z;
    infile1.read(reinterpret_cast<char *>(&z), 4);
    vector<int> buffer1(z, 0);
    infile1.read((char *) &buffer1[0], buffer1.size() * sizeof(int));
    for (int i = 0; i < z; ++i)
    {
        cout << setiosflags(ios::fixed | ios::left) << setw(3) << buffer1[i] << " ";
    }

    const auto endTime = std::clock();
    std::cout << endl << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    //getchar();

    return 0;
}


void mergeRuns(fstream &infile1, fstream &infile2, fstream &outfile, int run1_start, int run2_start, int run2_end)
{

    int read_size_B = block_size_B * sizeof(int);

    int read_blk1_offset = run1_start;
    int read_blk2_offset = run2_start;

    vector<int> bufferBr1(block_size_B);
    vector<int> bufferBr2(block_size_B);

    infile1.read((char *) &bufferBr1[0], read_size_B);
    infile2.seekg(run2_start, ios::cur);
    infile2.read((char *) &bufferBr2[0], read_size_B);

    vector<int> bufferBw(block_size_B);

    int p1 = 0, p2 = 0, po = 0;

    while (read_blk1_offset < run1_start && read_blk2_offset < run2_end)
    {
        if (bufferBr1[p1] < bufferBr2[p2])
        {
            bufferBw[po] = bufferBr1[p1];
            p1++;

            if (p1 > block_size_B)
            {
                read_blk1_offset += read_size_B;
                if (read_blk1_offset < run1_start)
                {
                    infile1.read((char *) &bufferBr1[0], read_size_B);
                    p1 = 0;
                }
            }
        } else
        {
            bufferBw[po] = bufferBr2[p2];
            p2++;
            if (p2 > block_size_B)
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
        if (po > block_size_B)
        {
            outfile.write((char *) &bufferBw[0], read_size_B);
            po = 0;
        }
    }

    if (read_blk1_offset == run1_start)
    {
        outfile.write((char *) &bufferBr2[0], read_size_B);

        for (int i = read_blk2_offset + read_size_B; i < run2_end; i += read_size_B)
        {
            infile2.read((char *) &bufferBr2[0], read_size_B);
            outfile.write((char *) &bufferBr2[0], read_size_B);
        }
    } else if (read_blk2_offset == run2_start)
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
    const int n = 24;
    //srand(static_cast<unsigned int>(time(NULL)));


    vector<int> array(n);
    int c = n;
    for (int i = 0; i < n; ++i)
    {
        array[i] = rand() % n;
        c--;
    }

    for (int i = 0; i < n; ++i)
    {
        cout << setiosflags(ios::fixed | ios::left) << setw(3) << array[i] << " ";
    }

    ofstream out("input.bin", ios::out | ios::binary);

    out.write((char *) &n, sizeof(int));

    out.write((char *) &array[0], n * sizeof(int));

    out.close();
}


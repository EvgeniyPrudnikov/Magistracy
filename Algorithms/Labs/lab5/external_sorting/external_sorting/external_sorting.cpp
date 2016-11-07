
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

void
mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, long run1_start, long run2_start, long run2_end);

void coutFile(char *filename);


int main(int argc, char *argv[])
{

    const auto startTime = std::clock();

    CreateTestFile();

    long N;

    ifstream infile("input.bin", ios::in | ios::binary);
    ofstream tempfile("temp1.bin", ios::out | ios::binary);


    infile.read(reinterpret_cast<char *>(&N), 8);
    tempfile.write(reinterpret_cast<char *>(&N), 8);

    long init_offset = 8;

    vector<long> bufferM(block_size_M);

    long runs = ceil((double) N / block_size_M);
    long read_size_M = block_size_M * sizeof(long);

    for (int j = 0; j < runs; ++j)
    {
        if (N - j * block_size_M < block_size_M)
        {
            read_size_M = (N - j * block_size_M) * sizeof(long);
            bufferM.resize(read_size_M / sizeof(long));
        }

        infile.read((char *) &bufferM[0], read_size_M);

        sort(bufferM.begin(), bufferM.end());

        tempfile.write((char *) &bufferM[0], read_size_M);
    }

    bufferM.clear();
    tempfile.close();
    infile.close();

    //coutFile("temp1.bin");


    ifstream tempfile1;
    ifstream tempfile2;
    ofstream outfile1;

    read_size_M = block_size_M * sizeof(long);

    long levels = ceil(log2((double) N / block_size_M));


    for (int i = 0; i < levels; ++i)
    {
        if (i % 2 == 0)
        {
            tempfile1.open("temp1.bin", ios::in | ios::binary);
            tempfile2.open("temp1.bin", ios::in | ios::binary);
            outfile1.open("temp2.bin", ios::out | ios::binary | ios::trunc);
        } else
        {
            tempfile1.open("temp2.bin", ios::in | ios::binary);
            tempfile2.open("temp2.bin", ios::in | ios::binary);
            outfile1.open("temp1.bin", ios::out | ios::binary | ios::trunc);
        }

        outfile1.write(reinterpret_cast<char *>(&N), 8);

        for (int k = init_offset; k < N * sizeof(long) + init_offset; k += 2 * read_size_M)
        {
            long run1_start = k;
            long run2_start = k + read_size_M;
            long run2_end = k + 2 * read_size_M;
            long lol = (N * sizeof(long) + init_offset) - (k + read_size_M);
            if (lol < read_size_M)
            {
                run2_end = run2_start + ((N * sizeof(long) + init_offset) - (k + read_size_M));
            }

            mergeRuns(tempfile1, tempfile2, outfile1, run1_start, run2_start, run2_end);
        }
        read_size_M *= 2;

        tempfile1.close();
        tempfile2.close();
        outfile1.close();
    }


    rename("temp2.bin", "output.bin");
    coutFile("output.bin");


    const auto endTime = std::clock();
    std::cout << endl << "done in  " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    //getchar();

    return 0;
}


void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, long run1_start, long run2_start, long run2_end)
{
/*
    int block_size_B1 = block_size_B;
    int block_size_B2 = block_size_B;

    int read_size_B1 = block_size_B1 * sizeof(long);
    int read_size_B2 = block_size_B2 * sizeof(long);
*/
    int read_size_B = block_size_B * sizeof(long);

    long read_blk1_offset = run1_start;
    long read_blk2_offset = run2_start;

    vector<long> bufferBr1(block_size_B);
    vector<long> bufferBr2(block_size_B);

    infile1.seekg(run1_start, ios::beg);
    infile1.read((char *) &bufferBr1[0], read_size_B);

    infile2.seekg(run2_start, ios::beg);
    infile2.read((char *) &bufferBr2[0], read_size_B);

    vector<long> bufferBw(block_size_B);

    int p1 = 0, p2 = 0, po = 0;

    while (read_blk1_offset < run2_start && read_blk2_offset < run2_end)
    {

        if ( run2_end - read_blk2_offset < read_size_B)
        {
            //block_size_B2 = (run2_end - read_blk2_offset)/sizeof(long);
            read_blk2_offset += run2_end - read_blk2_offset;
        }


        if (bufferBr1[p1] < bufferBr2[p2])
        {
            bufferBw[po] = bufferBr1[p1];
            p1++;

            if (p1 > block_size_B - 1)
            {

                read_blk1_offset += block_size_B;

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

                read_blk2_offset +=block_size_B;

                if (read_blk2_offset < run2_end)
                {
                    infile2.read((char *) &bufferBr2[0], read_size_B);
                    p2 = 0;
                }
            }
        }

        po++;
        if (po > (run2_end - read_blk2_offset)/sizeof(long) - 1)
        {
            outfile.write((char *) &bufferBw[0], read_size_B);
            po = 0;
        }
    }

    if (read_blk1_offset == run2_start)
    {
        outfile.write((char *) &bufferBr2[0], block_size_B);

        for (int i = read_blk2_offset + block_size_B; i < run2_end; i += block_size_B)
        {
            infile2.read((char *) &bufferBr2[0], block_size_B);
            outfile.write((char *) &bufferBr2[0], block_size_B);
        }
    } else if (read_blk2_offset == run2_end)
    {
        outfile.write((char *) &bufferBr1[0], block_size_B);

        for (int i = read_blk1_offset + block_size_B; i < run2_start; i += block_size_B)
        {
            infile1.read((char *) &bufferBr1[0], block_size_B);
            outfile.write((char *) &bufferBr1[0], block_size_B);
        }
    }
}

void coutFile(char *filename)
{
    cout << endl;
    ifstream infile(filename, ios::in | ios::binary);

    long n;
    infile.read(reinterpret_cast<char *>(&n), 8);
    vector<long> buffer(n, 0);
    infile.read((char *) &buffer[0], buffer.size() * sizeof(long));
    for (int i = 0; i < n; ++i)
    {
        cout << setiosflags(ios::fixed | ios::left) << setw(3) << buffer[i] << " ";
    }
    infile.close();
}

void CreateTestFile()
{
    const long n = 9;
    //srand(static_cast<unsigned int>(time(NULL)));

    vector<long> array(n);
    int c = n;
    for (int i = 0; i < n; ++i)
    {
        array[i] = c;//rand() % n;
        c--;
    }

    for (int i = 0; i < n; ++i)
    {
        cout << setiosflags(ios::fixed | ios::left) << setw(3) << array[i] << " ";
    }

    ofstream out("input.bin", ios::out | ios::binary);

    out.write((char *) &n, sizeof(long));

    out.write((char *) &array[0], n * sizeof(long));

    out.close();
}


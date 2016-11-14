
//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>


using namespace std;
//M = 128 * 256 + 1
const int block_size_M = 4;
const int block_size_B = 2;
const int init_offset = sizeof(int);


struct two
{
    int data[2];
};

struct three
{
    int data[3];
};


void CreateTestFile();

template<typename T>
void createDelList(char *input_filename, char *output_filename);

template<typename T1, typename T2, typename T3>
void Join3(char *input_filename1, char *input_filename2, char *output_filename, int coordinate_f1, int coordinate_f2);

template<typename T>
void ExternalSort(char *input_filename, char *output_filename, int coordinate);

template<typename T>
void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end,
               int coordinate);

template<typename T>
void coutFile(char *filename);


int main(int argc, char *argv[])
{

    const auto startTime = std::clock();

    CreateTestFile();


    ExternalSort<two>("input.bin", "output1.bin", 1);
    ExternalSort<two>("input.bin", "output0.bin", 0);

    Join3<two, two, three>("output1.bin", "output0.bin", "join1.bin", 1, 0);

    coutFile<three>("join1.bin");

    createDelList<two>("input.bin","delList.bin");

    coutFile<two>("delList.bin");

    const auto endTime = std::clock();
    std::cout << endl << "done in  " << setprecision(6) << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    //getchar();

    return 0;
}

template<typename T>
void createDelList(char *input_filename, char *output_filename)
{
    int N;

    vector<two> bufferMw(block_size_M);

    int write_blk_size = block_size_M * sizeof(T);

    ifstream infile(input_filename, ios::in | ios::binary);
    infile.read(reinterpret_cast<char *>(&N), init_offset);
    infile.close();

    ofstream outfile(output_filename, ios::out | ios::binary);
    outfile.write(reinterpret_cast<char *>(&N), init_offset);

    int m = N/block_size_M;
    T tmp;

    for (int i = 0; i < m; ++i)
    {

        if (N - i * block_size_M < block_size_M)
        {
            write_blk_size = (N - i * block_size_M) * sizeof(T);
            bufferMw.resize(write_blk_size / sizeof(T));
        }

        srand(time(NULL));
        for (int j = 0; j < block_size_M; ++j)
        {
            tmp.data[0] = rand() % 2;
            tmp.data[1] = rand() % 2;
            bufferMw[j] = tmp;
        }

        outfile.write((char *) &bufferMw[0], write_blk_size);

    }

    bufferMw.clear();
    vector<T>().swap(bufferMw);
}

/*
 * TODO: redo Join!!!
 */

template<typename T1, typename T2, typename T3>
void Join3(char *input_filename1, char *input_filename2, char *output_filename, int coordinate_f1, int coordinate_f2)
{
    int N1;
    int N2;

    ifstream infile1(input_filename1, ios::in | ios::binary);
    ifstream infile2(input_filename2, ios::in | ios::binary);
    ofstream outfile(output_filename, ios::out | ios::binary);


    infile1.read(reinterpret_cast<char *>(&N1), init_offset);
    infile2.read(reinterpret_cast<char *>(&N2), init_offset);

    int N3 = min(N1, N2);

    outfile.write(reinterpret_cast<char *>(&N3), init_offset);

    vector<T1> bufferBr1(block_size_B);
    vector<T2> bufferBr2(block_size_B);
    vector<T3> bufferWr(block_size_B);

    int read_blk_size1 = block_size_B * sizeof(T1);
    int read_blk_size2 = block_size_B * sizeof(T2);
    int write_blk_size = block_size_B * sizeof(T3);

    int p1 = 0, p2 = 0, po = 0;

    T3 tmp;
    /*
     * TODO: big block size support;
     */
    for (int i = 0; i <= N2; i++)
    {
        int curr_blk = (i / block_size_B);

        if (i == block_size_B * curr_blk)
        {

            if (N1  - curr_blk * block_size_B < read_blk_size1 / sizeof(T1))
            {
                read_blk_size1 = (N1  - curr_blk * block_size_B)* sizeof(T1);
                read_blk_size2 = (N1  - curr_blk * block_size_B)* sizeof(T1);
            }

            if (N3  - curr_blk * block_size_B < write_blk_size/ sizeof(T3))
            {
                write_blk_size = (N3 - curr_blk * block_size_B) * sizeof(T3);
            }


            infile1.read((char *) &bufferBr1[0], read_blk_size1);
            infile2.read((char *) &bufferBr2[0], read_blk_size2);
            if (i != 0) outfile.write((char *) &bufferWr[0], write_blk_size);
            p1 = 0;
            p2 = 0;
            po = 0;
        }

        if (bufferBr1[p1].data[coordinate_f1] == bufferBr2[p2].data[coordinate_f2])
        {
            tmp.data[0] = bufferBr1[p1].data[0];
            tmp.data[1] = bufferBr1[p1].data[1];
            tmp.data[2] = bufferBr2[p2].data[1];

            bufferWr[po] = tmp;

            p1++;
            p2++;
            po++;
        }
    }

    bufferBr1.clear();
    bufferBr2.clear();
    bufferWr.clear();

    vector<T1>().swap(bufferBr1);
    vector<T2>().swap(bufferBr2);
    vector<T3>().swap(bufferWr);

}

template<typename T>
void ExternalSort(char *input_filename, char *output_filename, int coordinate)
{
    int N;

    ifstream infile(input_filename, ios::in | ios::binary);
    ofstream tempfile("temp1.bin", ios::out | ios::binary);


    infile.read(reinterpret_cast<char *>(&N), init_offset);
    tempfile.write(reinterpret_cast<char *>(&N), init_offset);

    //======================================================================================================================
    // Phase 1
    //======================================================================================================================
    vector<T> bufferM(block_size_M);

    int runs = ceil((double) N / block_size_M);
    int read_size_M = block_size_M * sizeof(T);

    for (int j = 0; j < runs; ++j)
    {
        if (N - j * block_size_M < block_size_M)
        {
            read_size_M = (N - j * block_size_M) * sizeof(T);
            bufferM.resize(read_size_M / sizeof(T));
        }

        infile.read((char *) &bufferM[0], read_size_M);

        sort(bufferM.begin(), bufferM.end(), [coordinate](T const &a, T const &b) -> bool
        { return a.data[coordinate] < b.data[coordinate]; });

        tempfile.write((char *) &bufferM[0], read_size_M);
    }

    bufferM.clear();
    vector<T>().swap(bufferM);
    tempfile.close();
    infile.close();

    //======================================================================================================================
    // Phase 2
    //======================================================================================================================

    if (N > block_size_M)
    {

        ifstream tempfile1;
        ifstream tempfile2;
        ofstream outfile1;

        read_size_M = block_size_M * sizeof(T);

        int levels = (int) ceil(log2(runs));


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

            outfile1.write(reinterpret_cast<char *>(&N), init_offset);

            int end_of_file = N * sizeof(T) + init_offset;

            for (int offset = init_offset; offset < end_of_file; offset += 2 * read_size_M)
            {
                int run1_start = offset;
                int run2_start = run1_start + read_size_M;
                int run2_end = run1_start + 2 * read_size_M;

                if (end_of_file - run1_start < read_size_M)
                {
                    run2_start = run1_start;
                }

                if (end_of_file - run2_start < read_size_M)
                {
                    run2_end = end_of_file;
                }

                if (run2_end == run2_start)
                {
                    run2_start = run1_start;
                }

                mergeRuns<T>(tempfile1, tempfile2, outfile1, run1_start, run2_start, run2_end, coordinate);
            }
            read_size_M *= 2;

            tempfile1.close();
            tempfile2.close();
            outfile1.close();
        }

        levels % 2 == 0 ? rename("temp1.bin", output_filename) : rename("temp2.bin", output_filename);
        levels % 2 == 0 ? remove("temp2.bin") : remove("temp1.bin");
        //coutFile<T>((char *)output_filename);

    } else
    {
        rename("temp1.bin", output_filename);
        //coutFile<T>((char *)output_filename);
    }
}

template<typename T>
void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end,
               int coordinate)
{


    int blk_size_B1 = block_size_B;
    int blk_size_B2 = block_size_B;

    int read_blk_size1 = block_size_B * sizeof(T);
    int read_blk_size2 = block_size_B * sizeof(T);

    int read_blk_offset1 = run1_start;
    int read_blk_offset2 = run2_start;


    if (run2_end - read_blk_offset2 < read_blk_size2)
    {
        blk_size_B2 = (run2_end - read_blk_offset2) / sizeof(T);
        read_blk_size2 = (run2_end - read_blk_offset2);
    }


    vector<T> bufferBr1(blk_size_B1);
    vector<T> bufferBr2(blk_size_B2);
    vector<T> bufferBw(blk_size_B1);

    infile1.seekg(read_blk_offset1, ios::beg);
    infile1.read((char *) &bufferBr1[0], read_blk_size1);


    infile2.seekg(read_blk_offset2, ios::beg);
    infile2.read((char *) &bufferBr2[0], read_blk_size2);


    int p1 = 0, p2 = 0, po = 0;

    while (read_blk_offset1 < run2_start && read_blk_offset2 < run2_end)
    {

        if (run2_end - read_blk_offset2 < read_blk_size2)
        {
            blk_size_B2 = (run2_end - read_blk_offset2) / sizeof(T);
            read_blk_size2 = (run2_end - read_blk_offset2);
        }


        if (bufferBr1[p1].data[coordinate] < bufferBr2[p2].data[coordinate])
        {
            bufferBw[po] = bufferBr1[p1];
            p1++;

            if (p1 > blk_size_B1 - 1)
            {
                read_blk_offset1 += read_blk_size1;

                if (read_blk_offset1 < run2_start)
                {
                    infile1.read((char *) &bufferBr1[0], read_blk_size1);
                    p1 = 0;
                }
            }
        } else
        {
            bufferBw[po] = bufferBr2[p2];
            p2++;

            if (p2 > blk_size_B2 - 1)
            {
                read_blk_offset2 += read_blk_size2;

                if (read_blk_offset2 < run2_end)
                {
                    infile2.read((char *) &bufferBr2[0], read_blk_size2);
                    p2 = 0;
                }
            }
        }

        po++;
        if (po > blk_size_B1 - 1)
        {
            outfile.write((char *) &bufferBw[0], read_blk_size1);
            po = 0;
        }
    }


    if (read_blk_offset1 == run2_start)
    {
        int i;
        for (i = p2; i < blk_size_B2; ++i)
        {
            bufferBw[po] = bufferBr2[i];
            po++;
            if (po > blk_size_B1 - 1) break;
        }
        outfile.write((char *) &bufferBw[0], po * sizeof(T));

        if (++i < blk_size_B2)
        {
            outfile.write((char *) &bufferBr2[i], (blk_size_B2 - i) * sizeof(T));
        }

        for (int j = read_blk_offset2 + read_blk_size2; j < run2_end; j += read_blk_size2)
        {
            if (run2_end - j < read_blk_size2)
            {
                read_blk_size2 = run2_end - j;
            }
            infile2.read((char *) &bufferBr2[0], read_blk_size2);
            outfile.write((char *) &bufferBr2[0], read_blk_size2);
        }
    } else if (read_blk_offset2 == run2_end)
    {
        int i;
        for (i = p1; i < blk_size_B1; ++i)
        {
            bufferBw[po] = bufferBr1[i];
            po++;
            if (po > blk_size_B1 - 1) break;
        }

        outfile.write((char *) &bufferBw[0], po * sizeof(T));

        if (++i < blk_size_B1)
        {
            outfile.write((char *) &bufferBr1[i], (blk_size_B1 - i) * sizeof(T));
        }

        for (int j = read_blk_offset1 + read_blk_size1; j < run2_start; j += read_blk_size1)
        {
            if (run2_start - j < read_blk_size1)
            {
                read_blk_size1 = run2_start - j;
            }
            infile1.read((char *) &bufferBr1[0], read_blk_size1);
            outfile.write((char *) &bufferBr1[0], read_blk_size1);
        }
    }

    bufferBr1.clear();
    bufferBr2.clear();
    bufferBw.clear();

    vector<T>().swap(bufferBr1);
    vector<T>().swap(bufferBr2);
    vector<T>().swap(bufferBw);
}

template<typename T>
void coutFile(char *filename)
{
    cout << endl << endl;
    ifstream infile(filename, ios::in | ios::binary);

    int b;
    infile.read(reinterpret_cast<char *>(&b), init_offset);
    vector<T> buffer(b);
    infile.read((char *) &buffer[0], buffer.size() * sizeof(T));

    int l = sizeof(T) / sizeof(int);

    for (int i = 0; i < b; ++i)
    {
        for (int j = 0; j < l; ++j)
        {
            cout << setiosflags(ios::fixed | ios::left) << buffer[i].data[j] << " ";
        }
        cout << endl;
    }
    infile.close();

    vector<T>().swap(buffer);
}


void CreateTestFile()
{
    int n = 10;
    srand(static_cast<unsigned int>(time(NULL)));

    vector<int> array{6, 7, 7, 1, 1, 3, 3, 2, 2, 8, 8, 5, 5, 4, 4, 10, 10, 9, 9, 6};
/*
	int c = n;
	for (int i = 0; i < n*2; ++i)
	{
		array[i] = rand() % n;
		c--;
	}
  */
    for (int i = 0; i < n * 2 - 1; i += 2)
    {
        cout << setiosflags(ios::fixed | ios::left) << setprecision(2) << setw(1) << array[i] << " " << array[i + 1]
             << endl;
    }

    ofstream out("input.bin", ios::out | ios::binary | ios::trunc);

    out.write((char *) &n, sizeof(int));

    out.write((char *) &array[0], n * 2 * sizeof(int));

    out.close();
    vector<int>().swap(array);
}


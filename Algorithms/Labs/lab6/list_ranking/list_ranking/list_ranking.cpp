
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>


using namespace std;

const int block_size_M = 4;
const int block_size_B = 2;


struct two
{
	int data[2];
};


void CreateTestFile();
template <typename T>
void ExternalSort (char* input_filename, char* output_filename, int coordinate);
template <typename T>
void mergeRuns (ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end , int coordinate);
template<typename T>
void coutFile (char *filename);


int main(int argc, char *argv[])
{

	const auto startTime = std::clock();

	CreateTestFile();

	ExternalSort<two>("input.bin", "output.bin", 1);

	const auto endTime = std::clock();
	std::cout << endl << "done in  " << setprecision(6) << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	getchar();

	return 0;
}

template <typename T>
void ExternalSort( char* input_filename, char* output_filename, int  coordinate)
{
	int N;

	ifstream infile(input_filename, ios::in | ios::binary);
	ofstream tempfile("temp1.bin", ios::out | ios::binary);


	infile.read(reinterpret_cast<char *>(&N), 4);
	tempfile.write(reinterpret_cast<char *>(&N), 4);

	int init_offset = 4;
	//======================================================================================================================
	// Phase 1
	//======================================================================================================================
	vector<T> bufferM(block_size_M);

	int runs = ceil((double)N / block_size_M);
	int read_size_M = block_size_M * sizeof(T);

	for (int j = 0; j < runs; ++j)
	{
		if (N - j * block_size_M < block_size_M)
		{
			read_size_M = (N - j * block_size_M) * sizeof(T);
			bufferM.resize(read_size_M / sizeof(T));
		}

		infile.read((char *)&bufferM[0], read_size_M);

		sort(bufferM.begin(), bufferM.end(), [coordinate](T const & a, T const & b) -> bool { return a.data[coordinate] < b.data[coordinate]; });

		tempfile.write((char *)&bufferM[0], read_size_M);
	}

	bufferM.clear();
	tempfile.close();
	infile.close();

	//coutFile<T>((char *) "temp1.bin");

	//======================================================================================================================
	// Phase 2
	//======================================================================================================================

	if (N > block_size_M)
	{

		ifstream tempfile1;
		ifstream tempfile2;
		ofstream outfile1;

		read_size_M = block_size_M * sizeof(T);

		int levels = (int)ceil(log2(runs));


		for (int i = 0; i < levels; ++i)
		{
			if (i % 2 == 0)
			{
				tempfile1.open("temp1.bin", ios::in | ios::binary);
				tempfile2.open("temp1.bin", ios::in | ios::binary);
				outfile1.open("temp2.bin", ios::out | ios::binary | ios::trunc);
			}
			else
			{
				tempfile1.open("temp2.bin", ios::in | ios::binary);
				tempfile2.open("temp2.bin", ios::in | ios::binary);
				outfile1.open("temp1.bin", ios::out | ios::binary | ios::trunc);
				//coutFile<T>((char *) "temp2.bin");
			}

			outfile1.write(reinterpret_cast<char *>(&N), 4);

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

				mergeRuns <T> (tempfile1, tempfile2, outfile1, run1_start, run2_start, run2_end, coordinate);
			}
			read_size_M *= 2;

			tempfile1.close();
			tempfile2.close();
			outfile1.close();
		}

		//coutFile<T>((char *) "temp1.bin");
		//coutFile<T>((char *) "temp2.bin");
		levels % 2 == 0 ? rename("temp1.bin", output_filename) : rename("temp2.bin", output_filename);
		coutFile<T>((char *)output_filename);

	}
	else
	{
		rename("temp1.bin", output_filename);
		coutFile<T>((char *)output_filename);
	}
}

template <typename T>
void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end, int coordinate)
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
	infile1.read((char *)&bufferBr1[0], read_blk_size1);


	infile2.seekg(read_blk_offset2, ios::beg);
	infile2.read((char *)&bufferBr2[0], read_blk_size2);


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
					infile1.read((char *)&bufferBr1[0], read_blk_size1);
					p1 = 0;
				}
			}
		}
		else
		{
			bufferBw[po] = bufferBr2[p2];
			p2++;

			if (p2 > blk_size_B2 - 1)
			{
				read_blk_offset2 += read_blk_size2;

				if (read_blk_offset2 < run2_end)
				{
					infile2.read((char *)&bufferBr2[0], read_blk_size2);
					p2 = 0;
				}
			}
		}

		po++;
		if (po > blk_size_B1 - 1)
		{
			outfile.write((char *)&bufferBw[0], read_blk_size1);
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
		outfile.write((char *)&bufferBw[0], po * sizeof(T));

		if (++i < blk_size_B2)
		{
			outfile.write((char *)&bufferBr2[i], (blk_size_B2 - i) * sizeof(T));
		}

		for (int j = read_blk_offset2 + read_blk_size2; j < run2_end; j += read_blk_size2)
		{
			if (run2_end - j < read_blk_size2)
			{
				read_blk_size2 = run2_end - j;
			}
			infile2.read((char *)&bufferBr2[0], read_blk_size2);
			outfile.write((char *)&bufferBr2[0], read_blk_size2);
		}
	}
	else if (read_blk_offset2 == run2_end)
	{
		int i;
		for (i = p1; i < blk_size_B1; ++i)
		{
			bufferBw[po] = bufferBr1[i];
			po++;
			if (po > blk_size_B1 - 1) break;
		}

		outfile.write((char *)&bufferBw[0], po * sizeof(T));

		if (++i < blk_size_B1)
		{
			outfile.write((char *)&bufferBr1[i], (blk_size_B1 - i) * sizeof(T));
		}

		for (int j = read_blk_offset1 + read_blk_size1; j < run2_start; j += read_blk_size1)
		{
			if (run2_start - j < read_blk_size1)
			{
				read_blk_size1 = run2_start - j;
			}
			infile1.read((char *)&bufferBr1[0], read_blk_size1);
			outfile.write((char *)&bufferBr1[0], read_blk_size1);
		}
	}

	bufferBr1.clear();
	bufferBr2.clear();
	bufferBw.clear();
}

template<typename T>
void coutFile(char *filename)
{
	cout << endl << endl;
	ifstream infile(filename, ios::in | ios::binary);

	int b;
	infile.read(reinterpret_cast<char *>(&b), 4);
	vector<T> buffer(b);
	infile.read((char *)&buffer[0], buffer.size() * sizeof(T));
	for (int i = 0; i < b; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left)  << buffer[i].data[0] << " " << buffer[i].data[1] << setw(4) << " ";
	}
	infile.close();
}

void CreateTestFile()
{
	int n = 4;
	srand(static_cast<unsigned int>(time(NULL)));

	vector<int> array {10, 1, 9, 2, 8, 3, 7, 4};//{6,7,7,1,1,3,3,2,2,8,8,5,5,4,4,10,10,9,9,6};
					 //7,4 , 8,3 , 9,2 , 10,1
	/*int c = n;
	for (int i = 0; i < n; ++i)
	{
		array[i] = rand() % n;
		c--;
	}
	*/
	for (int i = 0; i < n*2; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << setprecision(2) << setw(4) << array[i] << " ";
	}

	ofstream out("input.bin", ios::out | ios::binary | ios::trunc);

	out.write((char *)&n, sizeof(int));

	out.write((char *) &array[0], n*2 * sizeof(int));

	out.close();
}


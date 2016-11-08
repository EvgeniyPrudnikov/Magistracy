
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>


using namespace std;

const long long block_size_M = 6;
const long long block_size_B = 3;

void CreateTestFile();

void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, long long run1_start, long long run2_start, long long run2_end);

void coutFile(char *filename);


int main(int argc, char *argv[])
{

	const auto startTime = std::clock();

	CreateTestFile();

	long long N;

	ifstream infile("input.bin", ios::in | ios::binary);
	ofstream tempfile("temp1.bin", ios::out | ios::binary);


	infile.read(reinterpret_cast<char *>(&N), sizeof(double));
	tempfile.write(reinterpret_cast<char *>(&N), sizeof(double));

	long long init_offset = sizeof(double);
	//======================================================================================================================
	// Phase 1
	//======================================================================================================================
	vector<double> bufferM(block_size_M);

	long long runs = ceil((double)N / block_size_M);
	long long read_size_M = block_size_M * sizeof(double);

	for (int j = 0; j < runs; ++j)
	{
		if (N - j * block_size_M < block_size_M)
		{
			read_size_M = (N - j * block_size_M) * sizeof(double);
			bufferM.resize(read_size_M / sizeof(double));
		}

		infile.read((char *)&bufferM[0], read_size_M);

		sort(bufferM.begin(), bufferM.end());

		tempfile.write((char *)&bufferM[0], read_size_M);
	}

	bufferM.clear();
	tempfile.flush();
	tempfile.close();
	infile.close();

	coutFile((char *) "temp1.bin");

	//======================================================================================================================
	// Phase 2
	//======================================================================================================================

	if (N > block_size_M)
	{

		ifstream tempfile1;
		ifstream tempfile2;
		ofstream outfile1;

		read_size_M = block_size_M * sizeof(double);

		long long levels = (long long)ceil(log2(runs));


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
				coutFile((char *) "temp2.bin");
			}

			outfile1.write(reinterpret_cast<char *>(&N), sizeof(double));

			for (long long k = init_offset; k < N * sizeof(double) + init_offset; k += 2 * read_size_M)
			{
				long long run1_start = k;
				long long run2_start = k + read_size_M;
				long long run2_end = k + 2 * read_size_M;

				if ((N * sizeof(double) + init_offset) - k < read_size_M)
				{
					run2_start = k;
				}

				if ((N * sizeof(double) + init_offset) - run2_start < read_size_M)
				{
					run2_end = (N * sizeof(double) + init_offset);
				}

				if (run2_end == run2_start)
				{
					run2_start = k;
				}

				mergeRuns(tempfile1, tempfile2, outfile1, run1_start, run2_start, run2_end);
			}
			read_size_M *= 2;

			tempfile1.close();
			tempfile2.close();
			outfile1.close();
		}

		//coutFile((char *) "temp1.bin");
		//coutFile((char *) "temp2.bin");
		levels % 2 == 0 ? rename("temp1.bin", "output.bin") : rename("temp2.bin", "output.bin");
		coutFile((char *) "output.bin");

	}
	else
	{
		rename("temp1.bin", "output.bin");
		coutFile((char *) "output.bin");
	}

	const auto endTime = std::clock();
	std::cout << endl << "done in  " << setprecision(6) << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	getchar();

	return 0;
}


void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, long long run1_start, long long run2_start, long long run2_end)
{


	long long blk_size_B1 = block_size_B;
	long long blk_size_B2 = block_size_B;

	long long read_blk_size1 = block_size_B * sizeof(double);
	long long read_blk_size2 = block_size_B * sizeof(double);

	long long read_blk_offset1 = run1_start;
	long long read_blk_offset2 = run2_start;


	if (run2_end - read_blk_offset2 < read_blk_size2)
	{
		blk_size_B2 = (run2_end - read_blk_offset2) / sizeof(double);
		read_blk_size2 = (run2_end - read_blk_offset2);
	}


	vector<double> bufferBr1(blk_size_B1);
	vector<double> bufferBr2(blk_size_B2);
	vector<double> bufferBw(blk_size_B1);

	infile1.seekg(read_blk_offset1, ios::beg);
	infile1.read((char *)&bufferBr1[0], read_blk_size1);


	infile2.seekg(read_blk_offset2, ios::beg);
	infile2.read((char *)&bufferBr2[0], read_blk_size2);


	long long p1 = 0, p2 = 0, po = 0;

	while (read_blk_offset1 < run2_start && read_blk_offset2 < run2_end)
	{

		if (run2_end - read_blk_offset2 < read_blk_size2)
		{
			blk_size_B2 = (run2_end - read_blk_offset2) / sizeof(double);
			read_blk_size2 = (run2_end - read_blk_offset2);
		}


		if (bufferBr1[p1] < bufferBr2[p2])
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
		long long i;
		for (i = p2; i < blk_size_B2; ++i)
		{
			bufferBw[po] = bufferBr2[i];
			po++;
			if (po > blk_size_B1 - 1) break;
		}
		outfile.write((char *)&bufferBw[0], po * sizeof(double));
		//read_blk_offset2 += po * sizeof(double);

		if (++i < blk_size_B2)
		{
			outfile.write((char *)&bufferBr2[i], (blk_size_B2 - i) * sizeof(double));
		}

		for (long long j = read_blk_offset2 + read_blk_size2; j < run2_end; j += read_blk_size2)
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
		long long i;
		for (i = p1; i < blk_size_B1; ++i)
		{
			bufferBw[po] = bufferBr1[i];
			po++;
			if (po > blk_size_B1 - 1) break;
		}

		outfile.write((char *)&bufferBw[0], po * sizeof(double));
		//read_blk_offset1 += po * sizeof(double);

		if (++i < blk_size_B1 )
		{
			outfile.write((char *)&bufferBr1[i], (blk_size_B1 - i) * sizeof(double));
		}

		for (long long j = read_blk_offset1 + read_blk_size1; j < run2_start; j += read_blk_size1)
		{
			if (run2_start - j < read_blk_size1)
			{
				read_blk_size1 = run2_start - j;
			}
			infile1.read((char *)&bufferBr1[0], read_blk_size1);
			outfile.write((char *)&bufferBr1[0], read_blk_size1);
		}
	}
}

void coutFile(char *filename)
{
	cout << endl << endl;
	ifstream infile(filename, ios::in | ios::binary);

	long long b;
	infile.read(reinterpret_cast<char *>(&b), sizeof(double));
	vector<double> buffer(b, 0);
	infile.read((char *)&buffer[0], buffer.size() * sizeof(double));
	for (int i = 0; i < b; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << setprecision(2) << setw(4) << buffer[i] << " ";
	}
	infile.close();
}

void CreateTestFile()
{
	long long n =25;
	srand(static_cast<unsigned int>(time(NULL)));

	vector<double> array(n); //{9.59151, 13.19813, 13.52202, 5.75033, 10.43956, 12.70324, 13.74969, 13.38816, 7.55983, 15.90561, 0.97864, 12.07022, 11.87048, 2.45158, 12.47136, 9.16660};
	
	int c = n;
	for (int i = 0; i < n; ++i)
	{
		array[i] = ((double)rand() / RAND_MAX) + rand() % n;
		c--;
	}
	
	for (int i = 0; i < n; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << setprecision(2) << setw(4) << array[i] << " ";
	}

	ofstream out("input.bin", ios::out | ios::binary | ios::trunc);

	out.write((char *)&n, sizeof(double));

	out.write((char *) &array[0], n * sizeof(double));

	out.close();

	sort(array.begin(), array.end());
	cout << endl;
	for (int i = 0; i < n; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << setprecision(2) << setw(4) << array[i] << " ";
	}
}


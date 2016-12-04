#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <map> 
#include <string>

using namespace std;

const int block_size_M = 40 * 40;
const int block_size_B = 20 * 20;
const int init_offset = sizeof(int);


struct long_two
{
	long long data[2];
};

struct two
{
	int data[2];

	bool operator==(const two &rhs) const
	{
		return data[0] == rhs.data[0] && data[1] == rhs.data[1];
	}
};

struct three
{
	int data[3];
};


template <typename T1, typename T2>
int JoinInsert(string input_filename1, string input_filename2, string output_filename);

template<typename T1, typename T2>
void Rank(string input_filename, string output_filename);

template<typename T1>
int JoinDel(string input_filename1, string input_filename2, string output_filename);

template<typename T1, typename T2>
void createDelList(string input_filename1, string input_filename2, string output_filename1, string output_filename2);

template <typename T1, typename T2>
void RandAi(string input_filename, string output_filename);

template<typename T>
void ExternalSort(string input_filename, string output_filename, int coordinate);

template<typename T>
void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end, int coordinate);

template <typename T1, typename T2>
void Output(string input_filename, string output_filename);

int main()
{
	int N;
	int N_down;
	int N_up = 0;

	ifstream infile("input.bin", ios::in | ios::binary);
	infile.read(reinterpret_cast<char *>(&N), init_offset);
	infile.close();
	
	N_down = N;
	string i_filename;
	int v = 1;

	int blk_size_M = block_size_M;

	if (N_down <= blk_size_M)
	{
		blk_size_M = N_down;
		N_up = N_down;
	}

	while (N_down > blk_size_M)
	{
		i_filename = v == 1 ? "input.bin" : "6.bin";

		ExternalSort<two>(i_filename, "s_1.bin", 1);

		RandAi<two, three>("s_1.bin", "r_1.bin");

		ExternalSort<three>("r_1.bin", "r_2.bin", 1);

		createDelList<three, two>("r_1.bin", "r_2.bin", "l_4.bin", "del_" + to_string(v) + ".bin");

		ExternalSort<two>("l_4.bin", "s_l_5.bin", 0);

		N_down = JoinDel<two>("s_l_5.bin", "del_" + to_string(v) + ".bin", "6.bin");

		v++;
		remove("s_1.bin");
		remove("r_1.bin");
		remove("r_2.bin");
		remove("s_l_5.bin");
	}

	i_filename = v == 1 ? "input.bin" : "6.bin";

	ExternalSort<two>(i_filename, "s_6.bin", 0);

	v--;
	Rank<two, long_two>("s_6.bin", "rnk.bin");

	while (N_up != N)
	{
		ExternalSort<long_two>("rnk.bin", "1.bin", 1); ExternalSort<two>("del_" + to_string(v) + ".bin", "2.bin", 0);
		N_up = JoinInsert<two, long_two>("1.bin", "2.bin", "rnk.bin");
		v--;
		remove("1.bin");
		remove("2.bin");
	}

	ExternalSort<long_two>("rnk.bin", "res.bin", 0);

	Output<long_two, int>("res.bin", "output.bin");

	return 0;
}

template <typename T1, typename T2>
void Output(string input_filename, string output_filename)
{
	int N;

	ifstream infile(input_filename, ios::in | ios::binary);
	infile.read(reinterpret_cast<char *>(&N), init_offset);

	ofstream outfile(output_filename, ios::out | ios::binary);

	vector<T1> bufferMr(block_size_M);
	vector<T2> bufferMw(block_size_M);

	int read_blk_size = block_size_M * sizeof(T1);
	int write_blk_size = block_size_M * sizeof(T2);

	int m = ceil((double)N / block_size_M);

	for (int i = 0; i < m; i++)
	{
		if (N - i * block_size_M < block_size_M)
		{
			read_blk_size = (N - i * block_size_M) * sizeof(T1);
			bufferMr.resize(read_blk_size / sizeof(T1));

			write_blk_size = (N - i * block_size_M) * sizeof(T2);
			bufferMw.resize(write_blk_size / sizeof(T2));
		}

		infile.read((char *)&bufferMr[0], read_blk_size);

		for (int j = 0; j < bufferMr.size(); j++)
		{
			bufferMw[j] = (int)bufferMr[j].data[1];
		}
		outfile.write((char *)&bufferMw[0], write_blk_size);
	}

	infile.close();
	outfile.close();

	vector<T1>().swap(bufferMr);
	vector<T2>().swap(bufferMw);
}

template <typename T1, typename T2 >
int JoinInsert(string  input_filename1, string  input_filename2, string  output_filename)
{
	int N1;
	int N2;
	int N3 = 0;
	int	inserted_elems = 0;

	ifstream infile1(input_filename1, ios::in | ios::binary);
	ifstream infile2(input_filename2, ios::in | ios::binary);

	infile1.read(reinterpret_cast<char *>(&N1), init_offset);
	infile2.read(reinterpret_cast<char *>(&N2), init_offset);

	ofstream outfile(output_filename, ios::out | ios::binary | ios::trunc);
	outfile.seekp(init_offset, ios::beg);

	int block_size_file1 = min(N1, block_size_M);
	int block_size_file2 = min(N2, block_size_M);

	vector<T2> bufferMr1(block_size_file1);
	vector<T1> bufferMr2(block_size_file2);
	vector<T2> bufferMw(block_size_M);

	int read_blk_size1 = block_size_file1 * sizeof(T2);
	int read_blk_size2 = block_size_file2 * sizeof(T1);
	int write_blk_size = block_size_M * sizeof(T2);

	int m = ceil(static_cast<double>(max(N1, N2)) / block_size_M);
	int blk2_reads = 0;
	int p_read1 = 0;
	int p_read2 = 0;
	int p_write = 0;

	for (int i = 0; i < m; i++)
	{
		if (N1 - i * block_size_file1 < block_size_file1)
		{
			read_blk_size1 = (N1 - i * block_size_file1) * sizeof(T2);
			bufferMr1.resize(read_blk_size1 / sizeof(T2));
		}

		if (N1 - i * block_size_M < block_size_M)
		{
			if (p_write > 0 && p_write <= bufferMw.size())
			{
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T2));
				N3 += p_write;
				p_write = 0;
			}

			write_blk_size = (N1 - i * block_size_M) * sizeof(T2);
			bufferMw.resize(write_blk_size / sizeof(T2));
		}
		infile1.read((char *)&bufferMr1[0], read_blk_size1);

		if (i == 0)
		{
			if (N2 - i * block_size_file2 < block_size_file2 && N2 - i * block_size_file2 > 0)
			{
				read_blk_size2 = (N2 - i * block_size_file2) * sizeof(T1);
				bufferMr2.resize(read_blk_size2 / sizeof(T1));
			}
			infile2.read((char *)&bufferMr2[0], read_blk_size2);
			blk2_reads++;
		}

		if (inserted_elems < N2)
		{
			for (p_read1 = 0; p_read1 < bufferMr1.size(); p_read1++)
			{
				if (bufferMr1[p_read1].data[1] == bufferMr2[p_read2].data[0])
				{
					if (p_write == bufferMw.size())
					{
						outfile.write((char *)& bufferMw[0], write_blk_size);
						N3 += bufferMw.size();
						p_write = 0;
					}

					bufferMw[p_write++] = { 2 * bufferMr1[p_read1].data[0], bufferMr1[p_read1].data[1] };

					if (p_write == bufferMw.size())
					{
						outfile.write((char *)& bufferMw[0], write_blk_size);
						N3 += bufferMw.size();
						p_write = 0;
					}

					bufferMw[p_write++] = { (2 * bufferMr1[p_read1].data[0]) + 1, bufferMr2[p_read2].data[1] };

					inserted_elems++;
					if (inserted_elems == N2) continue;
					
					p_read2++;
					if (p_read2 == bufferMr2.size())
					{
						if (N2 - blk2_reads * block_size_file2 < block_size_file2 && N2 - blk2_reads * block_size_file2 > 0)
						{
							read_blk_size2 = (N2 - blk2_reads * block_size_file2) * sizeof(T1);
							bufferMr2.resize(read_blk_size2 / sizeof(T1));
						}
						infile2.read((char *)&bufferMr2[0], read_blk_size2);
						blk2_reads++;
						p_read2 = 0;
					}
				}
				else
				{
					if (p_write == bufferMw.size())
					{
						outfile.write((char *)& bufferMw[0], write_blk_size);
						N3 += bufferMw.size();
						p_write = 0;
					}
					bufferMw[p_write++] = { 2 * bufferMr1[p_read1].data[0] , bufferMr1[p_read1].data[1] };
				}
			}
		}
		else
		{
			if (p_write > 0 && p_write <= bufferMw.size())
			{
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T2));
				N3 += p_write;
				p_write = 0;
			}
			
			for (int u = p_write; u < bufferMr1.size(); u++)
			{
				bufferMw[u] = { 2 * bufferMr1[u].data[0] , bufferMr1[u].data[1] };
			}
			outfile.write((char *)&bufferMw[0], read_blk_size1);
			N3 += (int)bufferMw.size();
		}
	}

	if (p_write > 0 && p_write <= bufferMw.size())
	{
		outfile.write((char *)&bufferMw[0], p_write * sizeof(T2));
		N3 += p_write;
	}

	vector<T2>().swap(bufferMr1);
	vector<T1>().swap(bufferMr2);
	vector<T2>().swap(bufferMw);

	outfile.seekp(0, ios::beg);
	outfile.write((char *)&N3, init_offset);
	outfile.close();
	infile1.close();
	infile2.close();

	return N3;
}

template<typename T1, typename T2>
void Rank(string input_filename, string output_filename)
{
	int N;

	ifstream infile(input_filename, ios::in | ios::binary);
	infile.read(reinterpret_cast<char *>(&N), init_offset);

	vector<T1> bufferMr(N);
	vector<T2> bufferMw;
	map<int, int> map_buffer;

	infile.read((char*)&bufferMr[0], N * sizeof(T1));

	ofstream outfile(output_filename, ios::out | ios::binary | ios::trunc);
	outfile.write((char*)&N, init_offset);

	int val_next = 0;
	int key_next = 0;
	int j = 1;

	val_next = bufferMr[0].data[0];
	bufferMw.push_back({ j++, val_next });

	val_next = bufferMr[0].data[1];
	key_next = val_next;
	bufferMw.push_back({ j++, val_next });

	for (auto const& x : bufferMr)
	{
		map_buffer.emplace(move(x.data[0]), move(x.data[1]));
	}

	while (j <= N)
	{
		val_next = map_buffer[key_next];
		bufferMw.push_back({ j++, val_next });
		key_next = val_next;
	}

	outfile.write((char *)& bufferMw[0], N * sizeof(T2));

	vector<T1>().swap(bufferMr);
	vector<T2>().swap(bufferMw);

	infile.close();
	outfile.close();
}

template<typename T1>
int JoinDel(string input_filename1, string input_filename2, string output_filename)
{
	int N1;
	int N2;
	int N3 = 0;
	int deleted_elems = 0;

	ifstream infile1(input_filename1, ios::in | ios::binary);
	ifstream infile2(input_filename2, ios::in | ios::binary);

	infile1.read(reinterpret_cast<char *>(&N1), init_offset);
	infile2.read(reinterpret_cast<char *>(&N2), init_offset);

	ofstream outfile(output_filename, ios::out | ios::binary | ios::trunc);
	outfile.seekp(init_offset, ios::beg);

	vector<T1> bufferMr1(block_size_M);
	vector<T1> bufferMr2(block_size_M);
	vector<T1> bufferMw(block_size_M);

	int read_blk_size1 = block_size_M * sizeof(T1);
	int read_blk_size2 = block_size_M * sizeof(T1);
	int write_blk_size = block_size_M * sizeof(T1);

	int m = ceil(static_cast<double>(max(N1, N2)) / block_size_M);
	int p_read1 = 0;
	int p_read2 = 0;
	int p_write = 0;

	for (int i = 0; i < m; ++i)
	{
		if (N1 - i * block_size_M < block_size_M)
		{
			read_blk_size1 = (N1 - i * block_size_M) * sizeof(T1);
			bufferMr1.resize(read_blk_size1 / sizeof(T1));
		}

		if (N1 - i * block_size_M < block_size_M)
		{
			if (p_write > 0 && p_write <= bufferMw.size())
			{
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T1));
				N3 += p_write;
				p_write = 0;
			}

			write_blk_size = (N1 - i * block_size_M) * sizeof(T1);
			bufferMw.resize(write_blk_size / sizeof(T1));
		}
		infile1.read((char *)&bufferMr1[0], read_blk_size1);

		if (i == 0)
		{
			if (N2 - i * block_size_M < block_size_M && N2 - i * block_size_M > 0)
			{
				read_blk_size2 = (N2 - i * block_size_M) * sizeof(T1);
				bufferMr2.resize(read_blk_size2 / sizeof(T1));
			}

			infile2.read((char *)&bufferMr2[0], read_blk_size2);
		}

		if (deleted_elems < N2)
		{
			for (p_read1 = 0; p_read1 < bufferMr1.size(); ++p_read1)
			{
				if (bufferMr1[p_read1].data[0] != bufferMr2[p_read2].data[1])
				{

					bufferMw[p_write] = bufferMr1[p_read1];
					p_write++;

					if (p_write == bufferMw.size())
					{
						outfile.write((char *)&bufferMw[0], write_blk_size);
						N3 += (int)bufferMw.size();
						p_write = 0;
					}
				}
				else
				{
					deleted_elems++;
					if (deleted_elems == N2) continue;
					p_read2++;

					if (p_read2 == bufferMr2.size())
					{
						if (N2 - i * block_size_M < block_size_M && i > 0 && N2 - i * block_size_M > 0)
						{
							read_blk_size2 = (N2 - i * block_size_M) * sizeof(T1);
							bufferMr2.resize(read_blk_size2 / sizeof(T1));
						}

						infile2.read((char *)&bufferMr2[0], read_blk_size2);
						p_read2 = 0;
					}
				}
			}
		}
		else
		{
			if (p_write > 0 && p_write <= bufferMw.size())
			{
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T1));
				N3 += p_write;
				p_write = 0;
			}
			outfile.write((char *)&bufferMr1[0], read_blk_size1);
			N3 += (int)bufferMr1.size();
		}
	}

	if (p_write > 0 && p_write <= bufferMw.size())
	{
		outfile.write((char *)&bufferMw[0], p_write * sizeof(T1));
		N3 += p_write;
	}

	outfile.seekp(0, ios::beg);
	outfile.write(reinterpret_cast<char *>(&N3), init_offset);
	outfile.close();

	infile1.close();
	infile2.close();

	vector<T1>().swap(bufferMr1);
	vector<T1>().swap(bufferMr2);
	vector<T1>().swap(bufferMw);

	return N3;
}

template<typename T1, typename T2>
void createDelList(string input_filename1, string input_filename2, string output_filename1, string output_filename2)
{
	int del_cnt = 0;
	int N;
	int N2 = 0;
	vector<T1> bufferMr1(block_size_B);
	vector<T1> bufferMr2(block_size_B);
	vector<T2> bufferMw1(block_size_B);
	vector<T2> bufferMw2(block_size_B);

	int read_blk_size1 = block_size_B * sizeof(T1);
	int read_blk_size2 = block_size_B * sizeof(T1);
	int write_blk_size1 = block_size_B * sizeof(T2);
	int write_blk_size2 = block_size_B * sizeof(T2);

	ifstream infile1(input_filename1, ios::in | ios::binary);
	ifstream infile2(input_filename2, ios::in | ios::binary);
	
	infile1.read(reinterpret_cast<char *>(&N), init_offset);
	infile2.seekg(init_offset, ios::beg);

	ofstream outfile1(output_filename1, ios::out | ios::binary | ios::trunc);
	outfile1.seekp(init_offset, ios::beg);

	ofstream outfile2(output_filename2, ios::out | ios::binary | ios::trunc);
	outfile2.seekp(init_offset, ios::beg);

	int m = ceil((double)N / block_size_B);

	int p_write2 = 0;
	
	for (int i = 0; i < m; ++i)
	{
		if (N - i * block_size_B < block_size_B)
		{
			if (p_write2 > 0 && p_write2 <= bufferMw2.size())
			{
				outfile2.write((char *)&bufferMw2[0], p_write2 * sizeof(T2));
				p_write2 = 0;
			}

			read_blk_size1 = (N - i * block_size_B) * sizeof(T1);
			bufferMr1.resize(read_blk_size1 / sizeof(T1));

			read_blk_size2 = (N - i * block_size_B) * sizeof(T1);
			bufferMr2.resize(read_blk_size2 / sizeof(T1));

			write_blk_size1 = (N - i * block_size_B) * sizeof(T2);
			bufferMw1.resize(write_blk_size1 / sizeof(T2));

			write_blk_size2 = (N - i * block_size_B) * sizeof(T2);
			bufferMw2.resize(write_blk_size2 / sizeof(T2));
		}

		infile1.read((char *)&bufferMr1[0], read_blk_size1);
		infile2.read((char *)&bufferMr2[0], read_blk_size2);

		for (int p_read_1_2 = 0; p_read_1_2 < bufferMr1.size(); ++p_read_1_2)
		{
			if (bufferMr1[p_read_1_2].data[0] == 0 && bufferMr2[p_read_1_2].data[0] == 1)
			{
				del_cnt++;

				bufferMw1[p_read_1_2] = { bufferMr1[p_read_1_2].data[1], bufferMr2[p_read_1_2].data[2] };
				bufferMw2[p_write2] = { bufferMr1[p_read_1_2].data[1], bufferMr1[p_read_1_2].data[2] };

				N2++;
				p_write2++;
				if (p_write2 == bufferMw2.size())
				{
					outfile2.write((char *)&bufferMw2[0], write_blk_size2);
					p_write2 = 0;
				}
			}
			else
			{
				bufferMw1[p_read_1_2] = { bufferMr1[p_read_1_2].data[1], bufferMr1[p_read_1_2].data[2] };
			}
		}
		outfile1.write((char *)&bufferMw1[0], write_blk_size1);
	}

	if (p_write2 > 0 && p_write2 <= bufferMw2.size())
	{
		outfile2.write((char *)&bufferMw2[0], p_write2 * sizeof(T2));
	}

	vector<T1>().swap(bufferMr1);
	vector<T1>().swap(bufferMr2);
	vector<T2>().swap(bufferMw1);
	vector<T2>().swap(bufferMw2);

	outfile1.seekp(0, ios::beg);
	outfile2.seekp(0, ios::beg);

	outfile1.write((char *)&N, init_offset);
	outfile2.write((char *)&N2, init_offset);

	infile1.close();
	infile2.close();
	outfile1.close();
	outfile2.close();
}

// sort by 1
template <typename T1, typename T2>
void RandAi(string input_filename, string output_filename)
{
	int N;

	ifstream infile(input_filename, ios::in | ios::binary);
	ofstream outfile(output_filename, ios::out | ios::binary | ios::trunc);

	infile.read((char *)&N, init_offset);

	outfile.write((char *)&N, init_offset);

	vector<T1> bufferMr(block_size_M);
	vector<T2> bufferMw(block_size_M);

	int read_blk_size = block_size_M * sizeof(T1);
	int write_blk_size = block_size_M * sizeof(T2);

	int m = ceil((double)N / block_size_M);

	for (int i = 0; i < m; i++)
	{
		if (N - i * block_size_M < block_size_M)
		{
			read_blk_size = (N - i * block_size_M) * sizeof(T1);
			bufferMr.resize(read_blk_size / sizeof(T1));
		}

		if (N - i * block_size_M < block_size_M)
		{
			write_blk_size = (N - i * block_size_M) * sizeof(T2);
			bufferMw.resize(write_blk_size / sizeof(T2));
		}

		infile.read((char *)&bufferMr[0], read_blk_size);

		srand(time(NULL));
		for (int j = 0; j < bufferMw.size(); j++)
		{
			bufferMw[j] = { (i == 0 && j == 0) ? 1 : rand() % 2, bufferMr[j].data[0], bufferMr[j].data[1] };
		}
		outfile.write((char *)&bufferMw[0], write_blk_size);
	}

	vector<T1>().swap(bufferMr);
	vector<T2>().swap(bufferMw);

	infile.close();
	outfile.close();
}

template<typename T>
void ExternalSort(string input_filename, string output_filename, int coordinate)
{
	int N;

	ifstream infile(input_filename, ios::in | ios::binary);
	ofstream tempfile("temp1.bin", ios::out | ios::binary | ofstream::trunc);

	infile.read((char *)&N, init_offset);
	tempfile.write((char *)&N, init_offset);

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

		sort(bufferM.begin(), bufferM.end(), [coordinate](T const &a, T const &b) -> bool
		{ return a.data[coordinate] < b.data[coordinate]; });
		tempfile.write((char *)&bufferM[0], read_size_M);
	}

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
			}

			outfile1.write((char *)&N, init_offset);

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

		levels % 2 == 0 ? rename("temp1.bin", output_filename.c_str()) : rename("temp2.bin", output_filename.c_str());
		levels % 2 == 0 ? remove("temp2.bin") : remove("temp1.bin");
	}
	else
	{
		rename("temp1.bin", output_filename.c_str());
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
			bufferBw[po++] = bufferBr1[p1++];

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
			bufferBw[po++] = bufferBr2[p2++];

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
			bufferBw[po++] = bufferBr2[i];
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
			bufferBw[po++] = bufferBr1[i];
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

	vector<T>().swap(bufferBr1);
	vector<T>().swap(bufferBr2);
	vector<T>().swap(bufferBw);
}
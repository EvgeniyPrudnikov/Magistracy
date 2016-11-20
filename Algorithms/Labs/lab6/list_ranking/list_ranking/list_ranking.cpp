
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <map> 


using namespace std;
//M = 128 * 256
// M % B == 0 !!!!! 

const int block_size_M = 6;
const int block_size_B = 2;
const int init_offset = sizeof(int);

struct two
{
    int data[2];
	
	bool operator==(const two &rhs ) const
    {
        return data[0] == rhs.data[0] && data[1] == rhs.data[1];
    }
};

struct three
{
    int data[3];
};


void CreateTestFile();

void CreateTestFile_PROD();

template <typename T1, typename T2, typename T3>
void JoinInsert(char * input_filename1, char * input_filename2, char * output_filename);


void Rank(char * input_filename, char * output_filename);

template<typename T1, typename T2, typename T3>
int JoinDel(char *input_filename1, char *input_filename2, char *output_filename);

template<typename T1, typename T2, typename T3>
void createDelList(char *input_filename, char *output_filename1, char *output_filename2);

template<typename T1, typename T2, typename T3>
void Join3(char *input_filename1, char *input_filename2, char *output_filename);

template<typename T>
void ExternalSort(char *input_filename, char *output_filename, int coordinate);

template<typename T>
void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, int run1_start, int run2_start, int run2_end, int coordinate);

template<typename T>
void coutFile(char *filename);



int main(int argc, char *argv[])
{

    const auto startTime = std::clock();

    //CreateTestFile_PROD();
    CreateTestFile();


    ExternalSort<two>("input.bin", "output1.bin", 1);

    ExternalSort<two>("input.bin", "output0.bin", 0);

   // coutFile<two>("output0.bin");

    Join3<two, two, three>("output1.bin", "output0.bin", "join1.bin");

   // coutFile<three>("join1.bin");

    createDelList<three, two, two>("join1.bin", "input1.bin", "delList.bin");

	//cout << endl<< "input1.bin:" << endl;
    //coutFile<two>("input1.bin");
	//coutFile<two>("delList.bin");

    ExternalSort<two>("input1.bin", "input1_s.bin", 0);

    JoinDel<two, two, two>("input1_s.bin", "delList.bin", "input2.bin");

    //coutFile<two>("input2.bin");

	Rank("input2.bin", "input3.bin");

	cout << endl << "ranked:" << endl;

	coutFile<two>("input3.bin");

	ExternalSort<two>("input3.bin", "input4.bin", 1);

	coutFile<two>("input4.bin");

	ExternalSort<two>("delList.bin", "s_delList.bin", 0);

	coutFile<two>("s_delList.bin");
	
	JoinInsert<two, two, two>("input4.bin","s_delList.bin","insert1.bin");

	ExternalSort<two>("insert1.bin", "insert2.bin", 0);

	coutFile<two>("insert2.bin");

    const auto endTime = clock();
    cout << endl << "done in  " << setprecision(6) << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
    getchar();

    return 0;
}



template <typename T1 , typename T2 , typename T3>
void JoinInsert(char * input_filename1, char * input_filename2, char * output_filename)
{
	int N1;
	int N2;
	int N3 = 0;
	int	inserted_elems = 0;

	ifstream infile1(input_filename1, ios::in | ios::binary);
	ifstream infile2(input_filename2, ios::in | ios::binary);

	infile1.read(reinterpret_cast<char *>(&N1), init_offset);
	infile2.read(reinterpret_cast<char *>(&N2), init_offset);

	
	ofstream outfile(output_filename, ios::out | ios::binary);
	outfile.seekp(init_offset, ios::beg);
	

	int block_size_file1 = min(N1, block_size_M);
	int block_size_file2 = min(N2, block_size_M);

	vector<T1> bufferMr1(block_size_file1);
	vector<T2> bufferMr2(block_size_file2);
	vector<T3> bufferMw(block_size_M);

	int read_blk_size1 = block_size_file1 * sizeof(T1);
	int read_blk_size2 = block_size_file2 * sizeof(T2);
	int write_blk_size = block_size_M * sizeof(T3);

	int m = ceil(static_cast<double>(max(N1, N2)) / block_size_M);
	int p_read1 = 0;
	int p_read2 = 0;
	int p_write = 0;

	for (int i = 0; i < m; i++)
	{

		if (N1 - i * block_size_file1 < block_size_file1)
		{
			read_blk_size1 = (N1 - i * block_size_file1) * sizeof(T1);
			bufferMr1.resize(read_blk_size1 / sizeof(T1));
		}

		if (N1 - i * block_size_M < block_size_M)
		{
			if (p_write > 0 && p_write <= bufferMw.size())
			{
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T3));
				p_write = 0;
			}

			write_blk_size = (N1 - i * block_size_M) * sizeof(T3);
			bufferMw.resize(write_blk_size / sizeof(T3));
		}
		infile1.read((char *)&bufferMr1[0], read_blk_size1);

		if (i == 0)
		{
			if (N2 - i * block_size_file2 < block_size_file2 && N2 - i * block_size_file2 > 0)
			{
				read_blk_size2 = (N2 - i * block_size_file2) * sizeof(T2);
				bufferMr2.resize(read_blk_size2 / sizeof(T2));
			}

			infile2.read((char *)&bufferMr2[0], read_blk_size2);
		}

		if (inserted_elems < N2)
		{
			for (p_read1 = 0; p_read1 < bufferMr1.size(); p_read1++)
			{

				if (p_write == bufferMw.size())
				{
					outfile.write((char *)& bufferMw[0], write_blk_size);
					p_write = 0;
				}

				if (bufferMr1[p_read1].data[1] == bufferMr2[p_read2].data[0])
				{
					N3 += 2;
					bufferMw[p_write++] = { 2 * bufferMr1[p_read1].data[0], bufferMr1[p_read1].data[1] };
					bufferMw[p_write++] = { (2 * bufferMr1[p_read1].data[0]) + 1, bufferMr2[p_read2].data[1] };

					inserted_elems++;
					if (inserted_elems == N2) continue;
					p_read2++;

					if (p_read2 == bufferMr2.size())
					{
						if (N2 - i * block_size_M < block_size_M && i > 0 && N2 - i * block_size_M > 0)
						{
							read_blk_size2 = (N2 - i * block_size_M) * sizeof(T2);
							bufferMr2.resize(read_blk_size2 / sizeof(T2));
						}

						infile2.read((char *)&bufferMr2[0], read_blk_size2);
						p_read2 = 0;
					}

				}
				else
				{
					N3++;
					bufferMw[p_write++] = { 2 * bufferMr1[p_read1].data[0] , bufferMr1[p_read1].data[1] };
				}
			}
		}
		else
		{
			if (p_write > 0 && p_write <= bufferMw.size())
			{
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T3));
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
		outfile.write((char *)&bufferMw[0], p_write * sizeof(T3));
	}

	cout << endl << "inserted_elems " << inserted_elems << endl;

	bufferMr1.clear();
	bufferMr2.clear();
	bufferMw.clear();
	vector<T1>().swap(bufferMr1);
	vector<T2>().swap(bufferMr2);
	vector<T3>().swap(bufferMw);
	cout << endl << "n3 = " << N3 << endl;

	outfile.seekp(0, ios::beg);
	outfile.write(reinterpret_cast<char *>(&N3), init_offset);

}




void Rank(char * input_filename, char * output_filename)
{

	int N;

	ifstream infile(input_filename, ios::in | ios::binary);
	infile.read(reinterpret_cast<char *>(&N), init_offset);

	vector<two> bufferMr(N);
	vector<two> bufferMw;
	map<int, int> map_buffer;


	infile.read((char*)&bufferMr[0], N * sizeof(two));

	ofstream outfile(output_filename, ios::out | ios::binary);
	outfile.write(reinterpret_cast<char*>(&(N)), init_offset);


	int val_next = 0;
	int key_next = 0;
	int j = 1;

	val_next = bufferMr[0].data[0];
	bufferMw.push_back({ j++, val_next });
	

	val_next = bufferMr[0].data[1];
	key_next = val_next;
	bufferMw.push_back({ j++,val_next });
	

	for (auto const& x : bufferMr)
	{
		map_buffer.emplace(move(x.data[0]), move(x.data[1]));
	}

	while (j <= N )
	{
		val_next = map_buffer[key_next];
		bufferMw.push_back({ j++,val_next });
		key_next = val_next;
	}

	outfile.write((char *)& bufferMw[0], N * sizeof(two));

	bufferMr.clear();
	bufferMw.clear();

	vector<two>().swap(bufferMr);
	vector<two>().swap(bufferMw);
}

template<typename T1, typename T2, typename T3>
int JoinDel(char *input_filename1, char *input_filename2, char *output_filename)
{
	int N1;
	int N2;
	int N3 = 0;
	int deleted_elems = 0;

	ifstream infile1(input_filename1, ios::in | ios::binary);
	ifstream infile2(input_filename2, ios::in | ios::binary);

	infile1.read(reinterpret_cast<char *>(&N1), init_offset);
	infile2.read(reinterpret_cast<char *>(&N2), init_offset);

	ofstream outfile(output_filename, ios::out | ios::binary);
	outfile.seekp(init_offset, ios::beg);

	vector<T1> bufferMr1(block_size_M);
	vector<T2> bufferMr2(block_size_M);
	vector<T3> bufferMw(block_size_M);

	int read_blk_size1 = block_size_M * sizeof(T1);
	int read_blk_size2 = block_size_M * sizeof(T2);
	int write_blk_size = block_size_M * sizeof(T3);

	int m = ceil(static_cast<double>(max(N1,N2)) / block_size_M);
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
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T3));
				p_write = 0;
			}

			write_blk_size = (N1 - i * block_size_M) * sizeof(T3);
			bufferMw.resize(write_blk_size / sizeof(T3));
		}
		infile1.read((char *)&bufferMr1[0], read_blk_size1);

		if (i == 0)
		{
			if (N2 - i * block_size_M < block_size_M && N2 - i * block_size_M > 0)
			{
				read_blk_size2 = (N2 - i * block_size_M) * sizeof(T2);
				bufferMr2.resize(read_blk_size2 / sizeof(T2));
			}

			infile2.read((char *)&bufferMr2[0], read_blk_size2);
		}


		if (deleted_elems < N2)
		{
			for (p_read1 = 0; p_read1 < bufferMr1.size(); ++p_read1)
			{
				if (bufferMr1[p_read1].data[0] != bufferMr2[p_read2].data[1]  )
				{
					N3++;
					bufferMw[p_write] = bufferMr1[p_read1];
					p_write++;

					if (p_write == bufferMw.size())
					{
						outfile.write((char *)&bufferMw[0], write_blk_size);
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
							read_blk_size2 = (N2 - i * block_size_M) * sizeof(T2);
							bufferMr2.resize(read_blk_size2 / sizeof(T2));
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
				outfile.write((char *)&bufferMw[0], p_write * sizeof(T3));
				N3 += p_write;
				p_write = 0;
			}
			outfile.write((char *)&bufferMr1[0], read_blk_size1);
			N3 += (int)bufferMr1.size();
		}
	}

	if (p_write > 0 && p_write <= bufferMw.size())
	{
		outfile.write((char *)&bufferMw[0], p_write * sizeof(T3));
	}


	outfile.seekp(0, ios::beg);
	outfile.write(reinterpret_cast<char *>(&N3), init_offset);

	bufferMr1.clear();
	bufferMr2.clear();
	bufferMw.clear();
	vector<T1>().swap(bufferMr1);
	vector<T2>().swap(bufferMr2);
	vector<T3>().swap(bufferMw);
	cout << endl << "n3 = " << N3 << endl;

	return N3;
}

template<typename T1, typename T2, typename T3>
void createDelList(char *input_filename, char *output_filename1, char *output_filename2)
{
	int del_cnt = 0;
    int N;
    int cnt_f2 = 0;
    vector<T1> bufferMr(block_size_M);
    vector<T2> bufferMw1(block_size_M);
    vector<T3> bufferMw2(block_size_M);

    int read_blk_size = block_size_M * sizeof(T1);
    int write_blk_size1 = block_size_M * sizeof(T2);
    int write_blk_size2 = block_size_M * sizeof(T3);

    ifstream infile(input_filename, ios::in | ios::binary);
    infile.read(reinterpret_cast<char *>(&N), init_offset);

    ofstream outfile1(output_filename1, ios::out | ios::binary);
    outfile1.seekp(init_offset, ios::beg);

    ofstream outfile2(output_filename2, ios::out | ios::binary);
    outfile2.seekp(init_offset, ios::beg);

    int m = ceil((double) N / block_size_M);
    int curr_rand;
	int prev_rand = 0;
    int k = 0;
	for (int i = 0; i < m; ++i)
	{

		if (N - i * block_size_M < block_size_M)
		{

			if (k > 0 && k <= bufferMw2.size())
			{
				outfile2.write((char *)&bufferMw2[0], k * sizeof(T3));
				k = 0;
			}

			read_blk_size = (N - i * block_size_M) * sizeof(T1);
			bufferMr.resize(read_blk_size / sizeof(T1));

			write_blk_size1 = (N - i * block_size_M) * sizeof(T2);
			bufferMw1.resize(write_blk_size1 / sizeof(T2));

			write_blk_size2 = (N - i * block_size_M) * sizeof(T3);
			bufferMw2.resize(write_blk_size2 / sizeof(T3));
		}

		infile.read((char *)&bufferMr[0], read_blk_size);


		srand(time(nullptr));
		for (int j = 0; j < bufferMr.size(); ++j)
		{

			curr_rand = rand() % 2;

			//if (prev_rand == 0 && curr_rand == 1)
			if (bufferMr[j].data[1] == 2 || bufferMr[j].data[1] == 7 || bufferMr[j].data[1] == 10)
			{
				del_cnt++;
				bufferMw1[j] = { bufferMr[j].data[0], bufferMr[j].data[2] };
				bufferMw2[k] = { bufferMr[j].data[0], bufferMr[j].data[1] };
				cnt_f2++;
				k++;
				if (k == bufferMw2.size())
				{
					outfile2.write((char *)&bufferMw2[0], write_blk_size2);
					k = 0;
				}
			}
			else
			{
				bufferMw1[j] = { bufferMr[j].data[0], bufferMr[j].data[1] };

			}
			prev_rand = curr_rand;
		}
		outfile1.write((char *)&bufferMw1[0], write_blk_size1);
	}

    if (k > 0 && k <= bufferMw2.size())
    {
        outfile2.write((char *) &bufferMw2[0], k * sizeof(T3));
    }

    bufferMr.clear();
    bufferMw1.clear();
    bufferMw2.clear();
    vector<T1>().swap(bufferMr);
    vector<T2>().swap(bufferMw1);
    vector<T3>().swap(bufferMw2);

    outfile1.seekp(0, ios::beg);
    outfile2.seekp(0, ios::beg);

    outfile1.write(reinterpret_cast<char *>(&N), init_offset);
    outfile2.write(reinterpret_cast<char *>(&cnt_f2), init_offset);

	cout << endl << "del_cnt " << del_cnt << endl;
}

template<typename T1, typename T2, typename T3>
void Join3(char *input_filename1, char *input_filename2, char *output_filename)
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

    vector<T1> bufferMr1(block_size_M);
    vector<T2> bufferMr2(block_size_M);
    vector<T3> bufferMw(block_size_M);

    int read_blk_size1 = block_size_M * sizeof(T1);
    int read_blk_size2 = block_size_M * sizeof(T2);
    int write_blk_size = block_size_M * sizeof(T3);

    int m = ceil((double) N1 / block_size_M);

    for (int i = 0; i < m; i++)
    {

        if (abs(N1 - i * block_size_M) < block_size_M)
        {
            read_blk_size1 = (N1 - i * block_size_M) * sizeof(T1);
            bufferMr1.resize(read_blk_size1 / sizeof(T1));

        }
        if (abs(N2 - i * block_size_M) < block_size_M)
        {
            read_blk_size2 = (N2 - i * block_size_M) * sizeof(T1);
            bufferMr2.resize(read_blk_size2 / sizeof(T2));
        }

        if (abs(N3 - i * block_size_M) < block_size_M)
        {
            write_blk_size = (N3 - i * block_size_M) * sizeof(T3);
            bufferMw.resize(write_blk_size / sizeof(T3));
        }


        infile1.read((char *) &bufferMr1[0], read_blk_size1);
        infile2.read((char *) &bufferMr2[0], read_blk_size2);


        for (int j = 0; j < bufferMw.size(); j++)
        {
            bufferMw[j] = {bufferMr1[j].data[0], bufferMr1[j].data[1], bufferMr2[j].data[1]};
        }

        outfile.write((char *) &bufferMw[0], write_blk_size);

    }

    bufferMr1.clear();
    bufferMr2.clear();
    bufferMw.clear();

    vector<T1>().swap(bufferMr1);
    vector<T2>().swap(bufferMr2);
    vector<T3>().swap(bufferMw);

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

    cout << "N = " << b << endl;

    vector<T> buffer(b);
    infile.read((char *) &buffer[0], buffer.size() * sizeof(T));

    int l = sizeof(T) / sizeof(int);

    for (int i = 0; i < b; ++i)
    {
        for (int j = 0; j < l; ++j)
        {
            cout << setiosflags(ios::fixed | ios::left) <<setprecision(0)<< buffer[i].data[j] << " ";
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

void CreateTestFile_PROD()
{
    int n = 1250001;
    srand(static_cast<unsigned int>(time(NULL)));

    vector<int> array(2 * n);//{6, 7, 7, 1, 1, 3, 3, 2, 2, 8, 8, 5, 5, 4, 4, 10, 10, 9, 9, 6};

    int c = 2;
    array[0] = 1;
    for (int i = 1; i < n * 2 - 4; ++i)
    {
        array[i] = c;
        array[i + 1] = c;
        i++;
        c++;
    }

    array[n * 2 - 4] = c-1;
    array[n * 2 - 3] = c;
	array[n * 2 - 2] = c;
	array[n * 2 - 1] =1;

    ofstream out("input.bin", ios::out | ios::binary | ios::trunc);

    out.write((char *) &n, sizeof(int));

    out.write((char *) &array[0], n * 2 * sizeof(int));

    out.close();
    vector<int>().swap(array);
}





#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator> 

using namespace std;

const int block_size_B = 2;
const int init_offset = sizeof(int);

void CreateTestFile();
void coutFile_INT(string filename);
int getNumber(int a, int b, int & c);
void reverseResult(char* input_filename, char* output_filename);

int main()
{

	CreateTestFile();

	int n1;
	int n2;
	int n3 = 0;

	ifstream infile1("input.bin", ios::in | ios::binary);
	ifstream infile2("input.bin", ios::in | ios::binary);

	ofstream outfile("temp1.bin", ios::out | ios::binary | ios::trunc);

	infile1.read((char *)&n1, init_offset);
	infile1.seekg(n1 * sizeof(int), ios::cur);

	infile2.seekg(n1 * sizeof(int) + init_offset, ios::beg);
	infile2.read((char *)&n2, init_offset);
	infile2.seekg(n2 * sizeof(int), ios::cur);


	vector<int> bufferRA(block_size_B);
	vector<int> bufferRB(block_size_B);
	vector<int> bufferWC(block_size_B);

	int read_blk_size1 = block_size_B * sizeof(int);
	int read_blk_size2 = block_size_B * sizeof(int);
	int write_blk_size = block_size_B * sizeof(int);
	
	int read_offset1 = init_offset;
	int read_offset2 = n1 * sizeof(int) + 2 * init_offset;
	
	int p_write = 0, p_read1 = 0, p_read2 = 0;

	if (n1 < block_size_B)
	{
		read_blk_size1 = (n1) * sizeof(int);
		bufferRA.resize((n1));
	}
	if (n2 < block_size_B)
	{
		read_blk_size2 = (n2) * sizeof(int);
		bufferRB.resize((n2));
	}

	infile1.seekg(-read_blk_size1, ios::cur);
	infile1.read((char *)&bufferRA[0], read_blk_size1);

	infile2.seekg(-read_blk_size2, ios::cur);
	infile2.read((char *)&bufferRB[0], read_blk_size2);

	reverse(bufferRA.begin(), bufferRA.end());
	reverse(bufferRB.begin(), bufferRB.end());


	int number1_end = n1 * sizeof(int) + init_offset;
	int number2_end = (n2 + n1) * sizeof(int) + 2 * init_offset;


	int c = 0;

	while (read_offset1 < number1_end && read_offset2 < number2_end)
	{

		if (bufferRA[p_read1] + bufferRB[p_read2] + c >= 10)
		{
			bufferWC[p_write++] = (bufferRA[p_read1++] + bufferRB[p_read2++] + c) % 10;
			c = 1;

			if (p_read1 > bufferRA.size() - 1)
			{
				read_offset1 += read_blk_size1;

				if (read_offset1 < number1_end)
				{

					infile1.seekg(-read_blk_size1, ios::cur);

					if (number1_end - read_offset1 < read_blk_size1)
					{
						read_blk_size1 = (number1_end - read_offset1);
						bufferRA.resize((number1_end - read_offset1) / sizeof(int));
					}

					infile1.seekg(-read_blk_size1, ios::cur);
					infile1.read((char *)&bufferRA[0], read_blk_size1);
					reverse(bufferRA.begin(), bufferRA.end());
					p_read1 = 0;
				}
			}

			if (p_read2 > bufferRB.size() - 1)
			{
				read_offset2 += read_blk_size2;

				if (read_offset2 < number2_end)
				{
					infile2.seekg(-read_blk_size2, ios::cur);

					if (number2_end - read_offset2 < read_blk_size2)
					{
						read_blk_size2 = (number2_end - read_offset2);
						bufferRB.resize((number2_end - read_offset2) / sizeof(int));
					}

					infile2.seekg(-read_blk_size2, ios::cur);
					infile2.read((char *)&bufferRB[0], read_blk_size2);
					reverse(bufferRB.begin(), bufferRB.end());
					p_read2 = 0;
				}
			}
		}
		else
		{
			bufferWC[p_write++] = bufferRA[p_read1++] + bufferRB[p_read2++] + c;
			c = 0;

			if (p_read1 > bufferRA.size() - 1)
			{
				read_offset1 += read_blk_size1;

				if (read_offset1 < number1_end)
				{

					infile1.seekg(-read_blk_size1, ios::cur);

					if (number1_end - read_offset1 < read_blk_size1)
					{
						read_blk_size1 = (number1_end - read_offset1);
						bufferRA.resize((number1_end - read_offset1) / sizeof(int));
					}

					infile1.seekg(-read_blk_size1, ios::cur);

					infile1.read((char *)&bufferRA[0], read_blk_size1);
					reverse(bufferRA.begin(), bufferRA.end());
					p_read1 = 0;
				}
			}

			if (p_read2 > bufferRB.size() - 1)
			{
				read_offset2 += read_blk_size2;

				if (read_offset2 < number2_end)
				{

					infile2.seekg(-read_blk_size2, ios::cur);

					if (number2_end - read_offset2 < read_blk_size2)
					{
						read_blk_size2 = (number2_end - read_offset2);
						bufferRB.resize((number2_end - read_offset2) / sizeof(int));
					}
					infile2.seekg(-read_blk_size2, ios::cur);
					infile2.read((char *)&bufferRB[0], read_blk_size2);
					reverse(bufferRB.begin(), bufferRB.end());
					p_read2 = 0;
				}
			}
		}

		if (p_write > bufferWC.size() - 1)
		{
			outfile.write((char *)&bufferWC[0], write_blk_size);
			n3 += bufferWC.size();
			p_write = 0;
		}
	}

	if (read_offset1 == number1_end)
	{
		int i;
		for (i = p_read2; i < bufferRB.size(); ++i)
		{
			bufferWC[p_write++] = getNumber(bufferRB[i], 0, c);
			if (p_write > bufferRB.size() - 1) break;
		}
		
		outfile.write((char *)&bufferWC[0], p_write * sizeof(int));
		n3 += p_write;
		p_write = 0;

		for (int j = read_offset2 + read_blk_size2; j < number2_end; j += read_blk_size2)
		{
			infile2.seekg(-read_blk_size2, ios::cur);
			if (number2_end - j < read_blk_size2)
			{
				read_blk_size2 = number2_end - j;
				bufferRB.resize((number2_end - j) / sizeof(int));
				bufferWC.resize((number2_end - j) / sizeof(int));
			}
			infile2.seekg(-read_blk_size2, ios::cur);
			infile2.read((char *)&bufferRB[0], read_blk_size2);
			reverse(bufferRB.begin(), bufferRB.end());

			for (int p = 0; p < bufferRB.size(); p++)
			{
				bufferWC[p_write++] = getNumber(bufferRB[p], 0, c);
			}

			outfile.write((char *)&bufferWC[0], bufferWC.size() * sizeof(int));
			n3 += bufferWC.size();
			p_write = 0;
		}

		if (c == 1) 
		{
			outfile.write((char*)&c, sizeof(int));
			n3++;
		}

	}
	else if (read_offset2 == number2_end)
	{
		int i;
		for (i = p_read1; i < bufferRA.size(); ++i)
		{
			bufferWC[p_write++] = getNumber(bufferRA[i], 0, c);
			if (p_write > bufferRA.size() - 1) break;
		}
		

		outfile.write((char *)&bufferWC[0], p_write * sizeof(int));
		n3 += p_write;
		p_write = 0;


		for (int j = read_offset1 + read_blk_size1; j < number1_end; j += read_blk_size1)
		{
			infile1.seekg(-read_blk_size1, ios::cur);
			if (number1_end - j < read_blk_size1)
			{
				read_blk_size1 = number1_end - j;
				bufferRA.resize((number1_end - j) / sizeof(int));
				bufferWC.resize((number1_end - j) / sizeof(int));
			}
			infile1.seekg(-read_blk_size1, ios::cur);
			infile1.read((char *)&bufferRA[0], read_blk_size1);
			reverse(bufferRA.begin(), bufferRA.end());

			for (int p = 0; p < bufferRA.size(); p++)
			{
				bufferWC[p_write++] = getNumber(bufferRA[p], 0, c);
			}

			outfile.write((char *)&bufferWC[0], bufferWC.size()*sizeof(int));
			n3 += bufferWC.size();
			p_write = 0;
		}

		if (c == 1)
		{
			outfile.write((char*)&c, sizeof(int));
			n3++;
		}
	}

	vector<int>().swap(bufferRA);
	vector<int>().swap(bufferRB);
	vector<int>().swap(bufferWC);

	outfile.write((char *)&n3, init_offset);

	infile1.close();
	infile2.close();
	outfile.close();

	reverseResult("temp1.bin", "output.bin");

	coutFile_INT("output.bin");

	getchar();
	return 0;
}


void reverseResult(char* input_filename, char* output_filename)
{

	int new_blk_size = 2 * block_size_B;

	ifstream infile(input_filename, ios::in | ios::binary);
	ofstream outfile(output_filename, ios::out | ios::binary | ios::trunc);

	infile.seekg(0, infile.end);
	int N = infile.tellg() / sizeof(int);

	int read_blk_size = new_blk_size * sizeof(int);

	vector<int> bufferR(new_blk_size);

	if (N < new_blk_size)
	{
		read_blk_size = N * sizeof(int);
		bufferR.resize(N);
	}

	int m = ceil((double)N / (new_blk_size));

	for (int i = 0; i < m; i++)
	{
		
		if (N - i * new_blk_size < new_blk_size)
		{
			read_blk_size = (N - i * new_blk_size) * sizeof(int);
			bufferR.resize(N - i * new_blk_size);
		}

		infile.seekg(-read_blk_size, ios::cur);

		infile.read((char *)&bufferR[0], read_blk_size);

		reverse(bufferR.begin(), bufferR.end());

		outfile.write((char *)&bufferR[0], read_blk_size);
		infile.seekg(-read_blk_size, ios::cur);

	}

	vector<int>().swap(bufferR);

	infile.close();
	outfile.close();
}

int getNumber(int a, int b, int & c)
{
	int result;

	if (a + b + c >= 10)
	{
		result = (a + b + c) % 10;
		c = 1;
	}
	else
	{
		result = a + b + c;
		c = 0;
	}
	return result;
}


void coutFile_INT(string filename)
{
	cout << endl << endl;
	ifstream infile(filename, ios::in | ios::binary);
	int M;
	infile.read((char*)&M, init_offset);

	vector<int> buffer(M);
	infile.read((char *)&buffer[0], buffer.size() * sizeof(int));


	for (int i = 0; i < M; ++i)
	{
		cout << setiosflags(ios::fixed | ios::left) << setprecision(0) << buffer[i] << "";
	}
	infile.close();

	vector<int>().swap(buffer);
}


void CreateTestFile()
{

	int n1 = 9;
	int n2 = 1;

	srand(static_cast<unsigned int>(time(NULL)));
	//vector<int> array1(n1);
	//vector<int> array2(n2);

	vector<int> array1{9,9,9,9,9,9,9,9,9};
	vector<int> array2{2};

	//for (int i = 0; i < n1; i++)
	//{
	//	array1[i] = rand() % 9 + 1;
	//}

	//for (int i = 0; i < n2; i++)
	//{
	//	array2[i] = rand() % 9 + 1;
	//}

	for (int i = 0; i < n1; i++)
	{
		cout<< array1[i]<<"";
	}
	cout << endl;
	for (int i = 0; i < n2; i++)
	{
		cout<<array2[i]<< "";
	}
	cout << endl;
	ofstream out("input.bin", ios::out | ios::binary | ios::trunc);

	out.write((char *)&n1, sizeof(int));

	out.write((char *)&array1[0], n1 * sizeof(int));

	out.write((char *)&n2, sizeof(int));

	out.write((char *)&array2[0], n2 * sizeof(int));

	stringstream ss1;
	copy(array1.begin(), array1.end(), ostream_iterator<int>(ss1, ""));
	string str1 = ss1.str();


	stringstream ss2;
	copy(array2.begin(), array2.end(), ostream_iterator<int>(ss2, ""));
	string str2 = ss2.str();


	long long a = std::atoi(str1.c_str());
	long long b = std::atoi(str2.c_str());
	long long c = a + b;
	cout << endl << c << endl;
	out.close();
	vector<int>().swap(array1);
	vector<int>().swap(array2);
}


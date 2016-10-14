#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

int main()
{
	const int n = 4096;
	const int m = 4096;
	srand((unsigned int) time(NULL));


	//char fnum[14] = {3, 4, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0};

	char matrix[n*m];

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i*n + j] = rand() % 255;
		}
	}

	ofstream out("numbers", ios::out | ios::binary);

	out.write((char *) &n, sizeof n);
	out.write((char *) &m, sizeof m);
	out.write((char *) &matrix, sizeof matrix);

	out.close();

	return 0;
}
// cache_miss.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>
#include <random>
#include <ctime>


using namespace std;



const long cacheSize = 3145728, ways = 12, cacheLineSize = 64;
long numberOfSets = cacheSize / (ways * cacheLineSize);


// initialize cache, lru
vector<vector<long>> cache(numberOfSets, vector<long>(ways, -10));
vector<vector<long>> LRU(numberOfSets, vector<long>(ways, -10));

long check = 0, hit = 0;


//Function to change the tag order in the LRU algorithm
void InitializeLRU(vector<vector<long>> & lru)
{
	for (int i = 0; i < lru.size(); i++)
		for (int j = 0; j < lru[i].size(); j++)
			lru[i][j] = j;
}

void BringToTop(vector<vector<long>> & lru, long set, long ways, long x)
{
	long i, pos;
	for (i = 0; i < ways; i++)
		if (lru[set][i] == x)
			pos = i;
	for (i = pos; i < ways - 1; i++)
		lru[set][i] = lru[set][i + 1];
	lru[set][ways - 1] = x;
}

template <typename T>
void AnalyzeAddress(T &variable, vector<vector<long>> &cache, vector<vector<long>> &LRU, int cacheLineSize, long &check, long &hit, int algorithm)
{

	long address = (long) &variable;
	long randomLine = 0;

	long numberOfSets = cache.size();
	long ways = cache[0].size();

	long set, tag, found, pos;
	set = (address / cacheLineSize) % numberOfSets;
	tag = address / (cacheLineSize * numberOfSets);

	long i;
	check++;
	found = 0;
	for (i = 0; i < ways; i++)
		if (cache[set][i] == tag)
		{
			found = 1;
			pos = i;
			break;
		}
	if (found)
	{
		hit++;
		BringToTop(LRU, set, ways, pos);
	}
	else
	{
		if (algorithm == 0)
		{
			i = LRU[set][0];
			cache[set][i] = tag;
			BringToTop(LRU, set, ways, i);
		}
		else
		{
			randomLine = rand() % ways;
			cache[set][randomLine] = tag;
		}
	}
}

void MultSimple(const float* __restrict a, const float* __restrict b, float* __restrict c, int n , int algorithm)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			c[i * n + j] = 0.f;
			for (int k = 0; k < n; ++k)
			{
				// analyze variables address
				AnalyzeAddress(a[i * n + k], cache, LRU, cacheLineSize, check, hit, algorithm);
				AnalyzeAddress(b[k * n + j], cache, LRU, cacheLineSize, check, hit, algorithm);
				AnalyzeAddress(c[i * n + j], cache, LRU, cacheLineSize, check, hit, algorithm);

				c[i * n + j] += a[i * n + k] * b[k * n + j];
			}
		}
	}
}

void Transpose(const float* __restrict a, float* __restrict b, int n , int algorithm)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			AnalyzeAddress(a[j * n + i], cache, LRU, cacheLineSize, check, hit, algorithm);
			AnalyzeAddress(b[i * n + j], cache, LRU, cacheLineSize, check, hit, algorithm);

			b[i * n + j] = a[j * n + i];
		}
	}
}

void FillRandom(float* a, int n)
{

	std::default_random_engine eng;
	std::uniform_real_distribution<float> dist;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			a[i * n + j] = dist(eng);
		}
	}
}


int main(int argc, char* argv[])
{
	InitializeLRU(LRU);

	const int n = atoi(argv[1]);
	cout << "n = " << n << endl;

	const int algorithm = atoi(argv[2]); // 0 - LRU , 1 - random
	cout << "algorithm: " << (algorithm == 0 ? "LRU" : "random") << endl;

	const int task = atoi(argv[3]); // 0 - Multiply , 1 - Transpose
	cout << "task: " << (task == 0 ? "Multiply" : "Transpose") << endl;

	float *a = new float[n * n];
	float *b = new float[n * n];
	float *c = new float[n * n];

	FillRandom(a, n);
	FillRandom(b, n);

	if (task == 0)
	{
		const auto startTime = clock();
		MultSimple(a, b, c, n, algorithm);
		const auto endTime = clock();
		cout << "timeSimple: " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	}
	else
	{
		const auto startTime = clock();
		Transpose(a, b, n, algorithm);
		const auto endTime = clock();
		cout << "timeTranspose: " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	}

	cout << "# of checks: " << check;
	cout << " # of hits: " << hit;
	cout << " # of misses: " << check - hit;
	cout << " Miss Rate: " << float(check - hit) / float(check);

	//getchar();
	return 0;
}



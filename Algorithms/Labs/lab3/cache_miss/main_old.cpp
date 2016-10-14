#include <iostream>
#include <random>
#include <ctime>

using namespace std;

//__declspec(align(16))
// __restrict
namespace {
    void MultSimple(const float* __restrict a, const float* __restrict b, float* __restrict c, int n)
    {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                c[i * n + j] = 0.f;
                for (int k = 0; k < n; ++k) {
                    c[i * n + j] += a[i * n + k] * b[k * n + j];
                }
            }
        }
    }

   void Transpose(const float* __restrict a, float* __restrict b, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
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
                a[j * n + i] = dist(eng);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    const int n = atoi(argv[1]);
    std::cerr << "n = " << n << std::endl;
	
	const int task = atoi(argv[2]); // 0 - Multiply , 1 - Transpose
	cout << "task: " << (task == 0 ? "Multiply" : "Transpose") << endl;    

    float* a = new float[n * n];
    float* b = new float[n * n];
    float* c = new float[n * n];

    FillRandom(a, n);
    FillRandom(b, n);

    	if (task == 0)
	{
		const auto startTime = clock();
		MultSimple(a, b, c, n);
		const auto endTime = clock();
		cout << "timeSimple: " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	}
	else
	{
		const auto startTime = clock();
		Transpose(a, b, n);
		const auto endTime = clock();
		cout << "timeTranspose: " << double(endTime - startTime) / CLOCKS_PER_SEC << '\n';
	}

    
    delete[] a;
    delete[] b;
    delete[] c;
}


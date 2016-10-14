#include <iostream>
#include <random>


using namespace std;

const int NM = 100;

vector<int> v (NM);

vector<vector<int> > g(NM, vector<int>(NM, 0));

int main ()
{

	g[0][0] = 1;
	v[0] = 2;


	for (int i = 1; i < NM; ++i)
	{

		double r = ((double) rand() / (RAND_MAX));

		double ni = i + 1;
		double pn = 1 / (2 * ni - 1);

		if (r < pn)
		{
			g[i][i] = 1;
			v[i] = 2;
		}
		else
		{
			double pv = pn;

			for (int k = 0; k < i; k++)
			{
				pv += v[k] / (2 * ni - 1.0);

				if (r <= pv)
				{
					g[i][k] = 1;
					v[i] += 1;
					v[k] += 1;
					break;
				}
			}
		}

	}

	for (int j = 0; j < NM; ++j)
	{
		for (int i = 0; i < NM;  ++i)
		{
			cout<< g[j][i] << " ";
		}
		cout<< ""<<endl;
	}



	return 0;
}

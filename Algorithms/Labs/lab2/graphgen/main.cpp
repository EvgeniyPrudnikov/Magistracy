// graphgen.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <random>
#include <iostream>
#include <queue>
#include <ctime>
#include <math.h>

using namespace std;

const int M = 12;
const int N = 1200;
const int NM = N*M;

void GenerateRandomGraph(vector<int> &g, vector<int> &v);
void GenerateWebGraph(vector<int> &g, vector<int> &v, vector<vector<int>> &wg, vector<int> &wv);
int FindConnCompnts(vector<vector<int>> & wg, vector<int> & connectedComponents);
void FloydWarshallAlgorithm(vector<vector<int>> & wg, vector<vector<int>> & dists);
int FindDiamAvgDistsComponents(vector<int> & connectedComponents, int componentsCount,vector<vector<int>> & dists, vector<double> & avgDists);

int main()
{
	vector<int> verticesOfRandomGraph(NM);
	vector<int> randomGraph(NM);
	vector<int> verticesOfWebGraph(N); // for #(n,d) histogramm
	vector<vector<int>> webGraph(N, vector<int>(N, 0));
	vector<int> connectedComponents(N, -1);
	int componentsCount;
	vector<vector<int>> dists(N, vector<int>(N, 0));
	int webGraphDiam;


	randomGraph[0] = 0; // first vertice with loop
	verticesOfRandomGraph[0] = 2; // # of edges in first vertice

	{
		GenerateRandomGraph(randomGraph, verticesOfRandomGraph);

		GenerateWebGraph(randomGraph, verticesOfRandomGraph, webGraph, verticesOfWebGraph);
	}


	componentsCount = FindConnCompnts(webGraph, connectedComponents);

	vector<double> avgDists(componentsCount);

	FloydWarshallAlgorithm(webGraph, dists);

	webGraphDiam = FindDiamAvgDistsComponents(connectedComponents, componentsCount, dists, avgDists);

	{
		cout << "n = " << N << endl;
		cout << "m = " << M << endl;
		cout << "Connected Components Count: " << componentsCount << endl;

		for (int i = 0; i < avgDists.size(); i++)
			cout << "Component " << i << " avg dist: " << avgDists[i];

		cout << endl << "webGraph Diam: " << webGraphDiam << endl;
		cout << "web Graph estimated diam range: " << log(N) / log(log(N))<< endl;
		cout << endl << "done." << endl;
	}
	getchar();

	return 0;
}

void FloydWarshallAlgorithm(vector<vector<int>> & wg , vector<vector<int>> & dists)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			dists[i][j] = wg[i][j] > 0 ? 1 : N*N;
		}
	}

	for (int i = 0; i < N; i++)
	{
		dists[i][i] = 0;
	}

	cout << endl;

	for (int k = 0; k<N; ++k)
		for (int i = 0; i<N; ++i)
			for (int j = 0; j<N; ++j)
				dists[i][j] = min(dists[i][j], dists[i][k] + dists[k][j]);

	wg.clear();
}

int FindDiamAvgDistsComponents(vector<int> & connectedComponents, int componentsCount, vector<vector<int>> & dists,vector<double> & avgDists)
{
	int d = 0;
	for (int i = 0; i < componentsCount; i++)
	{
		int comp_dist = 0;
		int comp_dist_n = 0;
		for (int j = 0; j < N; j++)
		{
			if (connectedComponents[j] != i)
			{
				continue;
			}
			for (int k = j + 1; k < N; k++)
			{
				if (connectedComponents[k] != i) {
					continue;
				}
				comp_dist += dists[j][k];
				++comp_dist_n;
				if (d < dists[j][k])
				{
					d = dists[j][k];
				}
			}
		}
		avgDists[i] = comp_dist / static_cast<double>(comp_dist_n);
	}
	return d;
}

int FindConnCompnts(vector<vector<int>> & wg, vector<int> & connectedComponents)
{
	int componentsCount = 0;

	queue <int> q;
	for (int i = 0; i < N; i++)
	{
		if (connectedComponents[i] != -1)
			continue;
		q.push(i);
		while (!q.empty())
		{
			int v = q.front();
			q.pop();

			if (connectedComponents[v] != -1)
				continue;

			connectedComponents[v] = componentsCount;

			for (int j = 0; j < N; j++)
			{
				if (wg[j][v] > 0 && connectedComponents[j] == -1)
				{
					q.push(j);
				}
			}
		}
		componentsCount++;
	}
	return componentsCount;
}

void GenerateRandomGraph(vector<int> &g, vector<int> &v)
{

	srand((unsigned int)time(NULL));

	for (int i = 1; i < NM; i++)
	{
		double r = (double)rand() / RAND_MAX;
		double ni = i + 1;
		double probabilityInN = 1 / (2 * ni - 1);

		if (r < probabilityInN)
		{
			g[i] = i;
			v[i] = 2;
		}
		else
		{
			double probabilityInV = probabilityInN;

			for (int k = 0; k < i; k++)
			{
				probabilityInV += v[k] / (2 * ni - 1.0);

				if (r <= probabilityInV)
				{
					g[i] = k;
					v[i] += 1;
					v[k] += 1;
					break;
				}
			}
		}
	}
}

void GenerateWebGraph(vector<int> &g, vector<int> &v, vector<vector<int>> &wg, vector<int> &wv)
{

	for (int i = 0; i < NM; i++)
	{
		wg[i / M][g[i] / M] += 1;
		wg[g[i] / M][i / M] += 1;
		wv[i / M] += v[i];
	}

	g.clear();	v.clear();

}

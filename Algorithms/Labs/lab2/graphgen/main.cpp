// graphGen.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <random>
#include <iostream>
#include <queue>

using namespace std;

const int M = 12;
const int N = 12;
const int NM = N*M;

void GenerateRandomGraph(vector<int> &g, vector<int> &v);
void GenerateWebGraph(vector<int> &g, vector<int> &v, vector<vector<int>> &wg, vector<int> &wv);
void FindAvgDistsConnCompnts(vector<vector<int>> & wg, vector<double> & avgDists);
int BFS(vector<vector<int>> graph );

int main()
{
	vector<int> verticesOfRandomGraph(NM);
	vector<int> randomGraph(NM);
	vector<int> verticesOfWebGraph(N);
	vector<vector<int>> webGraph (N, vector<int> (N, 0));
	double diam = 0;
	vector<double> avgDists;

	randomGraph[0] = 1; // first vertice with loop
	verticesOfRandomGraph[0] = 2; // # of edges in first vertice

	GenerateRandomGraph(randomGraph, verticesOfRandomGraph);

	GenerateWebGraph(randomGraph, verticesOfRandomGraph, webGraph, verticesOfWebGraph);

	FindAvgDistsConnCompnts(webGraph, avgDists);

	cout<< avgDists.size()<<endl;

	for (int i = 0; i <avgDists.size() ; ++i)
	{
		cout<< avgDists[i]<<endl;
	}

	int lol = 0;

	lol = BFS(webGraph);


	cout << "done"<<endl;

	//getchar();

	return 0;
}

int BFS(vector<vector<int>> graph )
{
	vector<bool> visited (N);
	vector <int> components(N);
	vector<int> dist(N,0);

	int components_n = 0;
	for (int i = 0; i < N; ++i)
	{
		if (visited[i])
		{
			continue;
		}
		++components_n;
		std::queue<int> queuev;
		queuev.push(i);
		components[i] = components_n;
		visited[i] = true;
		while (!queuev.empty())
		{
			int v = queuev.front();
			queuev.pop();
			for (int j = 0; j < N; ++j)
			{
				if (!visited[j] && graph[v][j] > 0)
				{
					queuev.push(j);
					components[j] = components_n;
					visited[j] = true;
					dist[graph[v][j]] = dist[v] + 1;
				}
			}
		}
	}

	cout<<"BFS dist:" << endl;

	for (int k = 0; k < N; ++k)
	{
		cout<<dist[k]<< " ";
	}

	return components_n;
}


void FindAvgDistsConnCompnts(vector<vector<int>> & wg, vector<double> & avgDists)
{
	int componentsCount = 0;
	vector<int> connectedComponents(N, -1);
	vector<int> dist(N, 0);
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
				if (wg[j][v] > 0  && connectedComponents[j] == -1)
				{
					int to = wg[j][v];
					q.push(j);
					dist[to] = dist[v] + 1;
				}
			}
		}
		componentsCount++;
	}

	for (int k = 0; k < N; ++k)
	{
		cout<< dist[k]<<" ";
	}

	avgDists.resize(componentsCount);

	for (int i = 0; i < componentsCount; i++)
	{
		double vecCount = 0;
		for (int j = 0; j < N; j++)
		{
			if (connectedComponents[j] == i)
			{
				vecCount++;
				avgDists[i] += dist[j];
			}
		}
		vecCount - 1 != 0 ? avgDists[i] /= vecCount - 1 : avgDists[i] = 0;
	}
}


void GenerateRandomGraph(vector<int> &g, vector<int> &v)
{

	srand((unsigned int) time(NULL));

	for (int i = 1; i < NM; i++)
	{
		double r = (double) rand() / RAND_MAX;
		double ni = i + 1;
		double probabilityInN = 1 / (2 * ni - 1);

		if (r < probabilityInN)
		{
			g[i] = i;
			v[i] = 2;
		} else
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
		wv[i / M] += v[i];
	}

	g.clear();	v.clear();

}
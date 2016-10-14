using System;
using System.Collections.Generic;

namespace GraphGenerator
{
	class Program
	{
		public static void Main (string[] args)
		{


			int M = 1;
			int N = 10;
			/*
			int[] v = new int[N];
			int[] vm = new int[N / M];

			short[,] g = new short[N, N];
			short[,] gm = new short[N / M, N / M];
			*/

			List<int> vl = new List<int> ();
			List<List<int>> gl = new List<List<int>> ();

			gl.Add (new List<int> (1));
			vl.Add (2);


			Random rand = new Random();
			for (int i = 1; i < N; i++) 
			{
				double r = rand.NextDouble();
				double ni = i + 1;
				double pn = 1 / (2 * ni - 1);

				if (r < pn)
				{
					gl.Add (new List<int>(i));
					vl.Add (2);
				}
				else
				{
					double pv = pn;

					for (int k = 0; k < i; k++)
					{
						pv += vl[k] / (2 * ni - 1.0);

						if (r <= pv)
						{
							gl.Add (new List<int>(k));
							vl[i] += 1;
							vl[k] += 1;
							break;
						}
					}
				}
			}

			int a = 1;


			//InitializeGraphs (g, gm, v, vm, N, M);

			//GenerateRandomGraph (g, v, N);

			//GenerateGraphM (g, gm, N, M);

			//print(g);
			Console.WriteLine ();
//			print(gm);
			Console.WriteLine ();
//			print(v);

			//GraphParameters (gm, N, M);

			Console.ReadLine ();
		}

		static void print (List<List<int>> gl , List<int> vl )
		{
			for (int i = 0; i < 10; i++) {
				
			}
			
		}


		static void print (short [,] g)
		{
			for (int i = 0; i < g.GetLength(0); i++)
			{
				for (int j = 0; j < g.GetLength(1); j++)
				{
					Console.Write(g[i, j] + ", ");
				}
				Console.WriteLine();
			}
		}

		static void print(int[] v)
		{

			for (int j = 0; j < v.GetLength(0); j++)
			{
				Console.Write(v[j] + ", ");
			}
			Console.WriteLine();

		}

		static void InitializeGraphs(short[,] g, short[,] gm, int[] v,int[] vm, int n, int m)
		{
			for (int i = 0; i < n; i++)
			{
				v[i] = 0;
				vm[i / m] = 0;

				for (int j = 0; j < n; j++)
				{
					g[i, j] = 0;
					gm[i / m, j / m] = 0;
				}
			}

			g[0, 0] = 1;
			v[0] = 2;
		}

		static void GenerateRandomGraph(short[,] g, int[] v, int n)
		{
			Random rand = new Random();

			for (int i = 1; i < n; i++)
			{
				double r = rand.NextDouble();
				double ni = i + 1;
				double pn = 1 / (2 * ni - 1);

				if (r < pn)
				{
					g[i, i] = 1;
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
							g[i, k] = 1;
							v[i] += 1;
							v[k] += 1;
							break;
						}
					}
				}
			}
		}

		static void GenerateGraphM(short[,] g, short[,] gm, int n, int m)
		{
			for (int i = 0; i < n; i += m)
			{
				for (int j = 0; j < n; j += m)
				{
					for (int k = i; k < i + m; k++)
					{
						for (int u = j; u < j + m; u++)
						{
							gm[i / m, j / m] += g[k, u];
						}
					}
				}
			}
		}

		static void GraphParameters(int[,] gm, int n, int m)
		{

			int l = n / m;

			const int INF = int.MaxValue; // Бесконечность
			int[,] d = new int[l, l];
			int[] e = new int[l]; // Эксцентриситет вершин
			List<int> c = new List<int>();
			int rad = INF; // Радиус графа
			int diam = 0; // Диаметр графа

			// Алгоритм Флойда-Уоршелла
			for (int k = 0; k < l; k++)
			{
				for (int j = 0; j < l; j++)
				{
					for (int i = 0; i < l; i++)
					{
						d[i, j] = Math.Min(gm[i, j], gm[i, k] + gm[k, j]);
					}
				}
			}

			// Нахождение эксцентриситета
			for (int i = 0; i < l; i++)
			{
				for (int j = 0; j < l; j++)
				{
					e[i] = Math.Max(e[i], d[i, j]);
				}
			}

			// Нахождение диаметра и радиуса
			for (int i = 0; i < l; i++)
			{
				rad = Math.Min(rad, e[i]);
				diam = Math.Max(diam, e[i]);
			}

			for (int i = 0; i < l; i++)
			{
				if (e[i] == rad)
				{
					c.Add(i);
				}

			}

			// print(gm);
			Console.WriteLine();
			// print(d);
		}

	}
}

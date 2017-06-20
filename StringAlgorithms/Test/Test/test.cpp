#include <iostream> 
#include <string> 
#include <vector> 
#include <algorithm> 
#include <fstream>

using namespace std;



vector<int> z_function(string s) {
	int n = int(s.length());
	vector<int> z(n);
	for (int i = 1, l = 0, r = 0; i<n; ++i) {

		if (i <= r) {

			z[i] = min(r - i + 1, z[i - l]);
		}
		while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {

			++z[i];
		}
		if (i + z[i] - 1 > r) {

			l = i, r = i + z[i] - 1;
		}
	}
	return z;
}




int main() {

	int n;
	string s1;
	string s2;
	
	int K = -1;
	ofstream outfile("output.txt");
	ifstream infile("input.txt");

	infile >> n;
	infile >> s1;
	infile >> s2;

	string super_s = s2 + "#" + s1 + s1;
	vector<int> z_f = z_function(super_s);


	for (int i = n + 1; i < z_f.size(); i++)
	{
		if(z_f[i] == n)
		{
			cout << i << " ";
			K = i - (n + 1);
			break;
		}
	}

	outfile << K;
	return 0;
}
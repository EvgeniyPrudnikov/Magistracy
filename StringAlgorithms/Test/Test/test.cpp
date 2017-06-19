#include <iostream> 
#include <string> 
#include <vector> 
#include <algorithm> 

using namespace std;

const int k = 26, NMAX = 10000;


struct bohr_vrtx {
	int next_vrtx[k], pat_num, suff_link, auto_move[k], par, suff_flink;
	bool flag;
	char symb;
};


vector<bohr_vrtx> bohr;
vector<string> pattern;


bohr_vrtx make_bohr_vrtx(int p, char c) {
	bohr_vrtx v;
	memset(v.next_vrtx, 255, sizeof(v.next_vrtx));
	memset(v.auto_move, 255, sizeof(v.auto_move));
	v.flag = false;
	v.suff_link = -1;
	v.par = p;
	v.symb = c;
	v.suff_flink = -1;
	return v;
}


void bohr_ini() {
	bohr.push_back(make_bohr_vrtx(0, '$'));
}


void add_string_to_bohr(const string& s) {
	int num = 0;
	for (int i = 0; i<s.length(); i++) {
		char ch = s[i] - 'a';
		if (bohr[num].next_vrtx[ch] == -1) {
			bohr.push_back(make_bohr_vrtx(num, ch));
			bohr[num].next_vrtx[ch] = bohr.size() - 1;
		}
		num = bohr[num].next_vrtx[ch];
	}
	bohr[num].flag = true;
	pattern.push_back(s);
	bohr[num].pat_num = pattern.size() - 1;
}


bool is_string_in_bohr(const string& s) {
	int num = 0;
	for (int i = 0; i<s.length(); i++) {
		char ch = s[i] - 'a';
		if (bohr[num].next_vrtx[ch] == -1) {
			return false;
		}
		num = bohr[num].next_vrtx[ch];
	}
	return true;
}


int get_auto_move(int v, char ch);


int get_suff_link(int v) {
	if (bohr[v].suff_link == -1)
		if (v == 0 || bohr[v].par == 0)
			bohr[v].suff_link = 0;
		else
			bohr[v].suff_link = get_auto_move(get_suff_link(bohr[v].par), bohr[v].symb);
	return bohr[v].suff_link;
}


int get_auto_move(int v, char ch) {
	if (bohr[v].auto_move[ch] == -1)
		if (bohr[v].next_vrtx[ch] != -1)
			bohr[v].auto_move[ch] = bohr[v].next_vrtx[ch];
		else
			if (v == 0)
				bohr[v].auto_move[ch] = 0;
			else
				bohr[v].auto_move[ch] = get_auto_move(get_suff_link(v), ch);
	return bohr[v].auto_move[ch];
}


int get_suff_flink(int v) {
	if (bohr[v].suff_flink == -1) {
		int u = get_suff_link(v);
		if (u == 0)
			bohr[v].suff_flink = 0;
		else
			bohr[v].suff_flink = (bohr[u].flag) ? u : get_suff_flink(u);
	}
	return bohr[v].suff_flink;
}


void check(int v, int i) {
	for (int u = v; u != 0; u = get_suff_flink(u)) {
		if (bohr[u].flag) {
			cout << i - pattern[bohr[u].pat_num].length() + 1 << " " << pattern[bohr[u].pat_num] << endl;
		}
	}
}


void find_all_pos(const string& s) {
	int u = 0;
	for (int i = 0; i<s.length(); i++) {
		u = get_auto_move(u, s[i] - 'a');
		check(u, i + 1);
	}
}


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


vector<int> prefix_function(string s) {
	int n = int(s.length());
	vector<int> pi(n);
	for (int i = 1; i<n; ++i) {

		int j = pi[i - 1];

		while (j > 0 && s[i] != s[j]) {
			j = pi[j - 1];
		}

		if (s[i] == s[j]) {
			++j;
		}

		pi[i] = j;
	}
	return pi;
}



int main() {

	string s1 = "abc";
	string s2 = "abcdabc";

	string s = s1 + "#" + s2;

	// KMP 
	//----------------------------------------------------
	
	vector<int> p = prefix_function(s);

	for (int i = 0; i < p.size(); i++) {

		if (p[i] == s1.size()) {

			cout << i - 2 * s1.size() + 1 << " ";

		}
	}
	//------------------------------------------------------


	bohr_ini();
	add_string_to_bohr(s1);
	find_all_pos(s2);


	getchar();

	return 0;
}
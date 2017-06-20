#include <iostream> 
#include <string> 
#include <vector> 
#include <algorithm> 
#include <fstream>

using namespace std;

const int k = 58;


struct trie_vrtx {
	int next_vrtx[k], pat_num, suff_link, auto_move[k], par, suff_flink;
	bool flag;
	char symb;
};


vector<trie_vrtx> trie;
vector<string> pattern;


trie_vrtx make_trie_vrtx(int p, char c) {
	trie_vrtx v;
	memset(v.next_vrtx, 255, sizeof(v.next_vrtx));
	memset(v.auto_move, 255, sizeof(v.auto_move));
	v.flag = false;
	v.suff_link = -1;
	v.par = p;
	v.symb = c;
	v.suff_flink = -1;
	return v;
}


void trie_ini() {
	trie.push_back(make_trie_vrtx(0, '$'));
}


void add_string_to_trie(const string& s) {
	int num = 0;
	for (int i = 0; i<s.length(); i++) {
		char ch = s[i] - 'A';
		if (trie[num].next_vrtx[ch] == -1) {
			trie.push_back(make_trie_vrtx(num, ch));
			trie[num].next_vrtx[ch] = trie.size() - 1;
		}
		num = trie[num].next_vrtx[ch];
	}
	trie[num].flag = true;
	pattern.push_back(s);
	trie[num].pat_num = pattern.size() - 1;
}


bool is_string_in_trie(const string& s) {
	int num = 0;
	for (int i = 0; i<s.length(); i++) {
		char ch = s[i] - 'A';
		if (trie[num].next_vrtx[ch] == -1) {
			return false;
		}
		num = trie[num].next_vrtx[ch];
	}
	return true;
}


int get_auto_move(int v, char ch);


int get_suff_link(int v) {
	if (trie[v].suff_link == -1)
		if (v == 0 || trie[v].par == 0)
			trie[v].suff_link = 0;
		else
			trie[v].suff_link = get_auto_move(get_suff_link(trie[v].par), trie[v].symb);
	return trie[v].suff_link;
}


int get_auto_move(int v, char ch) {
	if (trie[v].auto_move[ch] == -1)
		if (trie[v].next_vrtx[ch] != -1)
			trie[v].auto_move[ch] = trie[v].next_vrtx[ch];
		else
			if (v == 0)
				trie[v].auto_move[ch] = 0;
			else
				trie[v].auto_move[ch] = get_auto_move(get_suff_link(v), ch);
	return trie[v].auto_move[ch];
}


int get_suff_flink(int v) {
	if (trie[v].suff_flink == -1) {
		int u = get_suff_link(v);
		if (u == 0)
			trie[v].suff_flink = 0;
		else
			trie[v].suff_flink = (trie[u].flag) ? u : get_suff_flink(u);
	}
	return trie[v].suff_flink;
}


bool check_flinks(int v, int i, string& ans) {

	for (int u = v; u != 0; u = get_suff_flink(u)) {
		if (trie[u].flag && trie[u].suff_flink != -1) {
			ans = "YES";
			return true;
		}
	}
	return false;
}



void find_all_pos(const string& s, string& ans) {
	int u = 0;
	for (int i = 0; i < s.length(); i++) {
		u = get_auto_move(u, s[i] - 'A');
		if (check_flinks(u, i + 1, ans)) break;
	}
}



int main() {

	trie_ini();

	ofstream outfile("output.txt");
	ifstream infile("input.txt");
	string answer = "NO";

	int n;
	infile >> n;

	for (int i = 0; i < n; i++)
	{
		string s;
		infile >> s;
		add_string_to_trie(s);
	}

	infile.seekg(0);
	infile >> n;

	for (int i = 0; i < n; i++)
	{
		string s;
		infile >> s;
		find_all_pos(s, answer);
		if (answer == "YES") {
			break;
		}
	}
	outfile << answer;
	return 0;
}
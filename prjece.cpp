#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <time.h>
#include <time.h>
#include <pthread.h>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
using namespace std;

int n;
vector<int> I;
vector<int> J;
vector<int> ans0, ans1, ans2;
pthread_t thread0, thread1, thread2;
bool left0;

bool is_possible(int k, bool print=false)
{
	unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
	vector<vector<Minisat::Lit>> x(n);
	
	for (int i = 0; i < n; i ++ )
		for (int j = 0; j < k; j ++ )
			x[i].push_back(Minisat::mkLit(solver->newVar()));	

	// At least one vertex is the ith vertex in the vertex cover:
	for (int i = 0; i < k; i ++) {
		Minisat::vec<Minisat::Lit> ps;
		for (int j = 0; j < n; j ++)
			ps.push(x[j][i]);
		solver->addClause(ps);
	}	
	// No one vertex can appear twice in a vertex cover.
	for (int m = 0; m < n; m ++ )
		for (int p = 0; p < k; p ++)
			for (int q = p + 1; q < k; q ++)
				solver->addClause(~x[m][p], ~x[m][q]);
	
	// No more than one vertex appears in the mth position of the vertex cover.
	for (int m = 0; m < k; m ++ )
	{
		for (int p = 0; p < n; p ++)
			for (int q = p + 1; q < n; q ++)
				solver->addClause(~x[p][m], ~x[q][m]);

	}
	
	// Every edge is incident to at least one vertex in the vertex cover.
	int m = I.size();
	while(m--) {
		int i = I[m], j = J[m];
		Minisat::vec<Minisat::Lit> ps;
		for (int l = 0; l < k; l ++) {
			ps.push(x[i][l]);
			ps.push(x[j][l]);
		}
		solver->addClause(ps);
	}

	
	bool ans = solver->solve();
	
	if (print) {
		int i, j;
		for (i = 0; i < n; i ++ ) {
			for (j = 0; j < k; j ++)
				if (!Minisat::toInt(solver->modelValue(x[i][j])))
					break;
			if (j < k) {
				ans0.push_back(i);
			}
		}
	}
	
	return ans;
}

int solve()
{
	int st = 1, mid, ed = n;
	while (st < ed) {
		mid = (st + ed) / 2;
		if (is_possible(mid))
			ed = mid;
		else
			st = mid + 1;
	}
	return st;
	
}


static void* approx_1(void* arg)
{
	int m = I.size();
	int *degree = new int[n];
	bool *vEdge = new bool[m];
	vector<int> vNode;
	memset(vEdge, 0, sizeof(bool) * m);
	
	while(1) {
		memset(degree, 0, sizeof(int) * n);
		int mx = 0, mxNode;
		for (int i = 0; i < m; i ++)
			if (vEdge[i] == false) {
				degree[I[i]] ++;
				degree[J[i]] ++;
				if (mx < degree[I[i]])
					mx = degree[I[i]], mxNode = I[i];
				if (mx < degree[J[i]])
					mx = degree[J[i]], mxNode = J[i];
			}
		if (mx == 0)
			break;

		vNode.push_back(mxNode);
		for (int i = 0; i < m; i ++)
			if (I[i] == mxNode || J[i] == mxNode)
				vEdge[i] = true;
		
	}
	if (vNode.size() > 0) {
		sort(vNode.begin(), vNode.end());
		ans1.assign(vNode.begin(), vNode.end());
	}
	delete vEdge;
	delete degree;
	return NULL;
}

static void* approx_2(void* arg)
{
	int m = I.size();
	vector<int> vNode;
	bool *vEdge = new bool[m];
	memset(vEdge, 0, sizeof(bool) * m);
	
	for (int i = 0; i < m; i ++) {
		if(vEdge[i] == false) {
			vNode.push_back(I[i]);
			vNode.push_back(J[i]);
			for (int j = 0; j < m; j ++)
				if (I[j] == I[i] || J[j] == I[i] || I[j] == J[i] || J[j] == J[i])
					vEdge[j] = true;
		}
	}
	if (vNode.size() > 0) {
		sort(vNode.begin(), vNode.end());
		ans2.assign(vNode.begin(), vNode.end());
	}
	delete vEdge;
	return NULL;
}


void get_edges(string _edges)
{
	I.clear();
	J.clear();
	int i, j, v;
	bool ij = true;
	for (i = 0; _edges[i]; i ++) {
		if (_edges[i] < '0' || _edges[i] > '9')
			continue;
		v = 0;
		for (j = i; _edges[j] && _edges[j] >= '0' && _edges[j] <= '9'; j ++)
			v = v * 10 + (_edges[j] - '0');
		i = j;
		if (ij) {
			I.push_back(v);
			ij = false;
		}
		else {
			J.push_back(v);
			ij = true;
		}
	}
}

static void* cnf_sat(void* arg)
{
	
	if (I.size() > 0)
		is_possible(solve(), true);
	left0 = false;
	return NULL;
}

void print_ans(vector<int> ans, string prefix)
{
	cout << prefix;
	for (unsigned i = 0; i < ans.size(); i ++) {
		if (i)
			cout << ",";
		cout << ans[i];
	}
	cout << endl;
}

int main()
{
	char op;
	string _edges;
	int s;
	int timeLimit = 30;

	
	while (cin >> op >> n >> op >> _edges) {
		get_edges(_edges);
				
		left0 = true;
		ans0.clear();
		ans1.clear();
		ans2.clear();
		s = pthread_create(&thread0, NULL, cnf_sat, NULL);
		if (s != 0) break;
		s = pthread_create(&thread1, NULL, approx_1, NULL);
		if (s != 0) break;
		s = pthread_create(&thread2, NULL, approx_2, NULL);
		if (s != 0) break;

		// pthread_join(thread0, NULL);
		// pthread_join(thread1, NULL);
		// pthread_join(thread2, NULL);
		
		int j;
		for (j = 0; j < timeLimit && left0; j ++)
			sleep(1);

		if (!left0)
		   	print_ans(ans0, "CNF-SAT-VC: ");
		else {
			cout << "CNF-SAT-VC: timeout" << endl;
			pthread_cancel(thread0);
		}
		print_ans(ans1, "APPROX-VC-1: ");
		print_ans(ans2, "APPROX-VC-2: ");
		
	}
	
	return 0;
}

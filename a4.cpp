#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
using namespace std;

int n;
vector<int> I;
vector<int> J;

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
		for (int p = 0; p < n; p ++)
			for (int q = p + 1; q < n; q ++)
				solver->addClause(~x[p][m], ~x[q][m]);

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
			if (j < k)
				cout << i << " ";
		}
		cout << endl;
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

int main()
{
	char op;
	string _edges;
	
	while (cin >> op >> n >> op >> _edges) {
		get_edges(_edges);
		if (I.size() == 0)
			continue;
		else
			is_possible(solve(), true);
	}
	
	return 0;
}

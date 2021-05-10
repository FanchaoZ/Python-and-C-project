#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <queue>
using namespace std;
const int MAX = 2e5 + 5;
char op[5];
char e[MAX*4];
int n,tmp1,tmp2;
vector<int> vv[MAX];
int vis[MAX];
int pre[MAX];
vector<int> bfs(int s,int t) {
    vector<int> path;
    for(int i = 0; i<n; i++) vis[i] = 0;
    queue<int> q;
    q.push(s);
    vis[s] = 1;
    int find_path = 0;
    while(q.size()) {
        int cur = q.front();q.pop();
        int up = vv[cur].size();
        for(int i = 0; i<up; i++) {
            int v = vv[cur][i];
            if(vis[v]) continue;
            vis[v] = 1;
            q.push(v);
            pre[v] = cur;
            if(v == t) {
                find_path = 1;
                break;
            }
        }
    }
    if(find_path == 0) {
        return path;
    }
    path.push_back(t);
    for(int v = pre[t]; v!=s; v = pre[v]) {
        path.push_back(v);
    }
    path.push_back(s);
    return path;
}
int main()
{
    // int again = 0;
    
    while(cin>>op>>tmp1) {
        if(op[0] == 'V') {
            cout << op << " " << tmp1 << endl;
            n = tmp1;
            for(int i = 0; i<n; i++) vv[i].clear();
            cin>>op>>e;
            cout << op << " " << e << endl;
            int len = strlen(e),i = 1,flag = 0,j,a,b;
            while(i < len) {
                if(e[i] < '0' || e[i] > '9') {
                    i ++; continue;
                }
                j = i;
                while(j<len && e[j] >= '0' && e[j] <= '9') j++;
                int num = 0;
                for(int k = i; k<j; k++) {
                    num = num * 10 + e[k]-'0';
                }
                if(flag == 0) a = num;
                if(flag == 1) {
                    b = num;
                    if(a < n && b < n) {
                        vv[a].push_back(b);
                        vv[b].push_back(a);
                    }
                    else cerr<<"Error: Wrong edge("<<a<<", "<<b<<")!\n";
                }
                flag = 1-flag;
                i = j;
            }
        }
        else {
            cin>>tmp2;
            vector<int> ans = bfs(tmp1,tmp2);
            int up = ans.size();
            if(up == 0) {
                cerr<<"Error: path does not exist between the vertices!"<<endl;
            }
            else {
                for(int i = up-1; i>=0; i--) {
                    cout << ans[i];
                    if(i == 0) cout << endl;
                    else cout<<"-";
                }   
            }
            // break;
        }
        
    }   
    
    
    return 0;
}


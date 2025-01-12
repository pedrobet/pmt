#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <deque>
#include <cassert>
#include <fstream>
#include "algorithms.hpp"

using namespace std;

void build_goto(vector<string> pat_set,vector<vector<int>>& go_to, vector<vector<int>>& occ){
    go_to.push_back(vector<int> (128, -1));
    occ.push_back(vector<int> ());
    int nxt = 1;
    for(int k = 0; k<(int)pat_set.size(); k++){
        string pat = pat_set[k];
        int m = (int)pat.size();
        int cur = 0;
        int j=0;
        int c = int(pat[j]);
        
        while (j<m and go_to[cur][c] != -1)
        {
            cur = go_to[cur][c];
            j++;
            c = int(pat[j]);
            
        }
        
        while (j<m)
        {
            c = int(pat[j]);
            
            go_to[cur][c] = nxt;
            go_to.push_back(vector<int> (128, -1));
            occ.push_back(vector<int>());
            cur=nxt;
            j++;
            nxt++;
        }
        occ[cur].push_back(k);
    }
    for(int c = 0; c < 128; c++){
        if(go_to[0][c]== -1){
            go_to[0][c]=0;
        }
    }
}

void build_fail(vector<string> pat_set,vector<vector<int>>& go_to, vector<vector<int>>& occ,vector<int>& fails){
    int n = (int)go_to.size();
    fails.resize(n,-1);
    deque<int> bfs;
    for(int c = 0; c<128; c++){
        if(go_to[0][c] > 0){
            bfs.push_back(go_to[0][c]);
            fails[go_to[0][c]] = 0;
        }
    }
    while ((int)bfs.size() > 0)
    {
        int cur = bfs[0];
        bfs.pop_front();
        for(int c = 0; c<128;c++){
            int suc = go_to[cur][c];
            if(suc >= 0){
                bfs.push_back(suc);
                int brd = fails[cur];
                assert(brd>=0);
                while (go_to[brd][c]<0) brd = fails[brd];
                fails[suc] = go_to[brd][c];
                occ[suc].insert(occ[suc].end(),occ[fails[suc]].begin(),occ[fails[suc]].end());
            }
        }
    }
}
void build_fsm(vector<string> pat_set, vector<vector<int>>& go_to,vector<vector<int>>& occ,vector<int>& fails){
    build_goto(pat_set,go_to,occ);
    build_fail(pat_set,go_to,occ,fails);    
}

int aho_corasick(string txt, vector<int>& count,vector<vector<int>>& go_to, vector<vector<int>>& occ,vector<int>& fails){
    int n = (int)txt.size();
    int cur = 0;
    int i = 0;
    int find =0;
    while (i<n)
    {
        int c = int(txt[i]);
        while (go_to[cur][c] < 0)cur = fails[cur];
        cur = go_to[cur][c];
        for(int k = 0; k<(int)occ[cur].size();k++){
            count[occ[cur][k]]++;
            find++;
        }
        i++;
    }
    return find;
}
void run_aho_corasik(vector<string> txt_set, vector<string> pat_set, bool c){
    vector<vector<int>> lps_set;
    int l_txt_set = (int)txt_set.size();
    int l_pat_set = (int)pat_set.size();
    vector<int> count (l_pat_set,0);
    vector<vector<int>> go_to;
    vector<vector<int>> occ;
    vector<int> fails;
    build_fsm(pat_set,go_to,occ,fails);
    for(int i = 0; i<l_txt_set;i++){
        ifstream txt(txt_set[i]);
        string line;
        int n_line =1;
        while (!txt.eof())
        {
            getline(txt,line);
            int find = aho_corasick(line,count,go_to,occ,fails);
            if(find>0 and !c){
                cout << "Line "<< n_line <<": "<<line<<endl;
            }
            n_line++;
        }
        txt.close();
    }
    if(c) for(int i = 0;i<l_pat_set;i++){
        cout<<pat_set[i]<<": "<<count[i]<<endl;
    }
}
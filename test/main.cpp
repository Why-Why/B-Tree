#include "../include/BTree.h"
#include <iostream>
#include <map>

using namespace std;

const int T=10000;

int num[T],val[T];

int main() {
	BTree <int,int> t;
	t.create("ttt");

	srand(time(0));

	for(int i=0;i<T;++i)
		num[i]=i,val[i]=rand();
	for(int i=T-1;i>=0;--i) {
		int t=rand()%(i+1);
		swap(num[t],num[i]);
	}

	map <int,int> m;

	for(int i=0;i<T;++i) {
		if(t.insert(num[i],val[i])==0) cerr<<"Error insert\n";
		m.insert(pair<int,int>(num[i],val[i]));
	}

	for(int i=0;i<T;++i) {
		int ans;
		if(t.find(i,ans)==0) cerr<<"Error find\n";
		if(m.find(i)->second!=ans) cerr<<"NO\n";
	}
	cerr<<"OK\n";

	return 0;
}

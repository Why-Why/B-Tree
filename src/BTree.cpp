/*
   BTree class.
*/

#include "../include/BTree.h"

#include <cstring>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <string>

using std::string;
using std::lower_bound;

template <typename T_key,typename T_val>
BTree<T_key,T_val>::BTree() {
	fp_db1=fp_db2=NULL;
	memset(&head_db1,0,sizeof(head_db1));
	memset(&head_db2,0,sizeof(head_db2));
	buf1=buf2=NULL;
}

template <typename T_key,typename T_val>
BTree<T_key,T_val>::~BTree() {
	rewrite_head();
	clear();
}

template <typename T_key,typename T_val>
void BTree<T_key,T_val>::clear() {
	if(fp_db1) fclose(fp_db1);
	if(fp_db2) fclose(fp_db2);

	if(buf1) delete [] buf1;
	if(buf2) delete [] buf2;

	memset(&head_db1,0,sizeof(head_db1));
	memset(&head_db2,0,sizeof(head_db2));
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::check() {
	if(fp_db1 && fp_db2 && buf1 && buf2) return 1;
	return 0;
}

template <typename T_key,typename T_val>
long long BTree<T_key,T_val>::betterbase2(int x) {
	for(long long ret=1;;ret<<=1)
		if(ret>=x) return ret;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::read_head_db1() {
	if(NULL==fp_db1) return 0;

	fseek(fp_db1,0,SEEK_SET);
	fread(&head_db1,sizeof(Head_db1),1,fp_db1);
	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::read_head_db2() {
	if(NULL==fp_db2) return 0;

	fseek(fp_db2,0,SEEK_SET);
	fread(&head_db2,sizeof(Head_db2),1,fp_db2);
	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::rewrite_head() {
	if(NULL==fp_db1 || NULL==fp_db2) return 0;

	fseek(fp_db1,0,SEEK_SET);
	fwrite(&head_db1,sizeof(Head_db1),1,fp_db1);

	fseek(fp_db2,0,SEEK_SET);
	fwrite(&head_db2,sizeof(Head_db2),1,fp_db2);

	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::write_head_db1(unsigned int key_num,unsigned int key_size,unsigned int point_size) {
	if(NULL==fp_db1) return 0;

	head_db1.node_num=0;
	head_db1.node_size=betterbase2(4+key_size*key_num+point_size*(key_num+1));
	head_db1.key_num=key_num;
	head_db1.key_size=key_size;
	head_db1.point_size=point_size;
	head_db1.data_start=sizeof(Head_db1);
	head_db1.root_pos=0;
	head_db1.empty_pos=-1;

	fseek(fp_db1,0,SEEK_SET);
	fwrite(&head_db1,sizeof(Head_db1),1,fp_db1);

	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::write_head_db2(unsigned int data_size) {
	if(NULL==fp_db2) return 0;

	head_db2.node_num=0;
	head_db2.node_size=betterbase2(data_size);
	head_db2.data_size=data_size;
	head_db2.data_start=sizeof(Head_db2);
	head_db2.empty_pos=-1;

	fseek(fp_db2,0,SEEK_SET);
	fwrite(&head_db2,sizeof(Head_db2),1,fp_db2);

	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::open(const char * name,const char * path) {
	clear();

	string t;
	if(path) t=string(path);

	fp_db1=fopen((t+name+".db1").data(),"r+");
	fp_db2=fopen((t+name+".db2").data(),"r+");

	if(NULL==fp_db1 || NULL==fp_db2) {
		clear();
		return 0;
	}

	read_head_db1();
	read_head_db2();

	if(head_db1.key_size!=sizeof(T_key)) {
		clear();
		return 0;
	}
	if(head_db2.data_size!=sizeof(T_val)) {
		clear();
		return 0;
	}

	buf1=new char [head_db1.node_size];
	buf2=new char [head_db2.node_size];

	if(NULL==buf1 || NULL==buf2) {
		clear();
		return 0;
	}

	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::create(const char * name,const char * path,unsigned int key_num) {
	clear();

	string t;
	if(path) t=string(path);

	fp_db1=fopen((t+name+".db1").data(),"w+");
	fp_db2=fopen((t+name+".db2").data(),"w+");

	if(NULL==fp_db1 || NULL==fp_db2) {
		clear();
		return 0;
	}

	write_head_db1(key_num,sizeof(T_key),4);		// default point size is 4.
	write_head_db2(sizeof(T_val));

	buf1=new char [head_db1.node_size];
	buf2=new char [head_db2.node_size];

	if(NULL==buf1 || NULL==buf2) {
		clear();
		return 0;
	}

	return 1;
}

template <typename T_key,typename T_val>
int BTree<T_key,T_val>::find(const T_key & key) {
	if(!check()) return -1;
	if(head_db1.node_num==0) return -1;

	unsigned int p=head_db1.root_pos,num,t=0;

	while(!(p & (1<<31))) {			// not leaf.
		fseek(fp_db1,head_db1.data_start+p*head_db1.node_size,SEEK_SET);
		fread(buf1,head_db1.node_size,1,fp_db1);

		memcpy(&num,buf1,4);
		buf1+=4;

		t=upper_bound((T_key *)buf1,((T_key *)buf1)+num,key)-(T_key *)buf1;

		memcpy(&p,buf1+head_db1.key_size*head_db1.key_num+head_db1.point_size*p,4);
	}

	p^=(1<<31);
	// Find the pos.
	if(t<num && (!(key<((T_key *)buf1)[t])))
		return p;

	return -1;
}

template <typename T_key,typename T_val>
int BTree<T_key,T_val>::find_trace(const T_key & key,stack <int> & sta) {
	if(!check()) return -1;
	if(head_db1.node_num==0) return -1;

	while(!sta.empty()) sta.pop();
	unsigned int p=head_db1.root_pos,num,t=0;

	while(!(p & (1<<31))) {
		sta.push(p);

		fseek(fp_db1,head_db1.data_start+p*head_db1.node_size,SEEK_SET);
		fread(buf1,head_db1.node_size,1,fp_db1);

		memcpy(&num,buf1,4);
		buf1+=4;

		t=upper_bound((T_key *)buf1,((T_key *)buf1)+num,key)-(T_key *)buf1;

		memcpy(&p,buf1+head_db1.key_size*head_db1.key_num+head_db1.point_size*p,4);
	}

	if(t<num && (!(key<((T_key *)buf1)[t]))) return 1;
	return 0;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::find(const T_key & key,T_val & res) {
	int p=find(key);

	if(p==-1) return 0;

	fseek(fp_db2,head_db2.data_start+p*head_db2.node_size,SEEK_SET);
	fread(buf2,head_db2.node_size,1,fp_db2);
	memcpy(&res,buf1,head_db2.data_size);

	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::insert(const T_key & key,const T_val & val) {
	if(!check()) return 0;

	if(head_db1.node_num==0) {
		head_db1.root_pos=new_node_db1();
		head_db1.node_num=1;
	}

	stack <int> sta;
	if(find_trace(key,sta)!=0) return 0;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::update(const T_key & key,const T_val & val) {
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::erase(const T_key & key) {
}

int main() {
	BTree <int,int> tree;
	int a;

	cout<<tree.open("aaa")<<endl;
	tree.show();

	return 0;
}

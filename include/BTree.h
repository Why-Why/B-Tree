/*
   B+Tree class.
*/

#ifndef BTREE_H
#define BTREE_H

#include <cstdio>
#include <stack>

#include <iostream>
using namespace std;

using std::stack;

// The head of name.db1,32Byte,see Tree.md for more detail.
struct Head_db1 {
	unsigned int node_num;
	unsigned int node_size;
	unsigned int key_num;
	unsigned int key_size;
	unsigned int point_size;
	unsigned int data_start;
	unsigned int root_pos;
	int empty_pos;
};

// The head of name.db2,32Byte,see Tree.md for more detail.
struct Head_db2 {
	unsigned int node_num;
	unsigned int node_size;
	unsigned int data_size;
	unsigned int data_start;
	int empty_pos;
	char future[12];
};

// Remember that T_key should have < operator.
template <typename T_key,typename T_val>
class BTree {
	private:
		FILE * fp_db1, * fp_db2;
		Head_db1 head_db1;
		Head_db2 head_db2;
		char * buf1, * buf2;

	private:
		// Check whether this class can be insert,find or delete.
		bool check();
		// return a pow of 2 which is min number that is no less than x.
		long long betterbase2(int x);

		bool read_head_db1();
		bool read_head_db2();
		bool write_head_db1(unsigned int key_num,unsigned int key_size,unsigned int point_size);
		bool write_head_db2(unsigned int data_size);
		bool rewrite_head();

		// return the pos of data, -1 for no.
		int find(const T_key & key);
		// for insert and erase and update, 1 for have a same one, 0 for not, -1 for wrong.
		int find_trace(const T_key & key,stack <int> & sta);

	public:

	public:
		BTree();
		~BTree();

		// Open a database file, name without extension.
		bool open(const char * name,const char * path=NULL);
		bool create(const char * name,const char * path=NULL,unsigned int key_num=511);

		// Clear the FILE pointer,delete the buf pointer.
		void clear();

		// find the answer with the same key,save the answer to res. return 1 for yes, 0 for no.
		bool find(const T_key & key,T_val & res);
		// if exist the same key, return 0.
		bool insert(const T_key & key,const T_val & val);
		// if not exist the same key, return 0.
		bool update(const T_key & key,const T_val & val);
		// if not exist the same key, return 0.
		bool erase(const T_key & key);

		void show() {
			cout<<head_db2.node_num<<' '<<head_db2.node_size<<' '<<head_db2.data_size<<' '<<head_db2.data_start<<' '<<head_db2.empty_pos<<endl;
		}
};

#endif

/*
   B+Tree class.
*/

#ifndef BTREE_H
#define BTREE_H

#include <cstdio>

#include "Base.h"
#include "Store.h"
#include "Node.h"

#include <iostream>
using namespace std;

// Remember that T_key should have < operator.
template <typename T_key,typename T_val>
class BTree {
	private:
		Store<T_key,T_val> store;
		Head1 head;
		bool no;

	private:

		// return the pos of data, -1 for not find, -2 for error; leaf_pos is the leaf node that should be insert.
		int find(const T_key & key,Node<T_key> & leaf_node,int & use);

		// insert to the node with the key, and a new pointer point.
		bool insert_leaf(Node<T_key> & node1,const T_key & key,Point point);
		bool insert_parent(Node<T_key> & node1,const T_key & key,Node<T_key> & node2);

	public:

	public:
		BTree();
		~BTree();

		// Open a database file, name without extension.
		bool open(const char * name,const char * path=NULL);
		bool create(const char * name,const char * path=NULL,int key_num=510);

		// find the answer with the same key,save the answer to res. return 1 for yes, 0 for no.
		bool find(const T_key & key,T_val & res);

		// if exist the same key, return 0.
		bool insert(const T_key & key,const T_val & val);

		// if not exist the same key, return 0.
		bool erase(const T_key & key);

		void show(int p) {
			cerr<<"&&&&&&&&&\n";
			cerr<<p<<endl;
			Node<T_key> node(head.key_num);
			store.get_node1(node,p);

			if(node.leaf) {
				cerr<<"Leaf "<<node.num<<' '<<node.father<<endl;
				for(int i=0;i<node.num;++i)
					cerr<<node.key[i]<<' '<<node.point[i]<<endl;
				cerr<<"# "<<node.point[node.num]<<endl;
			}
			else {
				cerr<<"No leaf "<<node.num<<' '<<node.father<<endl;
				for(int i=0;i<node.num;++i)
					cerr<<node.key[i]<<' '<<node.point[i]<<endl;
				cerr<<"#  "<<node.point[node.num]<<endl;

				for(int i=0;i<=node.num;++i) show(node.point[i]);
			}
			cerr<<"EEEEEEEEEE\n";
		}

		void show() {
			cerr<<"TREE TREE TREE TREE TREE TREE\n";
			cerr<<head.root_pos<<endl;
			show(head.root_pos);
			cerr<<"END END END END END END END END\n\n";
		}
};

///////////////////////////////////////////////////////////

template <typename T_key,typename T_val>
BTree<T_key,T_val>::BTree() {
	no=1;
	memset(&head,0,sizeof(head));
}

template <typename T_key,typename T_val>
BTree<T_key,T_val>::~BTree() {
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::open(const char * name,const char * path) {
	if(0==store.open(name,path)) { no=1; return 0; }
	if(0==store.get_head1(head)) { no=1; return 0; }
	no=0;
	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::create(const char * name,const char * path,int key_num) {
	if(0==store.create(name,path,key_num)) { no=1; return 0; }
	if(0==store.get_head1(head)) { no=1; return 0; }
	no=0;
	return 1;
}

template <typename T_key,typename T_val>
int BTree<T_key,T_val>::find(const T_key & key,Node<T_key> & leaf_node,int & use) {
	use=0;
	if(no) return -2;

	int p=head.root_pos,t=0,last=-1;
	Node<T_key> temp(head.key_num);

	while(1) {
		if(0==store.get_node1(temp,p)) return -2;
		temp.father=last;
		store.set_node1(temp,p);

		if(temp.leaf) {			// !!!
			t=temp.lower_bound(key);
			leaf_node=temp;			// Maybe wrong.
		}
		else
			t=temp.upper_bound(key);

		last=p;
		p=temp.point[t];

		if(temp.leaf) break;
	}

	// Find the pos.
	if(t<temp.num && (!(key<temp.key[t]))) {
		if(p<0) use=0,p=-(p+1);
		else use=1;
		return p;
	}

	return -1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::find(const T_key & key,T_val & res) {
	if(no) return 0;

	Node<T_key> t(head.key_num);
	int use;
	int p=find(key,t,use);

	if(p<0 || 0==use) return 0;
	if(0==store.get_node2(res,p)) return 0;

	return 1;
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::insert_leaf(Node<T_key> & node1,const T_key & key,Point point) {
	if(node1.num<head.key_num) {
		node1.insert(key,point);
		return store.set_node1(node1,node1.id);
	}
	else {
		Node<T_key> node2(head.key_num);
		node1.insert(key,point);

		int len=(head.key_num+1)/2;
		node1.cut(0,len,node2);
		node2.set(len,key,node1.id);
		node2.id=store.new_node1(node2);			// !!!

		if(node2.id<0) return 0;

		if(0==insert_parent(node2,node1.key[0],node1)) return 0;
		if(0==store.set_node1(node1,node1.id)) return 0;		// parent may be changed.
		if(0==store.set_node1(node2,node2.id)) return 0;

		return 1;
	}
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::insert_parent(Node<T_key> & node1,const T_key & key,Node<T_key> & node2) {
	if(-1==node1.father) {			// root.
		Node<T_key> node(head.key_num);
		node.leaf=0;				// can be change through a function.
		node.father=-1;
		node.insert(key,node1.id);
		node.point[1]=node2.id;

		node.id=store.new_node1(node);
		if(node.id<0) return 0;

		node1.father=node2.father=node.id;

		head.root_pos=node.id;
		store.set_head1(head);

		return 1;
	}

	Node<T_key> node(head.key_num);
	store.get_node1(node,node1.father);

	if(node.num<head.key_num) {
		node.insert(key,node1.id);			// maybe wrong.
		return store.set_node1(node,node.id);
	}
	else {
		Node<T_key> r2(head.key_num);
		node.insert(key,node1.id);

		int len=(head.key_num+1)/2+1;
		node.cut(0,len,r2);

		T_key kk=r2.key[r2.num-1];			// !!!
		r2.pop();

		r2.id=store.new_node1(r2);
		if(r2.id<0) return 0;

		if(0==insert_parent(r2,kk,node)) return 0;
		if(0==store.set_node1(node,node.id)) return 0;
		if(0==store.set_node1(r2,r2.id)) return 0;

		return 1;
	}
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::insert(const T_key & key,const T_val & val) {
	if(no) return 0;

	Node<T_key> leaf_node(head.key_num);
	int use,t;
	t=find(key,leaf_node,use);
	if(-2==t || 1==use) return 0;		// have a same one or wrong.

	if(-1==t) {
		int p=store.new_node2(val);
		if(p<0) return 0;

		return insert_leaf(leaf_node,key,p);
	}
	else {
		if(0==store.set_node2(val,t)) return 0;
		int tt=leaf_node.lower_bound(key);
		leaf_node.point[tt]=t;
		store.set_node1(leaf_node,leaf_node.id);
		return 1;
	}
}

template <typename T_key,typename T_val>
bool BTree<T_key,T_val>::erase(const T_key & key) {
	if(no) return 0;

	Node<T_key> leaf_node(head.key_num);
	int use,t;
	t=find(key,leaf_node,use);
	if(t<0 || 0==use) return 0;

	int tt=leaf_node.lower_bound(key);
	leaf_node.point[tt]=-t-1;
	store.set_node1(leaf_node,leaf_node.id);

	return 1;
}

#endif

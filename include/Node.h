/*
	Node class.
*/

#ifndef NODE_H
#define NODE_H

#include "Base.h"

//template <typename T_key,typename T_val>
//class Store;

template <typename T_key>
class Node {
//	template <typename T1,typename T2> friend class Store;			// !!!

	public:
		int size;
		int id;

		int num;
		int leaf;
		int father;
		T_key * key;
		Point * point;

	public:
		Node(int N);
		~Node();

		Node(const Node<T_key> & x);
		Node<T_key> & operator = (const Node<T_key> & x);

		void set(int pos,const T_key & k,Point p);

		void insert(const T_key & k,Point p);
		// cut from [pos,pos+len) to node[0,len). to create a new node.
		void cut(int pos,int len,Node<T_key> & node);
		void pop();

		int lower_bound(const T_key & key);
		int upper_bound(const T_key & key);
};

/////////////////////////////////////////////////

template <typename T_key>
Node<T_key>::Node(int N) {
	size=N;

	id=num=leaf=father=0;
	key=new T_key [N+2];
	point=new Point [N+2];
}

template <typename T_key>
Node<T_key>::~Node() {
	if(key) delete [] key;
	if(point) delete [] point;
}

template <typename T_key>
Node<T_key>::Node(const Node<T_key> & x) {
	id=x.id; num=x.num; leaf=x.leaf; father=x.father;
	memcpy(key,x.key,sizeof(T_key)*(size+2));
	memcpy(point,x.point,sizeof(T_key)*(size+2));
}

template <typename T_key>
Node<T_key> & Node<T_key>::operator = (const Node<T_key> & x) {
	id=x.id; num=x.num; leaf=x.leaf; father=x.father;
	memcpy(key,x.key,sizeof(T_key)*(size+2));
	memcpy(point,x.point,sizeof(T_key)*(size+2));

	return *this;
}

template <typename T_key>
void Node<T_key>::set(int pos,const T_key & k,Point p) {
	key[pos]=k;
	point[pos]=p;
}

template <typename T_key>
void Node<T_key>::insert(const T_key & k,Point p) {
//	point[num+1]=point[num];
	++num;
	point[num]=point[num-1];

	for(int i=num-1;i>=1;--i)
		if(key[i-1]<k) {
			key[i]=k;
			point[i]=p;
			return;
		}
		else {
			key[i]=key[i-1];
			point[i]=point[i-1];
		}
	key[0]=k;
	point[0]=p;
}

template <typename T_key>
void Node<T_key>::cut(int pos,int len,Node<T_key> & node) {
	int t=pos;
	int to_pos=0;
	for(int i=0;i<len;++i,++pos,++to_pos) {
		node.key[to_pos]=key[pos];
		node.point[to_pos]=point[pos];
	}
	for(;pos<=num;++pos,++t) {
		key[t]=key[pos];
		point[t]=point[pos];
	}
	num-=len;
	node.num=len;
	node.leaf=leaf;
	node.father=father;
}

template <typename T_key>
void Node<T_key>::pop() {
	--num;
}

template <typename T_key>
int Node<T_key>::lower_bound(const T_key & k) {
	if(0==num) return 0;
	return std::lower_bound(key,key+num,k)-key;
}

template <typename T_key>
int Node<T_key>::upper_bound(const T_key & k) {
	if(0==num) return 0;
	return std::upper_bound(key,key+num,k)-key;
}

#endif

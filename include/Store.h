/*
	Store class.
*/

#ifndef STORE_H
#define STORE_H

#include "Base.h"
#include "Node.h"

// The head of name.db1,32Byte,see Tree.md for more detail.
struct Head1_Store {
	int node_num;
	int node_size;
	int key_num;
	int key_size;
	int point_size;
	int data_start;
	int root_pos;
	int empty_pos;
};

// The head of name.db2,32Byte,see Tree.md for more detail.
struct Head2_Store {
	int node_num;
	int node_size;
	int data_size;
	int data_start;
	int empty_pos;
	char future[12];
};

template <typename T_key,typename T_val>
class Store {
	private:
		FILE * fp_db1, * fp_db2;
		char * buf1;
		Head1_Store head1;
		Head2_Store head2;

	private:
		void clear();			// clear fp and buf1.
		bool check();			// judge whether fp and buf1 is ok.
		int biggerBase2(int x);		// return an integer y where y=pow(2,N) and y>=x.			// Maybe more than int !!!

		bool read_head1();
		bool read_head2();
		bool write_head1();
		bool write_head2();

	public:
		Store();
		~Store();

		// Open a database file, name without extension.
		bool open(const char * name,const char * path=NULL);
		bool create(const char * name,const char * path=NULL,int key_num=510);

		bool get_head1(Head1 & res);
		bool set_head1(const Head1 & res);

		bool get_node1(Node<T_key> & res,Point p);
		bool get_node2(T_val & res,Point p);
		bool set_node1(const Node<T_key> & res,Point p);
		bool set_node2(const T_val & res,Point p);

		int new_node1(const Node<T_key> & res);
		int new_node2(const T_val & res);
};

///////////////////////////////////////////////////

template <typename T_key,typename T_val>
Store<T_key,T_val>::Store() {
	fp_db1=fp_db2=NULL;
	memset(&head1,0,sizeof(head1));
	memset(&head2,0,sizeof(head2));
	buf1=NULL;
}

template <typename T_key,typename T_val>
Store<T_key,T_val>::~Store() {
	write_head1();
	write_head2();
	clear();
}

template <typename T_key,typename T_val>
void Store<T_key,T_val>::clear() {
	if(fp_db1) fclose(fp_db1);
	if(fp_db2) fclose(fp_db2);

	if(NULL!=buf1) delete [] buf1;

	memset(&head1,0,sizeof(head1));
	memset(&head2,0,sizeof(head2));
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::check() {
	if(fp_db1 && fp_db2 && buf1) return 1;
	return 0;
}

template <typename T_key,typename T_val>
int Store<T_key,T_val>::biggerBase2(int x) {
	for(long long ret=1;;ret<<=1)
		if(ret>=x) return (int)ret;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::read_head1() {
	if(NULL==fp_db1) return 0;

	fseek(fp_db1,0,SEEK_SET);
	fread(&head1,sizeof(Head1_Store),1,fp_db1);
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::read_head2() {
	if(NULL==fp_db2) return 0;

	fseek(fp_db2,0,SEEK_SET);
	fread(&head2,sizeof(Head2_Store),1,fp_db2);
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::write_head1() {
	if(NULL==fp_db1) return 0;			// !!!

	fseek(fp_db1,0,SEEK_SET);
	fwrite(&head1,sizeof(Head1_Store),1,fp_db1);
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::write_head2() {
	if(NULL==fp_db2) return 0;

	fseek(fp_db2,0,SEEK_SET);
	fwrite(&head2,sizeof(Head2_Store),1,fp_db2);
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::open(const char * name,const char * path) {
	clear();

	using std::string;

	string t;
	if(path) t=string(path);

	fp_db1=fopen((t+name+".db1").data(),"r+");
	fp_db2=fopen((t+name+".db2").data(),"r+");

	if(NULL==fp_db1 || NULL==fp_db2) {
		clear();
		return 0;
	}

	read_head1();
	read_head2();

	if(head1.key_size!=sizeof(T_key)) {
		clear();
		return 0;
	}
	if(head2.data_size!=sizeof(T_val)) {
		clear();
		return 0;
	}

	buf1=new char [head1.node_size];

	if(NULL==buf1) {
		clear();
		return 0;
	}

	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::create(const char * name,const char * path,int key_num) {
	clear();

	using std::string;

	string t;
	if(path) t=string(path);

	fp_db1=fopen((t+name+".db1").data(),"w+");
	fp_db2=fopen((t+name+".db2").data(),"w+");

	if(NULL==fp_db1 || NULL==fp_db2) {
		clear();
		return 0;
	}

	int key_size=sizeof(T_key),point_size=sizeof(Point);
	head1.node_num=0;
	head1.node_size=biggerBase2(12+key_size*key_num+point_size*(key_num+1));		// !!!
	head1.key_num=key_num;
	head1.key_size=key_size;
	head1.point_size=point_size;
	head1.data_start=sizeof(Head1_Store);
	head1.root_pos=0;			// !!!
	head1.empty_pos=-1;
	write_head1();

	head2.node_num=0;
	head2.node_size=biggerBase2(sizeof(T_val));
	head2.data_size=sizeof(T_val);
	head2.data_start=sizeof(Head2_Store);
	head2.empty_pos=-1;
	write_head2();

	buf1=new char [head1.node_size];

	if(NULL==buf1) {
		clear();
		return 0;
	}

	Node<T_key> node(key_num);
	node.num=0;
	node.leaf=1;
	node.father=-1;
	node.point[0]=-1;
	if(new_node1(node)<0) return 0;

	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::get_head1(Head1 & res) {
	if(0==check()) return 0;

	res.key_num=head1.key_num;
	res.root_pos=head1.root_pos;
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::set_head1(const Head1 & res) {
	if(0==check()) return 0;

	//head1.key_num=res.key_num;
	head1.root_pos=res.root_pos;
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::get_node1(Node<T_key> & res,Point p) {
	if(0==check()) return 0;

	if(fseek(fp_db1,head1.data_start+p*head1.node_size,SEEK_SET)) return 0;
	fread(buf1,head1.node_size,1,fp_db1);

	res.id=p;
	res.size=head1.key_num;

	memcpy(&res.num,buf1,4);
	memcpy(&res.leaf,buf1+4,4);
	memcpy(&res.father,buf1+8,4);

	memcpy(res.key,buf1+12,head1.key_num*head1.key_size);
	memcpy(res.point,buf1+12+head1.key_size*head1.key_num,(head1.key_num+1)*head1.point_size);
	
	return 1;	
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::get_node2(T_val & res,Point p) {
	if(0==check()) return 0;

	if(fseek(fp_db2,head2.data_start+p*head2.node_size,SEEK_SET)) return 0;
	fread(&res,head2.node_size,1,fp_db2);
	return 1;
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::set_node1(const Node<T_key> & res,Point p) {
	if(0==check()) return 0;

	memcpy(buf1,&res.num,4);
	memcpy(buf1+4,&res.leaf,4);
	memcpy(buf1+8,&res.father,4);

	memcpy(buf1+12,res.key,head1.key_num*head1.key_size);
	memcpy(buf1+12+head1.key_size*head1.key_num,res.point,(head1.key_num+1)*head1.point_size);

	if(fseek(fp_db1,head1.data_start+p*head1.node_size,SEEK_SET)) return 0;
	fwrite(buf1,head1.node_size,1,fp_db1);
}

template <typename T_key,typename T_val>
bool Store<T_key,T_val>::set_node2(const T_val & res,Point p) {
	if(0==check()) return 0;

	if(fseek(fp_db2,head2.data_start+p*head2.node_size,SEEK_SET)) return 0;
	fwrite(&res,head2.node_size,1,fp_db2);
	return 1;
}

template <typename T_key,typename T_val>
int Store<T_key,T_val>::new_node1(const Node<T_key> & res) {
	if(0==check()) return -1;

	memcpy(buf1,&res.num,4);
	memcpy(buf1+4,&res.leaf,4);
	memcpy(buf1+8,&res.father,4);

	memcpy(buf1+12,res.key,res.num*head1.key_size);
	memcpy(buf1+12+head1.key_size*head1.key_num,res.point,(res.num+1)*head1.point_size);

	int ret;
	++head1.node_num;

	if(-1==head1.empty_pos) {
		if(fseek(fp_db1,0,SEEK_END)) return -1;
		ret=head1.node_num-1;
	}
	else {
		if(fseek(fp_db1,head1.data_start+head1.empty_pos*head1.node_size,SEEK_SET)) return -1;
		ret=head1.empty_pos;

		Point temp;
		fread(&temp,head1.point_size,1,fp_db1);

		if(fseek(fp_db1,-head1.point_size,SEEK_CUR)) return -1;
		head1.empty_pos=temp;
	}
	fwrite(buf1,head1.node_size,1,fp_db1);

	return ret;
}

template <typename T_key,typename T_val>
int Store<T_key,T_val>::new_node2(const T_val & res) {
	if(0==check()) return -1;

	int ret;
	++head2.node_num;

	if(-1==head2.empty_pos) {
		if(fseek(fp_db2,0,SEEK_END)) return -1;
		ret=head2.node_num-1;
	}
	else {
		if(fseek(fp_db2,head2.data_start+head2.empty_pos*head2.node_size,SEEK_SET)) return -1;
		ret=head2.empty_pos;

		Point temp;
		fread(&temp,head1.point_size,1,fp_db2);

		if(fseek(fp_db2,-head1.point_size,SEEK_CUR)) return -1;
		head2.empty_pos=temp;
	}
	fwrite(&res,head2.node_size,1,fp_db2);

	return ret;
}

#endif

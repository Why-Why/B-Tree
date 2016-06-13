/*
	contain the base struct.
*/

#ifndef BASE_H
#define BASE_H

#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>

using std::lower_bound;
using std::upper_bound;

#include <iostream>
using namespace std;

// Base const num.
const int head_size=32;

// The type of Point in B+Tree.
typedef int Point;

// See Store.h head for more detail.
struct Head1 {
	int key_num;
	int root_pos;
};

#endif

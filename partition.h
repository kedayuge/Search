#pragma once
#include <vector>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <numeric>
#include <math.h>
#include "hash.h"
#include "minsearch.h"
using namespace std;

struct Node{
	string str;
	int s, e, l;
	vector<Node*> child;
};


map<int, vector<pair<string, int>>, greater<int>> partition(string& s, int input_r = 1);
Node* partitiontree(string& s, int input_r = 1);


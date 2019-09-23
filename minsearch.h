#pragma once
#include <vector> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <algorithm> 
#include <tuple>
#include <omp.h>
#include <numeric> 
#include <math.h>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>
#include <map>
#include <climits>
#include "hash.h"
#include "edit.h"
#include "timerec.h"
#include "partition.h"
#include "filter.h"
#include "match.h"
using namespace std;

extern int q;
extern int countbit;
extern string qtype;

struct compare {
	bool operator()(const string &s1, const string &s2) {
  		if (s1.length() < s2.length()) return true;
  		else if (s1.length() > s2.length()) return false;
  		return s1 < s2;
	}
};

static bool paircmp(const pair<int, int>& x,const pair<int, int> y)
{
	return x.second < y.second;
}

class pqcmp
{
public: 
	bool operator() (pair<int, int> & a, pair<int, int> & b)
	{
		return a.first < b.first;
	}
};


int main(int argc, char **argv);
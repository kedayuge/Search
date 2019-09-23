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

vector<int> calsig(string &s);

bool countfilter(vector<int>& x, vector<int>& y, int ed);

bool lengthfilter(string& x, string& y, int posx, int posy, int min_ed);

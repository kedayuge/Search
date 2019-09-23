#include "partition.h"

int min_r = 1;

vector<pair<int, int>> findAnchorAllK(string& s, vector<size_t>& value)
{
	vector<pair<int, int>> anchors;
	anchors.push_back(make_pair(0, INT_MAX));
	for (int i = min_r; i + min_r + q - 1 < s.size();)
	{
		int count = 1; 
		while(i + count + q - 1 < s.size() && i - count >= 0)
		{
			if(value[i] >= value[i + count + q - 1] || value[i] >= value[i - count] )
				break;
			++count;
		}
		if(count - 1 >= min_r)
		{
			anchors.push_back(make_pair(i, count));
		}
		i += count; 
	}
	anchors.push_back(make_pair(s.size(), INT_MAX));
	return anchors;
}

void addElement(string &s, vector<pair<int, int>>& anchors,  map<int, vector<pair<string, int>>, greater<int>>& output, int start, int end, unordered_set<int>& set)
{
	if(start < 0 || end < 0 || start >= anchors.size() || end >= anchors.size() || start >= end)
		return;
	int l = anchors[end].first - anchors[start].first, k = anchors[start].second;//min(anchors[start].second, anchors[end].second);
	if(l > 6 || dictsize > 26)
		output[k].push_back(make_pair(s.substr(anchors[start].first, l), anchors[start].first));	
	else if(qtype =="-k" && l >= 3 && s.size() - anchors[start].first >= 6 && set.find(anchors[start].first) == set.end())
	{
		set.insert(anchors[start].first);
		output[k].push_back(make_pair(s.substr(anchors[start].first, 6), anchors[start].first));
	}
}

void Anchor2Part(string &s, vector<pair<int, int>>& anchors,  map<int, vector<pair<string, int>>, greater<int>>& output, int start, int end, unordered_set<int>& set)
{
	if(start >= end - 1)// || anchors[end].first - anchors[start].first  < min_l)
		return;
	// at least 3 elements
	list<int> pos;	
	for(int i = start + 1; i < end; ++i)
		if(pos.size() == 0 || anchors[i].second > anchors[pos.front()].second)
		{
			pos.clear();
			pos.push_back(i);
		}
		else if(anchors[i].second == anchors[pos.front()].second)
			pos.push_back(i);
	
	pos.push_front(start);
	pos.push_back(end);
	for(auto it = pos.begin(), next = it; next != pos.end(); ++it, ++next)
	{
		if(next == it)
			++next;
		addElement(s, anchors, output,  *it, *next, set);
		Anchor2Part(s, anchors, output,  *it, *next, set);
	}
}

map<int, vector<pair<string, int>>, greater<int>> partition(string& s, int input_r)
{
	min_r = input_r;
	unordered_set<int> set;
	map<int, vector<pair<string, int>>, greater<int>> output;
	vector<size_t> value = hashstring(s);
	vector<pair<int, int>> anchors = findAnchorAllK(s, value);	
	Anchor2Part(s, anchors, output, 0, anchors.size() - 1, set);
	return output;
}


Node* Anchor2Parttree(string &s, vector<pair<int, int>>& anchors, int start, int end)
{
	if(start >= end - 1 || start < 0 || end < 0 || start >= anchors.size() || end >= anchors.size())
		return NULL;
	list<int> pos;	
	for(int i = start + 1; i < end; ++i)
		if(pos.size() == 0 || anchors[i].second > anchors[pos.front()].second)
		{
			pos.clear();
			pos.push_back(i);
		}
		else if(anchors[i].second == anchors[pos.front()].second)
			pos.push_back(i);
	
	Node* output = new Node;
	output->l = anchors[start].second, output->s = anchors[start].first, output->e = anchors[end].first, output->str = s.substr(anchors[start].first, anchors[end].first - anchors[start].first);

	pos.push_front(start);
	pos.push_back(end);
	for(auto it = pos.begin(), next = it; next != pos.end(); ++it, ++next)
	{
		if(next == it)
			++next;
		Node* newchild = Anchor2Parttree(s, anchors,  *it, *next);
		if(newchild)
			output->child.push_back(newchild);
	}
	return output;
}

Node* partitiontree(string& s, int input_r)
{
	min_r = input_r;
	vector<size_t> value = hashstring(s);
	vector<pair<int, int>> anchors = findAnchorAllK(s, value);
	return Anchor2Parttree(s, anchors, 0, anchors.size() - 1);
}
#include "hash.h"
#include "minsearch.h"

int hashbit;
int hashmask;

int dictsize = 37;
unordered_map<char, int> dict;
int maxrange = 1;

void builddict()
{
	if (dictsize == 5)
	{
		unordered_map<char, int> dict1({ { 'A', 0 }, { 'C', 1 }, { 'G', 2 }, { 'T', 3 }, { 'N', 4 } });
		dict = dict1;
	}
	else if (dictsize == 26)
	{
		for (int i = 0; i < 26; ++i)
		{
			dict['A' + i] = i;
			dict['a' + i] = i;
		}
	}
	else if(dictsize == 32)
	{
		for (int i = 0; i < 26; ++i)
		{
			dict['A' + i] = i;
			dict['a' + i] = i;
		}
		dict[' '] = 26;
		dict['.'] = 27;
		dict[':'] = 28;
		dict['+'] = 29;
		dict[','] = 30;
		dict['&'] = 31;
	}
	else
	{
		for (int i = 0; i < 26; ++i)
		{
			dict['A' + i] = i;
			dict['a' + i] = i;
		}
		for (int i = 0; i < 10; ++i)
			dict['0' + i] = i + 26;
		dict[' '] = 36;
	}
}

void initiHash()
{
	hashbit = ceil(log2(dictsize));
	hashmask = 0;
	for (int i = 0; i < hashbit; ++i)
	{
		++hashmask;
		hashmask <<= 1;
	}
	hashmask <<= (hashbit * (q - 1));
	hashmask = ~hashmask;

	builddict();
}

vector<size_t> hashstring(string &s)
{
	vector<size_t> output;
	hash<string> hash_fn;
	unordered_set<int> pre;
	int firstindex = 0;	
	for (int i = 0; i < s.size() - q + 1; ++i)
	{
		//output.push_back(hash_fn(s.substr(i, q))); continue;
		int value, len = q;
		string cur = s.substr(i, q);
		while(i < s.size() - len + 1 && pre.find(value = hash_fn(cur)) != pre.end())
		{
			cur += s[i + len - 1];	
			++len;	
		}
		output.push_back(value);

		while(len == q && pre.size() >= maxrange)
			pre.erase(output[firstindex++]);
		pre.insert(value);
	}
	return output;
}

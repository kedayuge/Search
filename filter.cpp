#include "filter.h"

vector<int> calsig(string &s)
{
	vector<int> output(pow(dictsize, countbit), 0);
 	int i = 0, sum = 0;
	while(i < countbit && i < s.size())
		sum = sum * dictsize + dict[s[i++]];
	++output[sum];
	while(i < s.size())
	{
		sum = (sum - dict[s[i - countbit]] * pow(dictsize, countbit - 1))*dictsize + dict[s[i]];
		++output[sum];
		++i;
	}
	return output;
}

bool countfilter(vector<int>& x, vector<int>& y, int ed)
{
	int count = 0;
	for(int i = 0; i < x.size(); ++i)
		count += abs(x[i] - y[i]);
	return count <= 2*countbit*ed;
}

bool lengthfilter(string& x, string& y, int posx, int posy, int min_ed)
{
	if (abs((int)x.size() - (int)y.size()) > min_ed)
		return false;
	if ((abs(posx - posy) + abs((int)x.size() - posx - (int)y.size() + posy)) > min_ed)
		return false;
	return true;
}


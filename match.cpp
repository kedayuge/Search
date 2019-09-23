#include "match.h"

struct match
{
	int pos1, pos2, len1, len2, level;
};

static bool matchcmp(const match& x, const match& y)
{
	return x.pos1 < y.pos1;
}

static bool matchunique(const match& x, const match& y)
{
	return x.pos1 == y.pos1 && x.pos2 == y.pos2;
}


void extend(match& m, string& x, string& y)
{
	int s1 = m.pos1, s2 = m.pos2, len1= m.len1, len2 = m.len2;
	while(s1 + len1 < x.size() && s2 + len2 < y.size() && x[s1 + len1] == y[s2 + len2])
		++len1, ++len2;
	while(s1 - 1 >= 0 && s2 - 1 >= 0 && x[s1 - 1] == y[s2 - 1])
		--s1, --s2, ++len1, ++len2;
	m.len1 = len1, m.len2 = len2, m.pos1 = s1, m.pos2 = s2;
}

bool oneedit(const string& a,const string &b)
{
    if (!a.size()) return b.size() == 1;
    if (!b.size()) return a.size() == 1;
    if (abs((int)a.size() - (int)b.size()) > 1) return false;
    if (a.size() == b.size()) {
        bool flag = false;
        for (int i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                if (flag) return false;
                flag = true;
            }
        }
        return true;
    } else {
        for (int i = 0; i < min(a.size(), b.size()); ++i) {
            if (a[i] != b[i]) {
                if (a.size() > b.size()) {
                    return a.substr(i+1) == b.substr(i);
                } else {
                    return a.substr(i) == b.substr(i + 1);
                }
            }
        }
    }
}

void bfstree(Node* root, map<int, vector<pair<string, int>>, greater<int>>& par, vector<match>& matches, string& x, string& y)
{
	
	unordered_map<int, unordered_map<string, vector<int>>> map;
	for(auto &x: par)
		for(auto &y:x.second)
			map[x.first][y.first].push_back(y.second);

	if(!root)
		return;
	int level = 1;
	queue<Node*> queue;
	queue.push(root);
	while(queue.size())
	{
		int size = queue.size();
		for(int i = 0; i < size; ++i)
		{
			Node* cur = queue.front();
			queue.pop();
			
			//if(map[cur->l].find(cur->str) != map[cur->l].end())
			//for(auto& pos :map[cur->l][cur->str])

			bool tmp = true;
			for(auto & sig:map[cur->l])
				if(oneedit(sig.first, cur->str))
				for(auto& pos :map[cur->l][sig.first])
				{
					tmp = false;
					match newmatch;
					newmatch.pos1 = cur->s, newmatch.pos2 = pos, newmatch.len1 = cur->str.size(), newmatch.len2 = sig.first.size(), newmatch.level = cur->l;
					extend(newmatch, x, y);
					matches.push_back(newmatch);
				}				
			if(tmp)
				for(auto& x:cur->child)
					queue.push(x);
		}
		++level;
	}
}

vector<match> mergematch(vector<match>& matches)
{
	vector<match> output;
	for(auto x:matches)
	{
		if(output.size() == 0 || x.pos1 > output.back().pos1 + output.back().len1)
		{
			output.push_back(x);
			continue;
		}
		output.back().len1 = x.pos1 + x.len1 - output.back().pos1;
		output.back().len2 = x.pos2 + x.len2 - output.back().pos2;
	}
	return output;
};


int checkstr(string &x, string& y)
{
	vector<match> matches;
	Node* root = partitiontree(x, 1);
	map<int, vector<pair<string, int>>, greater<int>> par = partition(y, 1);
	bfstree(root, par, matches, x, y);

	sort(matches.begin(), matches.end(), matchcmp);
	matches.erase(unique(matches.begin(), matches.end(), matchunique), matches.end());
	matches = mergematch(matches);
	int len = 0;
	for(auto& m:matches)
	{
		//cout<<m.pos1<<" "<<m.pos2<<" "<<m.len1<<" "<<x.substr(m.pos1, m.len1)<<endl;
		len += m.len1;
	}
	//cout<<len<<" "<<x.size()<<endl;
	return len;
}
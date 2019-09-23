// hashjoin.cpp : Defines the entry point for the console application.
//

//   g++ edit.cpp timerec.cpp hash.cpp minsearch.cpp partition.cpp filter.cpp match.cpp -std=c++11 -o minsearch -O3
#include "minsearch.h"

// input parameters
int k_input = 50;
string filename = "trec50k.txt", queryfile = "dnadataquery.txt";
vector<string> oridata, querydata; //original strings and query strings
int q = 2;
double alpha = 2.0;
string qtype;

// data storage
vector<int> indices;
vector<string> oridata_modified, query_modified; //modified strings for edit distance computation
unordered_map<int, unordered_map<string, vector<pair<int, int>>>> hash_table;
vector<pair<int, int>> candidate;
vector<pair<int, int>> outputs;


// parameter for filter
bool lenfilter = true;
int countbit = 1; 
vector<vector<int>> orisig; // sig for count filter
vector<vector<pair<int, int>>> neighbor;
int neighbork = -1;

// output and time
bool outputresult = false;
struct timeval tstart, tend, tstart1, tend1, tstart2, tend2;
float readtime, indextime, querytime, parttime, verifytime, partquery;
bool verbosetime = true;


void readdata()
{
	ifstream  data(filename);
	string cell;
	int number_string = 0;

	while (getline(data, cell))
	{
		if(cell.size() < 10)
			continue;
		number_string++;
		oridata.push_back(cell);
	}
	for (int i = 0; i < oridata.size(); i++)
		indices.push_back(i);
	compare c;
	
	sort(indices.begin(), indices.end(), [&](int i1, int i2) { if (oridata[i1].size() != oridata[i2].size())
									 return oridata[i1].size() <  oridata[i2].size(); 
								   else
									 return oridata[i1] <  oridata[i2];});
	sort(oridata.begin(), oridata.end(), c);
	
	oridata_modified = oridata;		// append distinguisher at the end
	for (int j = 0; j < oridata_modified.size(); j++){
		for (int k = 0; k < 8; k++) oridata_modified[j].push_back(j >> (8 * k));
	}

	ifstream  query(queryfile);
	number_string = 0;

	while (getline(query, cell))
	{
		if(cell.size() < 10)
			continue;
		number_string++;
		querydata.push_back(cell);
	}

	query_modified = querydata;		
	for (int j = 0; j < query_modified.size(); j++){
		for (int k = 0; k < 8; k++) query_modified[j].push_back(j >> (8 * k));
	}


	fprintf(stderr, "Threshold:%d \n", k_input);
	fprintf(stderr, "Number of string:%d \n", oridata.size());
	fprintf(stderr, "Number of query:%d \n", querydata.size());

}


void buildindex()
{
	neighbor.resize(oridata.size());
	for (int i = 0; i < oridata.size(); ++i)
	{
		recordtime(tstart1);
		map<int, vector<pair<string, int>>, greater<int>> par = partition(oridata[i]);
		recordtime(tend1);
		updatetime(parttime, tstart1, tend1);

		if(countbit > 0)
			orisig.push_back(calsig(oridata[i]));
		unordered_set<int> verified;
		for(auto& k : par)
		{	
			for(auto& x : k.second)
			{
				if(neighbork > 0 && k.first > 15 && hash_table[k.first][x.first].size() < 150)
				for(auto it = hash_table[k.first][x.first].begin(); it != hash_table[k.first][x.first].end(); ++it)
				{
					if(verified.find((*it).first) != verified.end() || (*it).first == i)
						continue;
					if(!lengthfilter(oridata[i], oridata[(*it).first], oridata[i].size(), (*it).second, neighbork))
						continue;

					verified.insert((*it).first); 
					int dp = edit_dp(oridata_modified[(*it).first].data(), oridata[(*it).first].size(), oridata_modified[i].data(), oridata[i].size(), neighbork);
					if(dp != -1)
						neighbor[i].push_back(make_pair((*it).first, dp));
					
				}
				hash_table[k.first][x.first].push_back(make_pair(i, x.second));
			}
		}
	}
	if(lenfilter)
	for(auto & level:hash_table)
		for(auto & vec:level.second)
			sort(vec.second.begin(), vec.second.end(), paircmp);
}
	

void searchthreshold()
{
	for (int i = 0; i < querydata.size(); ++i)
	{
		recordtime(tstart1);
		int min_r = max(1, (int)((querydata[i].size())/(2 * alpha * k_input + 2)));
		map<int, vector<pair<string, int>>, greater<int>> par = partition(querydata[i], min_r);
		recordtime(tend1);
		updatetime(partquery, tstart1, tend1);

		for (auto & k : par)
		{
			if (hash_table.find(k.first) == hash_table.end())
				continue;
			for(auto& x : k.second)
			{
				if(hash_table[k.first].find(x.first) == hash_table[k.first].end())
					continue;
				for (auto it = hash_table[k.first][x.first].begin(); it != hash_table[k.first][x.first].end(); ++it)
				{
					if(lengthfilter(oridata[(*it).first], querydata[i], (*it).second, x.second, k_input))
						candidate.push_back({ (*it).first, i });
				}
			}			
		}
	}
}


void verify()
{
	recordtime(tstart2);

	sort(candidate.begin(), candidate.end());
	candidate.erase(unique(candidate.begin(), candidate.end()), candidate.end());
	fprintf(stderr, "Num of candidate:%d\n", candidate.size());

	for (auto x : candidate)
	{
		#ifdef _WIN32
			int ed = edit_dp(oridata[x.first], querydata[x.second], k_input);
		#else
			int ed = edit_dp(oridata_modified[x.first].data(), oridata[x.first].size(), query_modified[x.second].data(), querydata[x.second].size(), k_input);
		#endif
		if (ed != -1)
		{
			outputs.push_back(x);
			if (outputresult)
				cout <<"queryid/outputid: "<< x.second << " " << x.first << endl;
		}
	}
	
	fprintf(stderr, "Num of outputs:%d\n", int(outputs.size()));
	recordtime(tend2);
	updatetime(verifytime, tstart2, tend2);
}



void outputtopk(int queryid, priority_queue<pair<int, int>,  vector<pair<int, int>>,  pqcmp>& cand)
{
	int i = 1;
	while(cand.size())
	{
		cout<<cand.top().second<<" "<<cand.top().first<<endl;
		++i;
		cand.pop();
	}
}

int find_pos(vector<pair<int, int>>bucket,int pos, int min_ed)
{
	if(bucket.size() < 3)
		return 0;
	int begin = 0, end = bucket.size() - 1;
	while(begin + 1 < end)
	{
		int mid = begin + (end - begin) / 2;
		if(bucket[mid].second  < pos - min_ed)
			begin = mid;
		else 
			end = mid;
	}
	return bucket[begin].second  >= pos - min_ed ? begin:end;
}


void searchtopk(){
	
	for (int i = 0; i < querydata.size(); ++i)
	{
		recordtime(tstart1);
		map<int, vector<pair<string, int>>, greater<int>> par = partition(querydata[i]);
		recordtime(tend1);
		updatetime(partquery, tstart1, tend1);

		int min_level = INT_MIN, min_ed = querydata[i].size(), num_c = 0, num_c2 = 0, num_pru = 0;
		unordered_set<int> verified; 
		priority_queue<pair<int, int>,  vector<pair<int, int>>,  pqcmp> cand; // <distance, id> 
		
		vector<int> querysig;
		if(countbit > 0)
		 	querysig = calsig(querydata[i]);
		
		for (auto & k : par)
		{
			if (hash_table.find(k.first) == hash_table.end())
				continue;
			if(cand.size() == k_input && k.first < min_level)
				break;
			for(auto& x : k.second) // proceed to nodes in next level
			{
				if(hash_table[k.first].find(x.first) == hash_table[k.first].end())
					continue;
				auto it = hash_table[k.first][x.first].begin() + find_pos(hash_table[k.first][x.first], x.second, min_ed);
				for (; it != hash_table[k.first][x.first].end(); ++it)
				{
					if((*it).second > x.second + min_ed)
						break;
					++num_c2;
					if(verified.find((*it).first) != verified.end())
						continue;
					if(lenfilter && cand.size() == k_input && !lengthfilter(oridata[(*it).first], querydata[i], (*it).second, x.second, min_ed))
						continue;
					if(cand.size() == k_input && countbit > 0 && !countfilter(orisig[(*it).first], querysig, min_ed))
						continue;
					verified.insert((*it).first); // must behind if, same sig may have different pos


					++num_c;
					recordtime(tstart2);
					int dp = edit_dp(oridata_modified[(*it).first].data(), oridata[(*it).first].size(), query_modified[i].data(), querydata[i].size(), min_ed);
					recordtime(tend2);
					updatetime(verifytime, tstart2, tend2);

					if(dp != -1)
					{
						cand.push(make_pair(dp, (*it).first));
						for(auto& nei:neighbor[(*it).first])
							if(nei.second + dp <= min_ed && verified.find(nei.first) == verified.end())
							{
								int dp2 = edit_dp(oridata_modified[nei.first].data(), oridata[nei.first].size(), query_modified[i].data(), querydata[i].size(), min_ed);
								cand.push(make_pair(dp2, nei.first));
								//cout<<i<<" "<<dp2<<endl;
								verified.insert(nei.first);
							}
						while(cand.size() > k_input)
						{
							cand.pop();
							min_ed = min(min_ed, cand.top().first - 1);
						}		
					}
					else if(dp == -1)
					{
						num_pru +=neighbor[(*it).first].size();
						for(auto& nei:neighbor[(*it).first])
								verified.insert(nei.first);
					}
					
				}
			}
			min_level =  max(1, (int)((querydata[i].size())/(2 * alpha * max(min_ed, 1) + 2)));				
		}
		if (outputresult)
			outputtopk(i, cand);
	}
}

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		filename = argv[1];
		queryfile = argv[2];
		k_input = atoi(argv[4]);
		q = atoi(argv[5]);
		dictsize = atoi(argv[6]);
		alpha = atof(argv[7]);
		fprintf(stderr, "alpha:%f \n", alpha);
	}
	
	string querytype(argv[3]);
	qtype = querytype;
	if(dictsize != 5 && dictsize != 26 && dictsize != 32 && dictsize != 37)
	{
		cout<<"dictsize should be 5 26 32 27"<<endl;
	}
	if(querytype != "-t" && querytype != "-k" )
	{
		cout<<"input: datafile queryfile -t/-k(threshold/topk) t/k q dictsize alpha"<<endl;
		return 0;
	}
	if(querytype != "-t" && querytype != "-k" )
	{
		cout<<"input: datafile queryfile -t/-k(threshold/topk) t/k q dictsize alpha"<<endl;
		return 0;
	}

	
	if(argc > 8 && atoi(argv[8]) == 1)
		outputresult = true;
	
	recordtime(tstart);
	initiHash();
	readdata();
	recordtime(tend);
	updatetime(readtime, tstart, tend);

	recordtime(tstart);
	buildindex();
	recordtime(tend);
	updatetime(indextime, tstart, tend);
	
	recordtime(tstart);
	if(querytype == "-t")
	{
		searchthreshold();
		verify();
	}
	else if(querytype == "-k")
	{
		searchtopk();
	}
	recordtime(tend);
	updatetime(querytime , tstart, tend);

	fprintf(stderr, "time for read:%f \n", readtime);
	fprintf(stderr, "Ntime for build indices:%f \n", indextime );
	fprintf(stderr, "avg time for query(ms):%f \n", querytime / querydata.size() * 1000.0);

	#ifdef _WIN32
		while (1);
	#endif
}



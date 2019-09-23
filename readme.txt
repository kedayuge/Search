1. compile:  
git clone https://github.com/kedayuge/Search.git
cd Search
g++ edit.cpp timerec.cpp hash.cpp minsearch.cpp partition.cpp filter.cpp match.cpp -std=c++11 -o minsearch -O3

2. usage: ./minsearch inputstr querystr -t/-k K/k q dictsize alpha

inputstr/querystr: filename of input strings and query strings 

-t or -k: query type threshold query or top-k query

K/k: edit distance threshold or top "k" query

q: length of q-gram, e.g. [2, 10] (depends on alpha beta size of input strings)

dictsize: alpha beta size of input strings, could be 5 for DNA dataset (ACGTN); 26 for UNIREF dataset (A~Z); 37 for TREC dataset (A~Z,0~9,' '); 32 for DBLP

alpha: a parameter controls accuracy and running time, should be a small number e.g. [0.5, 3]

3. example input:
./minsearch gen50ks.txt genquery.txt -t 150 8 5 0.6
./minsearch dblp.txt dblpquery.txt -t 25 2 32 1
./minsearch trec.txt trecquery.txt -t 50 2 37 1
./minsearch uniref.txt unirefquery.txt -t 25 1 26 2

./minsearch dblp.txt dblpquerytop.txt -k 15 2 32 3
./minsearch uniref.txt unirefquery.txt -k 15 2 26 3
./minsearch reads.txt readsquery.txt -k 15 2 5 3 
./minsearch gen50ks.txt genquery.txt -k 15 8 5 2

Note that, all output pairs are distinct, and verified by exact edit distance filter. 
Thus, the number of output divided by number of ground truth (by exact algorithm such as Passjoin, Qchunck) is the accuracy.

4.Input format:(both query and data)
A collection of strings seperated by enter
For example:
ACGTTTTTTTAATTTTATAGGGGGC
ACGTAATTTAATTTTATAGGGGGC
ACGTTTTTTTATTTATAGGGTTGGC
...

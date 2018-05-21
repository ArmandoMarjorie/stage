#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <map>
#include <memory>

using namespace std;

int main(int argc, char** argv) 
{ 
	ifstream data_file(argv[1], ios::in);
	if(!data_file)
	{ 
		cerr << "Impossible to open the file " << argv[1] << endl;
		return 0;
	}
	unsigned word1, word2;
	float proba;
	map< pair<float,unsigned>, unsigned > bi;
	data_file >> word1;
	data_file >> proba;
	while(data_file >> word1)
	{
		data_file >> word2;
		data_file >> proba;
		bi[make_pair(proba, word1)] = word2;
	}
	
	for(map<pair<float,unsigned>, unsigned >::iterator it=bi.begin(); it!=bi.end(); ++it) //parcours de la 2e map
	{
		cout << it->first.first << " " << it->first.second << " " << it->second << endl;
	}	
	
	return 0;
}


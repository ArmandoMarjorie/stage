#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>

using namespace std;

/** Calculate the maximum nb of words in all sentence to know how many 0 we have to add */
void max_length(char* id_filename, unsigned& max)
{
	ifstream file(id_filename, ios::in);
	if(!file)
	{
		cerr << "Impossible to open the file " << id_filename << endl;
		exit(EXIT_FAILURE);
	}
	unsigned val;
	while( file >> val ) 
	{
		if(val == -1)
		{
			file >> val;
			if( val > max )
				max = val;
		}
	}
	file.close();
}

/** Add 0 so every sentences have the same length */  
void add_padding(char* id_filename, char* output_filename, unsigned max)
{
	ifstream file(id_filename, ios::in);
	if(!file)
	{
		cerr << "Impossible to open the file " << id_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output_file(output_filename, ios::out | ios::trunc);
	if(!output_file)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	output_file << max <<endl;
	string line;
	unsigned i, nb_words;
	while( getline(file, line) )
	{
		i = 0;
		nb_words = 0;
		while(i<line.size() && line[i] != '-')
		{
			output_file << line[i];
			if(line[i] == ' ')
				++nb_words;
			++i;
		}
		if(i<line.size() && line[i]=='-')
		{
			while( nb_words < max )
			{
				output_file << "0 ";
				++nb_words;
			}
		}
		
		output_file << endl;
	}
	
}

int main(int argc, char** argv)
{
	if(argc != 7)
	{
		cerr << "Usage : " << argv[0]
			 << " dev_tok_id\n test_tok_id\n train_tok_id\n output_dev_name\n output_test_name\n output_train_name\n";
		exit(EXIT_FAILURE);
	}
	unsigned max=0;
	for(unsigned i=1; i<=3; ++i)
	{
		max_length(argv[i], max);
		cerr << "Max " << argv[i] << " = " << max << endl;
	}
	for(unsigned i=1; i<=3; ++i)
		add_padding(argv[i], argv[i+3], max);
	
	return 0;
}



















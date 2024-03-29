#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <locale>

using namespace std;

// sert pour generer le vocabulaire des mots de remplacements !!


/* snli_json_filename = file dans Files/without_id */
void write_vocab_file(char* snli_json_filename, map< string, unsigned >& vocab, ofstream& output)
{
	ifstream snli_file(snli_json_filename, ios::in);
	if(!snli_file)
	{ 
		cerr << "Impossible to open the file " << snli_json_filename << endl;
		exit(EXIT_FAILURE);
	}		
	cerr << "Reading " << snli_json_filename << " ...\n";
	//vector<string>::iterator it;
	map< string, unsigned >::iterator it;
	string word;
	unsigned i;
	while(snli_file >> word) //read a label
	{
		/* read premises and hypothesis */
		for(i=0; i<2; ++i)
		{
			snli_file >> word;
			while(word != "-1")
			{
				std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
				it = vocab.find(word);
				if( it == vocab.end() ) //unknown word
				{
					output << word << endl;
					vocab[word] = 1;
					//cerr << word << endl;				
				}
				snli_file >> word;
			}
			snli_file >> word; //read the sentence size
		}
	}
	
	snli_file.close();
}

int main(int argc, char** argv)
{
	//if(argc != 3)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "dev_file (in without_id folder)\n"
			 << "test_file (in without_id folder)\n"
			 << "train_file (in without_id folder)\n";
		//exit(EXIT_FAILURE);
	}
	
	/* Pour avoir snli.vocab */
	map< string, unsigned > vocab;
	unsigned j=0;
	ofstream output("snli.vocab", ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file snli.vocab" << endl;
		exit(EXIT_FAILURE);
	}
	write_vocab_file(argv[1], vocab, output);
	//write_vocab_file(argv[2], vocab, output);
	//write_vocab_file(argv[3], vocab, output);
	
	output.close();
	
	
	return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <locale>


/** LEXIQUE = VOCABULAIRE + LEUR ID */

using namespace std;

void generating_lexique(char* data_id_filename, char* data_word_filename, map<string, int>& word_to_id, ofstream& output)
{
	ifstream data_id_file(data_id_filename, ios::in);
	if(!data_id_file)
	{ 
		cerr << "Impossible to open the file " << data_id_filename << endl;
		exit(EXIT_FAILURE);
	}	
	ifstream data_word_file(data_word_filename, ios::in);
	if(!data_word_file)
	{ 
		cerr << "Impossible to open the file " << data_word_filename << endl;
		exit(EXIT_FAILURE);
	}	

	
	unsigned i;
	
	int id;
	string word;
	map<string, int>::iterator it;
	cerr << "reading" << data_id_filename << " and " << data_word_filename << " ...\n";
	
	while( data_word_file >> word && data_id_file >> id ) //read a label
	{
		/* read premises and hypothesis */
		for(i=0; i<2; ++i)
		{
			data_word_file >> word;
			data_id_file >> id;
			
			while(id != -1)
			{
				std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
				
				it = word_to_id.find(word);
				if( it == word_to_id.end() ) //unknown word
				{
					output << word << " " << id << endl;
					word_to_id[word] = id;			
				}		
				data_word_file >> word;
				data_id_file >> id;
			}
			data_word_file >> word;
			data_id_file >> id;
		}		
		
	}
	data_word_file.close();
	data_id_file.close();
	
}


int main(int argc, char** argv)
{
	if(argc != 8)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "test_file with id\n"
			 << "test_file with word\n"
			 << "dev_file with id\n"
			 << "dev_file with word\n"
			 << "train_file with id\n"
			 << "train_file with word\n"
			 << "lexique filename (output)\n";
		exit(EXIT_FAILURE);
	}
	ofstream output(argv[7], ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << argv[7] << endl;
		exit(EXIT_FAILURE);
	}
	map<string, int> word_to_id;
	for(unsigned i = 1; i <= 5; i+=2)
		generating_lexique(argv[i], argv[i+1], word_to_id, output);
	
	
	return 0;
}


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

void init_word_to_id(map<string, int>& word_to_id, char* glove_snli_filename, unsigned dim_embedding)
{
	ifstream glove_snli(glove_snli_filename, ios::in);
	if(!glove_snli)
	{ 
		cerr << "Impossible to open the file " << glove_snli_filename << endl;
		exit(EXIT_FAILURE);
	}	
	string word;
	float val;
	unsigned i;	
	unsigned line=1;
	cerr << "reading " << glove_snli_filename << " ...\n";
	while( glove_snli >> word )
	{
		word_to_id[word] = line; //add the word and give it the num of its line
		++line;
		for(i=0; i<dim_embedding; ++i)
			glove_snli >> val;
		
	}
	cerr << "ok\n";
	glove_snli.close();
}

void generating_output_file(char* set_filename, char* output_filename, map<string, int>& word_to_id, map<string, int>& label_to_id, unsigned& code_label,
	 map<string, int>& unk_word_to_id, unsigned& code_unk, bool all_unk_same)
{
	ifstream snli_file(set_filename, ios::in);
	if(!snli_file)
	{ 
		cerr << "Impossible to open the file " << set_filename << endl;
		exit(EXIT_FAILURE);
	}	
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	map<string, int>::iterator it;
	map<string, int>::iterator it_label;
	map<string, int>::iterator it_unk;
	unsigned i;
	string word;
	cerr << "generating" << output_filename << " ...\n";
	while( snli_file >> word ) //read a label
	{
		it_label = label_to_id.find(word);
		if( it_label == label_to_id.end() )
		{
			label_to_id[word] = code_label;
			++code_label;
		}
		output << label_to_id[word] << endl;
		
		/* read premises and hypothesis */
		for(i=0; i<2; ++i)
		{
			snli_file >> word;
			while(word != "-1")
			{
				std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
				it = word_to_id.find(word);
				if( it == word_to_id.end() ) //unknown word
				{
					if(all_unk_same)
						output << "0 ";
					else
					{
						it_unk = unk_word_to_id.find(word);
						if( it_unk == unk_word_to_id.end() )
						{
							unk_word_to_id[word] = code_unk;
							++code_unk;
						}
						output << unk_word_to_id[word] << " ";		
					}
				}
				else
					output << word_to_id[word] << " ";
				snli_file >> word;
			}
			snli_file >> word; //read the sentence size
			output << "-1 " << word << endl;
		}		
		
	}
	snli_file.close();
	output.close();
	
}


int main(int argc, char** argv)
{
	if(argc != 10)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "dev_file (in without_id folder)\n"
			 << "test_file (in without_id folder)\n"
			 << "train_file (in without_id folder)\n\n"
			 
			 << "output_dev_filename\n"
			 << "output_test_filename\n"
			 << "output_train_filename\n\n"
			 
			 << "glove_snli_file\n"
			 << "dim_embeddeding\n"
			 << "unk_words_are_the_same (1 if true, 0 otherwise)\n\n";
		exit(EXIT_FAILURE);
	}
	map<string, int> word_to_id;
	init_word_to_id(word_to_id, argv[7], static_cast<unsigned>(atoi(argv[8])));
	
	map<string, int> label_to_id;
	map<string, int> unk_word_to_id;
	unsigned code_label = 0;
	unsigned code_unk = 28650;
	bool all_unk_same = ( atoi(argv[9]) == 1 );
	generating_output_file(argv[1], argv[4], word_to_id, label_to_id, code_label, unk_word_to_id, code_unk, all_unk_same);
	generating_output_file(argv[2], argv[5], word_to_id, label_to_id, code_label, unk_word_to_id, code_unk, all_unk_same);
	generating_output_file(argv[3], argv[6], word_to_id, label_to_id, code_label, unk_word_to_id, code_unk, all_unk_same);
	
	
	return 0;
}


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

void init_vocab(vector<string>& vocab, char* snli_vocab_filename)
{
	ifstream snli_vocab(snli_vocab_filename, ios::in);
	if(!snli_vocab)
	{ 
		cerr << "Impossible to open the file " << snli_vocab_filename << endl;
		exit(EXIT_FAILURE);
	}	
	cerr << "reading snli.vocab ...\n";
	vector<string>::iterator it;
	string word;
	while(snli_vocab >> word)
	{
		it = find( vocab.begin(), vocab.end(), word );
		if( it==vocab.end() )
			vocab.push_back( word );
	}
	cerr << "ok\n";
	snli_vocab.close();
}

void generating_glove(vector<string>& vocab, char* glove_filename, char* glove_output_filename, unsigned dim_embedding)
{
	ifstream glove(glove_filename, ios::in);
	if(!glove)
	{ 
		cerr << "Impossible to open the file " << glove_filename << endl;
		exit(EXIT_FAILURE);
	}	
	ofstream output(glove_output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << glove_output_filename << endl;
		exit(EXIT_FAILURE);
	}
	vector<string>::iterator it;
	string word;
	float val;
	unsigned i;
	cerr << "generating "<< glove_output_filename << " ...\n";
	while( glove >> word )
	{
		it = find( vocab.begin(), vocab.end(), word );
		if( it != vocab.end() ) //the word exists in the snli files
		{
			output << word << " ";
			for(i=0; i<dim_embedding; ++i)
			{
				glove >> val;
				output << val << " ";
			}
			output << endl;
		}
		else
		{
			for(i=0; i<dim_embedding; ++i)
				glove >> val;
		}
	}
	
	glove.close();
	output.close();
	
}

int main(int argc, char** argv)
{
	if(argc != 5)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "snli.vocab\n"
			 << "glove_file\n"
			 << "output_glove_snli_filename\n"
			 << "dim_embeddeding\n\n";
		exit(EXIT_FAILURE);
	}
	vector<string> vocab;
	init_vocab(vocab, argv[1]);
	generating_glove(vocab, argv[2], argv[3], static_cast<unsigned>(atoi(argv[4])));
	
	return 0;
}


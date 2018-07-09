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
void write_vocab_file(char* snli_json_filename, char* output_f)
{
	ifstream snli_file(snli_json_filename, ios::in);
	if(!snli_file)
	{ 
		cerr << "Impossible to open the file " << snli_json_filename << endl;
		exit(EXIT_FAILURE);
	}		
	ofstream output(output_f, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file snli.vocab" << endl;
		exit(EXIT_FAILURE);
	}
	
	map< string, unsigned > vocab;
	map< string, unsigned >::iterator it;
	string word, word2, w;
	unsigned i;
	unsigned nb_word;
	
	while(getline(snli_file, word)) //read -3
	{
		cout << word << endl;
		getline(snli_file, word); //read the label
		cout << word << endl;
		
		/* read premises and hypothesis */
		for(unsigned nb=0; nb<2; ++nb)
		{
			getline(snli_file, word);//lit premise -2
			getline(snli_file, word); //lit nbr d'expr
			
			
			getline(snli_file, w); //lit premise
			stringstream fluxstring(w);
			while(fluxstring >> word)
			{	
				cout << word << endl;
				
				while(word[word.size()-1] != ']')
				{
					fluxstring >> word2;
					word = word + " ";
					word = word + word2;
				}
				cout << word << endl;
				for(unsigned j=1; j<word.size()-1; ++j)
				{
					stringstream ss;
					while(j < word.size()-1 && word[j] != ' ' )
					{
						ss << word[j];
						++j;
					}
					word2 = ss.str();
					it = vocab.find(word2);
					if( it == vocab.end() )
					{
						vocab[word2] = 1;
						output << word2 << endl;		
					}			
				}			
			}
				
		}
		getline(snli_file, word);getline(snli_file, word);getline(snli_file, word); //lit -3 de fin
	}
	
	snli_file.close();
	output.close();
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "mot_pour_lime"
			 << "output vocab du fichier";
		exit(EXIT_FAILURE);
	}
	
	/* Pour avoir snli.vocab */

	write_vocab_file(argv[1], argv[2]);
	
	
	
	
	return 0;
}

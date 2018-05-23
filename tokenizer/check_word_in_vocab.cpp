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

//sert pour tokenizer les mots de remplacements

void reading_lexique(char* lexique_filename, map<string, int>& word_to_id)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}
	string word;
	int id;
	while(lexique_file >> word && lexique_file >> id)
		word_to_id[word] = id;
	word_to_id["-1"] = -1;
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
}

/* */
void check(char* lexique, char* file, char* output_f, bool specific)
{
	ifstream f(file, ios::in);
	if(!f)
	{ 
		cerr << "Impossible to open the file " << file << endl;
		exit(EXIT_FAILURE);
	}		
	ofstream output(output_f, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file "<< output_f << endl;
		exit(EXIT_FAILURE);
	}
	map<string, int> word_to_id;
	reading_lexique(lexique, word_to_id);
	if(specific)
	{
		word_to_id["NEU"] = -2;
		word_to_id["SYN"] = -3;
	}
	map< string, int >::iterator it;
	string word;
	while(f >> word)
	{
		it = word_to_id.find(word);
		if( it == word_to_id.end() )
			output << "INCONNU ";
		else
			output << word_to_id[word] << " ";
	}
	
	f.close();
}

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique\n"
			 << "file mots remplacant detoken\n"
			 << "output (file mots remplacant token)\n";
		exit(EXIT_FAILURE);
	}
	

	check(argv[1], argv[2], argv[3]);
	return 0;
}

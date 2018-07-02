#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <utility>
#include <algorithm>
#include <locale>
#include <cmath>
#include <algorithm>

/** LEXIQUE = VOCABULAIRE + LEUR ID */
// g++ -std=c++11 detoken_explication.cpp -o Detok_expl

using namespace std;


void reading_lexique(char* lexique_filename, map<int, string>& id_to_word)
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
		id_to_word[id] = word;
	id_to_word[0] = "BOS";
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
}
/* faire detoken pr removing couple (avec DI) */

void detokenizer_with_couple(char* lexique_filename, char* explication_filename, char* output_filename)
{
	ifstream explication_file(explication_filename, ios::in);
	if(!explication_file)
	{ 
		cerr << "Impossible to open the file " << explication_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	cerr << "Reading " << lexique_filename << endl;
	/*Reading lexique and saving in a map the word of an id*/
	map<int, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word);
	int mot;
	while(explication_file >> mot)
	{
		while(mot != -1)
		{
			output << id_to_word[mot] << " ";
			explication_file >> mot;
		}
		output << "-1\n";
	}

	
}

void detoken_bigram(char* lexique_filename, char* explication_filename, char* output_filename)
{
	ifstream explication_file(explication_filename, ios::in);
	if(!explication_file)
	{ 
		cerr << "Impossible to open the file " << explication_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	cerr << "Reading " << lexique_filename << endl;
	/*Reading lexique and saving in a map the word of an id*/
	map<int, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word);
	int mot;
	float proba;
	while(explication_file >> proba)
	{
		output << proba << " ";
		explication_file >> mot;
		output << id_to_word[mot] << " ";
		explication_file >> mot;
		output << id_to_word[mot] << endl;
	}	
}

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique_file\n"
			 << "explication_file (format with id)\n"
			 << "output_name (explication file detokenized)\n";
		exit(EXIT_FAILURE);
	}
	
	detoken_bigram(argv[1], argv[2], argv[3]);
	
	return 0;
}



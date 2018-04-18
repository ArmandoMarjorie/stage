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
// g++ -std=c++11 detoken_explication.cpp -o Detok_expl

using namespace std;

string detoken_label(int label)
{
	switch(label)
	{
		case 0:
		{
			return "neutral";
		}
		case 1:
		{
			return "entailment";
		}
		case 2:
		{
			return "contradiction";
		}
		default:
		{
			return "not a label";
		}
	}
	
}

void detokenizer(char* lexique_filename, char* explication_filename, char* output_filename)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}
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
	string word;
	unsigned id;
	map<unsigned, string> id_to_word;
	while(lexique_file >> word && lexique_file >> id)
		id_to_word[id] = word;
	id_to_word[0] = "every word";
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
	int val;
	unsigned cpt;
	unsigned num_sample = 1;
	
	while(explication_file >> val)
	{
		output << "\tsample numero " << num_sample << "\nlabel : " << detoken_label(val) << ", ";
		explication_file >> val;
		output<< "label predicted : " << detoken_label(val) << endl;
		
		explication_file >> val;
		for(cpt=0; cpt <2; ++cpt) // reading the premise and the hypothesis
		{
			if(cpt==0)
				output << "premise : ";
			else
				output << "hypothesis : ";
			while(val != -1)
			{
				output << id_to_word[val] << " ";
				explication_file >> val;
			}
			explication_file >> val; //read the "-1"
			output << endl;
		}
		while(val != -3)
		{
			while(val != -1) //reading couple
			{
				if(val == -2)
					output << " ; ";
				else
					output << id_to_word[val] << " ";
				explication_file >> val;
			}
			explication_file >> val; //reading label's couple
			output << " = label predicted : " << detoken_label(val) << " , true label : " ;
			explication_file >> val;
			output << detoken_label(val) << endl;
			explication_file >> val;
		}
		++num_sample;
		output << endl;
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
	
	detokenizer(argv[1], argv[2], argv[3]);
	
	return 0;
}


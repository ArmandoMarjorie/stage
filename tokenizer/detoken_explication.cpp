#include "detoken_explication.hpp"


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

void reading_lexique(char* lexique_filename, map<unsigned, string>& id_to_word, bool DI)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}
	string word;
	unsigned id;
	while(lexique_file >> word && lexique_file >> id)
		id_to_word[id] = word;
	if(!DI)
		id_to_word[0] = "<every words>";
	else
		id_to_word[0] = "<no words>";
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
}
/* faire detoken pr removing couple (avec DI) */

void detokenizer_with_couple(char* lexique_filename, char* explication_filename, char* output_filename, bool DI)
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
	map<unsigned, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word, DI);
	
	int val;
	float val_di=0;
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
			if(!DI)
			{
				explication_file >> val; //reading label's couple
				output << " = label predicted : " << detoken_label(val) << " , true label : " ;
				explication_file >> val;
				output << detoken_label(val) << endl;
			}
			else
			{
				for(unsigned l=0; l<3; ++l) //read the 3 DIs
				{
					explication_file >> val_di; //reading DI
					output << " DI["<< detoken_label(l) <<"] : " << val_di << " ; ";
				}
				output << endl;
			}
			explication_file >> val;
		}
		++num_sample;
		output << endl;
	}
	
}



void detoken_expl(char* lexique_filename, char* explication_filename, char* output_filename)
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
	/*Reading lexique and saving in a map the word of an id*/
	map<unsigned, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word, false);	
	int val;
	unsigned cpt;
	unsigned num_sample=1;
	while(explication_file >> val)
	{
		output << "\tsample numero " << num_sample << "\nlabel : " << detoken_label(val) << ", ";
		++num_sample;
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
			explication_file >> val;
			output << endl;
		}
		
		/* important words in prem and in hyp */
		output << "in premise : ";
		while(val != -1)
		{
			output << id_to_word[val] << " ";
			explication_file >> val;
		}
		output << endl;
		explication_file >> val;
		output << "in hypothesis : ";
		while(val != -3)
		{
			output << id_to_word[val] << " ";
			explication_file >> val;
		}
		output << endl;
	}	
	output.close();
	explication_file.close();
	
}

/*
int main(int argc, char** argv)
{
	if(argc != 5)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique_file\n"
			 << "explication_file (format with id)\n"
			 << "with DI (1) or without DI (0)\n"
			 << "output_name (explication file detokenized)\n";
		exit(EXIT_FAILURE);
	}
	bool DI = (atoi(argv[3]) == 1);
	
	detokenizer(argv[1], argv[2], argv[4], DI);
	
	return 0;
}
*/

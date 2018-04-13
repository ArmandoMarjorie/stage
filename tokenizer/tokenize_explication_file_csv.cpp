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
// g++ -std=c++11 tokenize_explication_file_csv.cpp -o Tok_expl_csv

using namespace std;

void write_output(string label, ofstream& output)
{
	if(label == "neutral")
		output << "0\n";
	else if(label == "entailment")
		output << "1\n";
	else if(label == "contradiction")
		output << "2\n";
	else
		output << "incorrect entry\n";
}


void write_output(string sentence, map<string, unsigned>& word_to_id, ofstream& output)
{
	unsigned nb_words=0;
	for(unsigned i=0; i<sentence.size(); ++i)
	{
		stringstream word_ss;
		for(; i<sentence.size() && sentence[i] != ' '; ++i)
			word_ss << sentence[i];
		string word = word_ss.str();
		++nb_words;
		std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
		output << word_to_id[word] << " ";
	}
	output << "-1 " << nb_words << endl;
}


void write_output_couple(string sentence, map<string, unsigned>& word_to_id, ofstream& output)
{
	bool parenthesis=false;
	for(unsigned i=0; i<sentence.size(); ++i)
	{
		if(sentence[i] == '(' || sentence[i] == ',')
		{
			//cerr << "Read a ( \n";
			continue;
		}
		while(sentence[i] != ')')
		{
			stringstream word_ss;
			for(; i<sentence.size() && (sentence[i] != ',' && sentence[i] != ')'); ++i)
			{
				if(sentence[i] == ' ')
				{
					string word = word_ss.str();
				//	cerr << word << " ";
					std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
					if(word == "-1")
						output << "0 ";
					else
						output << word_to_id[word] << " ";
					std::stringstream().swap(word_ss); // flush word_ss
					++i;
				}
				word_ss << sentence[i];
				//cerr << "i = " << i << ", lettre = " << sentence[i] << endl;
			}
			//cerr << "sortie\n";
			if(sentence[i] == ',' || sentence[i] == ')') 
			{
				string word = word_ss.str();
				std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
				if(word == "-1")
					output << "0";
				else
					output << word_to_id[word]; 
				if(sentence[i] == ',')
				{
					++i; //read the ',' (comma)
					//cerr << word << " -2 ";
					output << " -2 ";
				}
				else
				{
					//cerr << word << endl;
					output << " -1 " << endl;
				}
			}
		}
	}
	output << "-3\n";
}

void generating_tokenizing_explication(char* lexique_filename, char* explication_filename, char* output_filename)
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
	
	string word;
	unsigned id;
	map<string, unsigned> word_to_id;
	while(lexique_file >> word && lexique_file >> id)
		word_to_id[word] = id;
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();
	
	getline(explication_file, word); //read the label of each column in the csv file
	unsigned cpt_guillemet;
	while(getline(explication_file, word))
	{
		cpt_guillemet=0;
		// Extracting label, premise, hypothesis and couple
		for(unsigned i=0; i<word.size(); ++i)
		{
			if(word[i] == '"')
				++cpt_guillemet;
				
			// label
			else if(cpt_guillemet == 3)
			{
				stringstream label_ss;
				for(; word[i] != '"'; ++i)
					label_ss << word[i];
				string label = label_ss.str();
				--i; //unread the "
				//cerr << "label = " << label << endl;
				write_output(label, output);
			}
			
			// premise
			else if(cpt_guillemet == 5)
			{
				stringstream premise_ss;
				for(; word[i] != '"'; ++i)
					premise_ss << word[i];	
				string premise = premise_ss.str();
				--i; //unread the "
			//	cerr << "premise = " << premise << endl;
				write_output(premise, word_to_id, output);			
			}
			
			// hypothesis
			else if(cpt_guillemet == 7)
			{
				stringstream hypothesis_ss;
				for(; word[i] != '"'; ++i)
					hypothesis_ss << word[i];	
				string hypothesis = hypothesis_ss.str();
				--i; //unread the "
			//	cerr << "hypothesis = " << hypothesis << endl ;
				write_output(hypothesis, word_to_id, output);			
			}
			
			// couple ex : "(Male,guy),(blue jacket,blue jacket),(lay,laying)"
			else if(cpt_guillemet == 13)
			{
				stringstream couple_ss;
				for(; word[i] != '"'; ++i)
					couple_ss << word[i];		
				string couple = couple_ss.str();
				--i;
			//	cerr << "couple = " << couple << "\n\n";
				write_output_couple(couple, word_to_id, output);
			}
		}
	}
	explication_file.close();
	output.close();
	
}


int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique_file\n"
			 << "explication_file (format csv)\n"
			 << "output_name (explication file tokenized)\n";
		exit(EXIT_FAILURE);
	}
	
	generating_tokenizing_explication(argv[1], argv[2], argv[3]);
	
	return 0;
}


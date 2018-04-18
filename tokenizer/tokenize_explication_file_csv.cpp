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
	bool premise = true;
	unsigned j;
	string w;
	
	for(unsigned i=0; i<sentence.size(); ++i)
	{
		if(sentence[i] == '(' || sentence[i] == ',')
			continue;
		stringstream word_ss;
		stringstream position_ss;
		
		// extracting words
		for(;sentence[i] != '['; ++i)
			word_ss << sentence[i];
		++i;
		
		// extracting words' positions
		for(;sentence[i] != ']'; ++i)
			position_ss << sentence[i];
		++i;

		string word = word_ss.str();
		vector<string> words;
		std::stringstream().swap(word_ss); // flush word_ss
		string position = position_ss.str();
		vector<string> positions;
		std::stringstream().swap(position_ss); // flush position_ss
		
		// words
		for(j=0; j<word.size(); ++j)
		{
			if(word[j] == ' ')
			{
				w = word_ss.str();
				std::transform(w.begin(), w.end(), w.begin(), ::tolower); 
				words.push_back(w);
				std::stringstream().swap(word_ss); // flush word_ss
				++j;
			}
			word_ss << word[j];
		}
		w = word_ss.str();
		std::transform(w.begin(), w.end(), w.begin(), ::tolower); 
		words.push_back(w);
		
		// words' positions
		for(j=0; j<position.size(); ++j)
		{
			if(position[j] == ' ')
			{
				w = position_ss.str();
				positions.push_back(w);
				std::stringstream().swap(position_ss); // flush position_ss
				++j;
			}
			position_ss << position[j];			
		}
		w = position_ss.str();
		if(w.size() == 0)
			w = "-4";
		positions.push_back(w);

		for(j=0; j<words.size(); ++j)
		{
			if(words[j] == "-1")
				output << "0 " << positions[j] << " ";
			else
				output << word_to_id[words[j]] << " " << positions[j] << " ";
		}
		if(premise)
		{
			premise = false;
			output << "-2 ";
		}
		else
		{
			premise = true;
			output << "-1\n";
		}
	}
	output << "-5\n"; //end of the couples list
}

string extract_sequences_between_guillemet(unsigned& i, string line)
{
	stringstream ss;
	for(; line[i] != '"'; ++i)
		ss << line[i];		
	string extraction = ss.str();
	--i; // unread the "
	return extraction;
}

void reading_lexique(char* lexique_filename, map<string, unsigned>& word_to_id)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}	
	unsigned id;
	string word;
	while(lexique_file >> word && lexique_file >> id)
		word_to_id[word] = id;
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();
}

/* have this kind of form : (entailment),(neutral),(contradiction) */
void write_output_couple_label(string line, ofstream& output)
{
	string label;
	for(unsigned i=0; i<line.size(); ++i)
	{
		if(line[i] == '(' || line[i] == ',')
			continue;
		stringstream ss;
		while(line[i] != ')')
		{
			ss << line[i];
			++i;
		}
		++i; //read the ')'
		label = ss.str();
		write_output(label, output);
	}
	output << "-3\n"; //end of the tokenized file
}


void generating_tokenizing_explication(char* lexique_filename, char* explication_filename, char* output_filename)
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
	
	string word;
	map<string, unsigned> word_to_id;
	reading_lexique(lexique_filename, word_to_id);
	
	getline(explication_file, word); //read the label of each column in the csv file
	unsigned cpt_guillemet;
	string extract;
	//unsigned cpt=0;
	while(getline(explication_file, word))
	{
		cpt_guillemet=0;
	//	++cpt;
		// Extracting label, premise, hypothesis and couple
		for(unsigned i=0; i<word.size(); ++i)
		{
			if(word[i] == '"')
				++cpt_guillemet;
				
			// label
			else if(cpt_guillemet == 3)
			{
				extract = extract_sequences_between_guillemet(i, word);
				write_output(extract, output);
			}
			
			// premise and hypothesis
			else if(cpt_guillemet == 5 || cpt_guillemet == 7)
			{
				extract = extract_sequences_between_guillemet(i, word);
				write_output(extract, word_to_id, output);			
			}
			
			// couple ex : "(Male,guy),(blue jacket,blue jacket),(lay,laying)"
			else if(cpt_guillemet == 13)
			{
				extract = extract_sequences_between_guillemet(i, word);
				//cerr << "sample " << cpt << "\n\n";
				write_output_couple(extract, word_to_id, output);
			}
			
			// couple's label
			else if(cpt_guillemet == 15)
			{
				extract =  extract_sequences_between_guillemet(i, word);
				write_output_couple_label(extract, output); 
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


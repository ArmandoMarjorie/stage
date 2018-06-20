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

void write_output_label(string label, ofstream& output)
{
	if(label != "neutral" && label != "entailment" && label != "contradiction")
		output << "incorrect entry\n";
	else
		output << "label: " << label << endl;
}


void write_output_words(string sentence, ofstream& output)
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
		output <<"[" << word << "] ";
	}
	output << "-1 " << nb_words << endl;
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



void generating_words(char* explication_filename, char* output_filename)
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
	
	string word;
	
	getline(explication_file, word); //read the label of each column in the csv file
	unsigned cpt_guillemet;
	string extract;
	unsigned cpt=1;
	while(getline(explication_file, word))
	{
		cout << "\n\tSAMPLE " << cpt << endl;
		++cpt;
		cpt_guillemet=0;
		output << "-3\n";
		
		// Extracting label, premise, hypothesis and couple
		for(unsigned i=0; i<word.size(); ++i)
		{
			if(word[i] == '"')
				++cpt_guillemet;
				
			// label
			else if(cpt_guillemet == 3)
			{
				extract = extract_sequences_between_guillemet(i, word);
				write_output_label(extract, output);
			}
			
			// premise and hypothesis
			else if(cpt_guillemet == 5 || cpt_guillemet == 7)
			{
				if(cpt_guillemet == 5)
					output <<"premise -2\n";
				else
					output <<"hypothesis -2\n";
				extract = extract_sequences_between_guillemet(i, word);
				write_output_words(extract, output);			
			}
			

		}
		output << "-3\n\n\n";
	}
	explication_file.close();
	output.close();
	
}




int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "explication_file (format csv)\n"
			 << "output_name (explication file tokenized)\n";
		exit(EXIT_FAILURE);
	}
	
	generating_words(argv[1], argv[2]);
	
	return 0;
}


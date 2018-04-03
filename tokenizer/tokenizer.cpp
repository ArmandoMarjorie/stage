#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>

using namespace std;

string extract_label(string section, map<string, unsigned>& label_to_id, unsigned& code_label)
{
	stringstream raw_label;
	map< string, unsigned >::iterator it_label;
	for(unsigned i=1; i<section.size()-2; ++i)
		raw_label << section[i];
	string label = raw_label.str();
	if(label == "-")
		return label;
	it_label = label_to_id.find(label);
	if(it_label == label_to_id.end()) //we didn't see this label before
	{
		label_to_id[label] = code_label;
		++code_label;
	}
	return label;
}

void tokenize(char* snli_json_filename, char* tok_filename, char* id_tok_filename, map<string, unsigned>& word_to_id,
	map<string, unsigned>& label_to_id, unsigned& code_word, unsigned& code_label)
{
	ofstream text_tok(tok_filename, ios::out | ios::trunc);
	if(!text_tok)
	{
		cerr << "Problem with the output file " << tok_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream text_parsed(id_tok_filename, ios::out | ios::trunc);
	if(!text_parsed)
	{
		cerr << "Problem with the output file " << id_tok_filename << endl;
		exit(EXIT_FAILURE);
	}
	ifstream snli_file(snli_json_filename, ios::in);
	if(!snli_file)
	{ 
		cerr << "Impossible to open the file " << snli_json_filename << endl;
		exit(EXIT_FAILURE);
	}
	bool good_sentence = true;
	map< string, unsigned >::iterator it_word;
	
	string section;
	while(snli_file >> section)
	{
		/* Label */
		if(section == "\"gold_label\":")
		{
			snli_file >> section;
			string label = extract_label(section, label_to_id, code_label);
			if(label == "-")
			{
				good_sentence = false;
				continue;
			}
			else
				good_sentence = true;
			text_tok << label_to_id[label] << endl;
			text_parsed << label << endl;
		}
		/* Sentence 1 and 2 */
		else if(section == "\"sentence1_parse\":" || section == "\"sentence2_parse\":")
		{
			unsigned nb_words_in_sentence=0;
			if(!good_sentence) //do not use sentences without label !
				continue;
			/* Handling a sentence */
			while(snli_file >> section)
			{
				if(section == "\"sentence2\":" || section == "{\"annotator_labels\":") //we read all the words of the sentence
					break;				
				stringstream word;
				size_t found = section.find(")");
				if (found != -1)
				{
					for(unsigned j=0; j<found; ++j)
					{
						word << section[j];
					}
					string real_word = word.str();
					text_parsed << real_word << ' ';
					it_word = word_to_id.find(real_word);
					if(it_word == word_to_id.end()) //we didn't see this word before
					{
						word_to_id[real_word] = code_word;
						++code_word;
					}
					text_tok << word_to_id[real_word] << ' ';
					
					++nb_words_in_sentence;
					
				}	
			}
			text_parsed << "-1 " << nb_words_in_sentence << endl;
			text_tok << "-1 " << nb_words_in_sentence << endl;
		}
	}
	snli_file.close();
	text_tok.close();
	text_parsed.close();
}

int main(int argc, char** argv)
{
	if(argc != 10)
	{
		cerr << "Usage : " << argv[0] << "\n"
			 << "dev_file (in json format)\n"
			 << "name_tokenized_dev_file\n"
			 << "name_tokenized_id_dev_file\n"
			 
			 << "test_file (in json format)\n"
			 << "name_tokenized_test_file\n"
			 << "name_tokenized_id_test_file\n"
			 
			 << "train_file (in json format)\n"
			 << "name_tokenized_train_file\n"
			 << "name_tokenized_id_train_file\n" ;
		exit(EXIT_FAILURE);
	}
	map<string, unsigned> word_to_id;
	map<string, unsigned> label_to_id;
	unsigned code_word=1;
	unsigned code_label=0;
	
	for(unsigned i=1; i<= 7; i+=3)
		tokenize(argv[i], argv[i+1], argv[i+2], word_to_id, label_to_id, code_word, code_label);
	return 0;
}

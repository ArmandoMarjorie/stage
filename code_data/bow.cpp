#include <algorithm>
#include "bow.hpp"

using namespace std;

BagOfWords::~BagOfWords()
{
	words.clear();
	for(unsigned i=0; i < switch_words.size(); ++i)
		switch_words[i]->~SwitchWords();
}

BagOfWords::BagOfWords(string& line)
{
	if(line[0] == '{')
		important_bag = false;
	else if(line[0] == '[')
		important_bag = true;
	else
	{
		cout << "IMPORTANCE INCONNUE\n";
		exit(EXIT_FAILURE);
	}
	
	std::string::size_type sz;
	int wordID;	
	string word, word2;
	stringstream fluxstring(line);
	
	// Gère les expressions de la phrase
	fluxstring >> word;
	while(word[word.size()-1] != ']' && word[word.size()-1] != '}')
	{
		fluxstring >> word2;
		word = word + " ";
		word = word + word2;
	}
	for(unsigned i=1; i < word.size()-1; ++i)
	{
		stringstream ss;
		while(i < word.size()-1 && word[i] != ' ')
		{
			ss << word[i];
			++i;
		}
		word2 = ss.str();
		wordID = std::stoi(word2,&sz);
		
		words.push_back(static_cast<unsigned>(wordID));
		//cout << "\tID = " << wordID << " , mot = " << word << " (" <<  important_bag << ") "<< endl;
	}	
	
	// Gère les mots de remplacements
	if(important_bag)
	{
		unsigned nb_expr;
		fluxstring >> nb_expr;
		for(int nb=0 ; nb < nb_expr; ++nb)
		{
			switch_words.push_back(new SwitchWords(fluxstring));
		
		}
	}
}



BagOfWords::BagOfWords(const vector<unsigned>& wordsID, bool imp)
{
	for(unsigned i=0; i<wordsID.size(); ++i)
		words.push_back(wordsID[i]);
	important_bag = imp;
}

BagOfWords::BagOfWords(BagOfWords const& copy)
{
	for(unsigned i=0; i < copy.words.size(); ++i)
		this->words.push_back( copy.words[i] );
		
	for(unsigned i=0; i<copy.switch_words.size(); ++i)
		this->switch_words.push_back( new SwitchWords( *(copy.switch_words[i]) ) );
	
	this->important_bag = copy.important_bag;
}

unsigned BagOfWords::get_nb_words()
{
	return words.size();
}

unsigned BagOfWords::get_word_id(unsigned num_words)
{
	return words[num_words];
	
}

bool BagOfWords::expr_is_important()
{
	return important_bag;
	
}

void BagOfWords::modif_BoW(unsigned num_switch_words, unsigned num_sw, bool imp)
{
	cout << "\tmodif bow...\n";
	words.clear();
	unsigned nb_expr_in_sw = switch_words[num_switch_words]->get_nb_expr_sw(num_sw);
	cout << "nb de mot dans la sw = " << nb_expr_in_sw << endl;
	
	for(unsigned i=0; i < nb_expr_in_sw; ++i)
		words.push_back(switch_words[num_switch_words]->get_word_id_sw(num_sw, i));
		
	important_bag = imp;
}
void BagOfWords::modif_BoW_random(bool imp)
{
	cout << "\tmodif bow...\n";
	words.clear();
	
	// (rand() % (MAX - MIN + 1)) + MIN;
	unsigned alea = (rand() % (36988)) + 1;
	words.push_back(alea);
		
	important_bag = imp;
}
void BagOfWords::modif_BoW(BagOfWords const& current_bow, unsigned num_switch_words, unsigned num_sw, bool imp)
{
	cout << "\tmodif bow...\n";
	words.clear();
	unsigned nb_expr_in_sw = current_bow.switch_words[num_switch_words]->get_nb_expr_sw(num_sw);
	cout << "nb de mot dans la sw = " << nb_expr_in_sw << endl;
	
	for(unsigned i=0; i < nb_expr_in_sw; ++i)
		words.push_back(current_bow.switch_words[num_switch_words]->get_word_id_sw(num_sw, i));
		
	important_bag = imp;
}

void BagOfWords::modif_BoW(BagOfWords& bow) //appelé quand on reset l'instance
{
	this->words.clear();
	for(unsigned i=0; i < bow.words.size(); ++i)
		this->words.push_back(bow.words[i]);
	this->important_bag = bow.important_bag;
}

void BagOfWords::print_a_sample()
{
	for(unsigned i=0; i<words.size(); ++i)
		cout << words[i] << " ";
}

unsigned BagOfWords::get_nb_switch_words()
{
	return switch_words.size();
}

unsigned BagOfWords::get_type_sw(unsigned num_switch_word, unsigned num_sw)
{
	return switch_words[num_switch_word]->get_type_sw(num_sw);
}

unsigned BagOfWords::get_nb_of_sw(unsigned num_switch_word)
{
	return switch_words[num_switch_word]->get_nb_of_sw();
}

void BagOfWords::print()
{
	for(unsigned i=0; i<switch_words.size(); ++i)
	{
		switch_words[i]->print();
	}
}

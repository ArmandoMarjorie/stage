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

void BagOfWords::modif_BoW(unsigned mot_inconnu_ID, bool imp)
{
	//cout << "COUCOU\n";
	words.clear();
	//cout << "CLEAR LE VECTEUR\n";
	words.push_back(mot_inconnu_ID);
	//cout << "MIS 0 DEDANS\n";
	important_bag = imp;
	//cout << "FINI !\n";
}

void BagOfWords::modif_BoW(BagOfWords& bow)
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

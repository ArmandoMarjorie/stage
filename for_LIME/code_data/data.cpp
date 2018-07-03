#include "data.hpp"
#include <algorithm>

using namespace std;
using namespace dynet;

/**
 * \file data.cpp
*/



BagOfWords::BagOfWords(string& word)
{
	if(word[0] == '{')
		important_bag = false;
	std::string::size_type sz;
	int wordID;
	string w;
	for(unsigned i=1; i < word.size()-1; ++i)
	{
		stringstream ss;
		while(i < word.size()-1 && word[i] != ' ')
		{
			ss << word[i];
			++i;
		}
		w = ss.str();
		wordID = std::stoi(w,&sz);
		
		words.push_back(static_cast<unsigned>(wordID));
		//cout << "\tID = " << wordID << " , mot = " << word << " (" <<  important_bag << ") "<< endl;
	}
	
}

unsigned BagOfWords::get_nb_words()
{
	return words.size();
}

unsigned BagOfWords::get_word_id(unsigned num_words)
{
	return words[num_words];
	
}

Data::Data(ifstream& database)
{
	string word, word2;
	database >> label;
	
	unsigned nb_bow;
	
	for(unsigned nb_sentences=0; nb_sentences < 2; ++nb_sentences)
	{
		database >> nb_bow;
		
		for(unsigned nb=0; nb < nb_bow; ++nb)
		{
			database >> word;
			while(word[word.size()-1] != ']' && word[word.size()-1] != '}')
			{
				database >> word2;
				word = word + " ";
				word = word + word2;
			}
			
			BagOfWords bow(word);
			if(nb_sentences==0)
				premise.push_back(bow);
			else
				hypothesis.push_back(bow);
				
			getline(database, word); //ignore mot de remplacement
		}	
	}
	
	database >> word; //lit -3 de fin
}

unsigned Data::get_label()
{
	return label;
}

unsigned Data::get_nb_expr(unsigned sentence)
{
	if(sentence==1)
		return premise.size();
	return hypothesis.size();
}


unsigned Data::get_nb_words(unsigned sentence, unsigned num_expr)
{
	if(sentence==1)
		return premise[num_expr].get_nb_words();
	return hypothesis[num_expr].get_nb_words();
}

unsigned Data::get_word_id(unsigned sentence, unsigned num_expr, unsigned num_words)
{
	if(sentence==1)
		return premise[num_expr].get_word_id(num_words);
	return hypothesis[num_expr].get_word_id(num_words);
}

DataSet::DataSet(char* filename)
{
	ifstream database(filename, ios::in);
	if(!database)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	int word;
	unsigned i=0, lab;
	while(database >> word)
	{
		//cout << "SAMPLE " << i << endl;
		Data data(database);
		//cout << endl << endl;
		dataset.push_back(data);
		lab = dataset[i].get_label();
		++i;
		
		switch(lab)
		{
			case 0:
			{
				++nb_neutral;
				break;
			}
			case 1:
			{
				++nb_inf;
				break;
			}			
			case 2:
			{
				++nb_contradiction;
				break;
			}			
		}
	}
	
	database.close();
}

unsigned DataSet::get_word_id(unsigned sentence, unsigned num_sample, unsigned num_expr, unsigned num_words)
{
	return dataset[num_sample].get_word_id(sentence, num_expr, num_words);
}

unsigned DataSet::get_nb_words(unsigned sentence, unsigned num_sample, unsigned num_expr)
{
	return dataset[num_sample].get_nb_words(sentence, num_expr);
	
}

unsigned DataSet::get_nb_expr(unsigned sentence, unsigned num_sample)
{
	return dataset[num_sample].get_nb_expr(sentence);
}


unsigned DataSet::get_nb_inf(){return nb_inf;}
unsigned DataSet::get_nb_neutral(){return nb_neutral;}
unsigned DataSet::get_nb_contradiction(){return nb_contradiction;}

unsigned DataSet::get_nb_sentences()
{
	return dataset.size();
}

unsigned DataSet::get_label(unsigned num_sample)
{
	return dataset[num_sample].get_label();
}
















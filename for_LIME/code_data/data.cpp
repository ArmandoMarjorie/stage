#include "data.hpp"
#include <algorithm>

using namespace std;
using namespace dynet;

/**
 * \file data.cpp
*/

BagOfWords::~BagOfWords()
{
	words.clear();
}

BagOfWords::BagOfWords(string& word)
{
	if(word[0] == '{')
		important_bag = false;
	else if(word[0] == '[')
		important_bag = true;
	else
	{
		cout << "IMPORTANCE INCONNUE\n";
		exit(EXIT_FAILURE);
	}
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


Data::~Data()
{
	for(unsigned i=0; i<premise.size(); ++i)
		delete premise[i];
	
	for(unsigned i=0; i<hypothesis.size(); ++i)
		delete hypothesis[i];
	
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
			
			//BagOfWords bow = new BagOfWords(word);
			if(nb_sentences==0)
				premise.push_back(new BagOfWords(word));
			else
				hypothesis.push_back(new BagOfWords(word));
				
			getline(database, word); //ignore mot de remplacement
		}	
	}
	
	database >> word; //lit -3 de fin
}

Data::Data(Data const& copy)
{
	for(unsigned i=0; i<copy.premise.size(); ++i)
		this->premise.push_back(new BagOfWords( *(copy.premise[i]) ));
	for(unsigned i=0; i<copy.hypothesis.size(); ++i)
		this->hypothesis.push_back(new BagOfWords( *(copy.hypothesis[i]) ) );
	this->label = copy.label;
	
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
		return premise[num_expr]->get_nb_words();
	return hypothesis[num_expr]->get_nb_words();
}

unsigned Data::get_word_id(unsigned sentence, unsigned num_expr, unsigned num_words)
{
	if(sentence==1)
		return premise[num_expr]->get_word_id(num_words);
	return hypothesis[num_expr]->get_word_id(num_words);
}


unsigned Data::get_nb_imp_words(bool is_premise)
{
	unsigned cpt=0;
	if(is_premise)
	{
		for(unsigned i=0; i<premise.size(); ++i)
			if(premise[i]->expr_is_important())
				++cpt;
	}
	else
	{
		for(unsigned i=0; i<hypothesis.size(); ++i)
			if(hypothesis[i]->expr_is_important())
				++cpt;
	}	
	
	return cpt;
}

unsigned Data::search_position(bool is_premise, unsigned num_buffer_in)
{
	unsigned nb_imp=0;
	unsigned position=0;
	//cout << "je cherche la position\n";	
	if(is_premise)
	{
		while(nb_imp != num_buffer_in)
		{
			if( premise[position]->expr_is_important() )
			{
				//cout << "(POS " << position << " est imp) ";  
				++nb_imp;
			}
			++position;
		}
		//cout << "[PREM] ";
	}
	else
	{
		//cout << "je vais chercher dans l'hypothese\n";
		while(nb_imp < num_buffer_in) //erreur de segmentation ici WTF !!!!
		{
			//cout << "(search) position = " << position << " ";
			if( hypothesis[position]->expr_is_important() )
				++nb_imp;
			++position;
		}
		//cout << "trouve!\n";
	}
	
	//cout << "(POS) " << static_cast<int>(position-1) << " "; 
	return position-1;
	
}

void Data::modif_LIME(bool is_premise, unsigned num_buffer_in, unsigned position)
{
	
	if(is_premise)
	{
		//cout << "MODIF DE " << premise[position]->get_word_id(0) << " EN 0\n";
		premise[position]->modif_BoW(0, premise[position]->expr_is_important());
	}
	else
	{
		//cout << "MODIF DE " << hypothesis[position]->get_word_id(0) << " EN 0\n";
		hypothesis[position]->modif_BoW(0, hypothesis[position]->expr_is_important());
	}
}

void Data::reset_data(Data const& data_copy)
{
	for(unsigned i=0; i<data_copy.premise.size(); ++i)
		this->premise[i]->modif_BoW(*(data_copy.premise[i]));
		
	for(unsigned i=0; i<data_copy.hypothesis.size(); ++i)
		this->hypothesis[i]->modif_BoW(*(data_copy.hypothesis[i]));
		
	this->label = data_copy.label;	
}


bool Data::expr_is_important(bool is_premise, unsigned num_expr)
{
	if(is_premise)
		return premise[num_expr]->expr_is_important();
	return hypothesis[num_expr]->expr_is_important();
}

void Data::print_a_sample()
{
	char c_o, c_f;
	cout << "\tPREMISE : \n\t";
	for(unsigned i=0; i<premise.size(); ++i)
	{
		if(premise[i]->expr_is_important())
		{
			c_o = '['; c_f = ']';
		}
		else
		{
			c_o = '{'; c_f = '}';
		}
		cout << c_o;
		premise[i]->print_a_sample();
		cout << c_f;
	}
		
	cout << "\n\tHYPOTHESIS : \n\t";
	for(unsigned i=0; i<hypothesis.size(); ++i)
	{
		if(hypothesis[i]->expr_is_important())
		{
			c_o = '['; c_f = ']';
		}
		else
		{
			c_o = '{'; c_f = '}';
		}
		cout << c_o;
		hypothesis[i]->print_a_sample();
		cout << c_f;
	}
	cout << endl;
}

Data* Data::get_data_object()
{
	return this;
}

DataSet::~DataSet()
{
	for(unsigned i=0; i<dataset.size(); ++i)
		dataset[i]->~Data();
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
		//Data data(database);
		//cout << endl << endl;
		dataset.push_back(new Data(database));
		lab = dataset[i]->get_label();
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


Data* DataSet::get_data_object(unsigned num_sample)
{
	return dataset[num_sample]->get_data_object();
}

bool DataSet::expr_is_important(unsigned num_sample, bool is_premise, unsigned num_expr)
{
	return dataset[num_sample]->expr_is_important(is_premise, num_expr);
}


unsigned DataSet::get_word_id(unsigned sentence, unsigned num_sample, unsigned num_expr, unsigned num_words)
{
	return dataset[num_sample]->get_word_id(sentence, num_expr, num_words);
}

//nb de mot dans l'expression num_expr
unsigned DataSet::get_nb_words(unsigned sentence, unsigned num_sample, unsigned num_expr)
{
	return dataset[num_sample]->get_nb_words(sentence, num_expr);
	
}

unsigned DataSet::get_nb_expr(unsigned sentence, unsigned num_sample)
{
	return dataset[num_sample]->get_nb_expr(sentence);
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
	return dataset[num_sample]->get_label();
}



void DataSet::print_a_sample(unsigned num_sample)
{
	dataset[num_sample]->print_a_sample();
}

void DataSet::print_everything()
{
	for(unsigned i=0; i<dataset.size(); ++i)
	{
		cout << "SAMPLE " << i << endl;
		dataset[i]->print_a_sample();
		cout << endl << endl;
	}
}

void DataSet::modif_LIME(char* buffer_in, unsigned num_sample)
{
	unsigned cpt_crochet=0;
	unsigned nbr_expr=0;
	unsigned nb_word;
	unsigned pos;
	string word;
	
	
	unsigned nb_imp_words_prem = dataset[num_sample]->get_nb_imp_words(true);
	
	for(unsigned i=0; i < strlen(buffer_in)-1; ++i)
	{
		
		if(buffer_in[i] == '[' || buffer_in[i] == ' ')
			continue;
		
		
		stringstream ss;
		while(i < strlen(buffer_in)-1 && buffer_in[i] != ']')
		{
			ss << buffer_in[i];
			++i;
			cout << buffer_in[i];
		}		
		++cpt_crochet;
		++nbr_expr;
		word = ss.str();
		
		if(word == "UNKWORDZ")
		{		
			if(nbr_expr <= nb_imp_words_prem)
			{
				pos = dataset[num_sample]->search_position(true, cpt_crochet);
				//modif
				dataset[num_sample]->modif_LIME(true, cpt_crochet, pos);
			}
			else
			{
				pos = dataset[num_sample]->search_position(false, cpt_crochet);
				//modif
				dataset[num_sample]->modif_LIME(false, cpt_crochet, pos); 
			}			
		
		}
		if(nbr_expr == nb_imp_words_prem)
		{
			cout << "\"HYPOTHESE\"";
			cpt_crochet = 0;
		}
	}
}

void DataSet::reset_data(Data const& data_copy, unsigned num_sample)
{
	dataset[num_sample]->reset_data(data_copy);
}









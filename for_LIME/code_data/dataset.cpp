#include "dataset.hpp"
#include <algorithm>

using namespace std;

/**
 * \file data.cpp
*/







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









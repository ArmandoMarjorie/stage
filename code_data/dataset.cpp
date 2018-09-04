#include "dataset.hpp"
#include <algorithm>

using namespace std;


/**
 * \file dataset.cpp
*/


	/* Constructors, Destructor */


/**
	* \brief DataSet Destructor. Release the allocated memory.
*/
DataSet::~DataSet()
{
	for(unsigned i=0; i<dataset.size(); ++i)
		dataset[i]->~Data();
}


/**
	* \brief DataSet Constructor. Initializes the label, the premise and 
	* the hypothesis of every instance.
	*
	* \param filename : file containing every instances.
*/
DataSet::DataSet(char* filename)
{
	ifstream database(filename, ios::in);
	if(!database)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	int word;
	unsigned num_sample=0, lab;
	while(database >> word)
	{
		dataset.push_back(new Data(database));
		lab = dataset[num_sample]->get_label();
		
		init_labels_infos(lab, num_sample);
		
		++num_sample;
	}
	
	database.close();
}


inline void DataSet::init_labels_infos(unsigned lab, unsigned num_sample)
{
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
		default:
		{
			cerr << "This is not a valid label (instance " 
				 << num_sample << ")\n";
			exit(EXIT_FAILURE);
		}			
	}
}


/**
	* \brief Data constructor not used for interpretation
	* 
	* \param data_filename : File containing the samples in this form :
	*       label
	*       premise -1 premise's length
	*       hypothesis -1 hypothesis' length
*/
DataSet::DataSet(char* data_filename, int not_interpret)
{
	ifstream database(data_filename, ios::in);
	if(!database)
	{ 
		cerr << "Impossible to open the file " << data_filename << endl;
		exit(EXIT_FAILURE);
	}
	int word;
	unsigned num_sample=0, lab;
	while(database >> lab)
	{
		dataset.push_back(new Data(database, lab));
		init_labels_infos(lab, num_sample);
		++num_sample;
	}
	
	database.close();
}


	/* Getters */


/**
	* \name get_data_object
	* \brief Returns the "num_sample"th instance to be saved.
	* 
	* \return The "num_sample"th instance.
*/
Data* DataSet::get_data_object(unsigned num_sample)
{
	return dataset[num_sample]->get_data_object();
}


/**
	* \name expr_is_important
	* \brief Tells if the "num_expr"th expression's importance is evaluated 
	* (in the premise or in the hypothesis from the "num_sample"th instance).
	* 
	* \param num_sample : numero of the instance.
	* \param is_premise : true if we process the premise, else false.
	* \param num_expr : numero of the expression in the premise or in the hypothesis.
	* 
	* \return True if the "num_expr"th expression's importance is evaluated, else false.
*/
bool DataSet::expr_is_important(unsigned num_sample, bool is_premise, unsigned num_expr)
{
	return dataset[num_sample]->expr_is_important(is_premise, num_expr);
}


/**
	* \name get_word_id
	* \brief Gives the word ID of the "num_words"th word in the "num_expr"th expression (in the premise or in the hypothesis, 
	* from the "num_sample"th instance).
	* For example : "an animal in front of a forest"
	* premise[2] = "in front of"
	* num_expr = 2, num_words = 0
	* Then it returns "in"'s ID.
	* 
	* \param is_premise : true if we process the premise, else false.
	* \param num_sample : numero of the instance.
	* \param num_expr : numero of the expression in the premise or in the hypothesis.
	* \param num_words : numero of the word in the expression (in the premise or in the hypothesis).
	* 
	* \return The word ID of the "num_words"th word in the "num_expr"th expression (in the premise or in the hypothesis).
*/
unsigned DataSet::get_word_id(bool is_premise, unsigned num_sample, unsigned num_expr, unsigned num_words)
{
	return dataset[num_sample]->get_word_id(is_premise, num_expr, num_words);
}


/**
	* \name get_nb_words
	* \brief Gives the number of words in the "num_expr"th expression (in the premise or in the hypothesis, 
	* from the "num_sample"th instance).
	* For example : "an animal in front of a forest"
	* premise[2] = "in front of"
	* Then it returns 3.
	* 
	* \param is_premise : true if we process the premise, else false.
	* \param num_sample : numero of the instance.
	* \param num_expr : numero of the expression in the premise or in the hypothesis.
	* 
	* \return The number of words in the "num_expr"th expressio in the premise or in the hypothesis.
*/
unsigned DataSet::get_nb_words(bool is_premise, unsigned num_sample, unsigned num_expr)
{
	return dataset[num_sample]->get_nb_words(is_premise, num_expr);
	
}


/**
	* \name get_nb_expr
	* \brief Gives the number of expressions, in the premise or in the hypothesis, 
	* from the "num_sample"th instance.
	* 
	* \param is_premise : true if we process the premise, else false.
	* \param num_sample : numero of the instance.
	* 
	* \return The number of expressions in the premise or in the hypothesis.
*/
unsigned DataSet::get_nb_expr(bool is_premise, unsigned num_sample)
{
	return dataset[num_sample]->get_nb_expr(is_premise);
}


/**
	* \name get_nb_inf
	* \brief Gives the number of Entailment instances.
	* 
	* \return The number of Entailment instances.
*/
unsigned DataSet::get_nb_inf(){return nb_inf;}


/**
	* \name get_nb_inf
	* \brief Gives the number of Neutral instances.
	* 
	* \return The number of Neutral instances.
*/
unsigned DataSet::get_nb_neutral(){return nb_neutral;}


/**
	* \name get_nb_inf
	* \brief Give the number of Contradiction instances.
	* 
	* \return The number of Contradiction instances.
*/
unsigned DataSet::get_nb_contradiction(){return nb_contradiction;}


/**
	* \name get_nb_instances
	* \brief Gives the number of instances.
	* 
	* \return The number of instances.
*/
unsigned DataSet::get_nb_instances()
{
	return dataset.size();
}


/**
	* \name get_label
	* \brief Gives the "num_sample"th instance's label.
	* 
	* \return The "num_sample"th instance's label.
*/
unsigned DataSet::get_label(unsigned num_sample)
{
	return dataset[num_sample]->get_label();
}


/**
	* \name get_nb_switch_words
	* \brief Give the number of alternative expressions for the "num_expr"th 
	* expression in the "num_sample"th instance.
	* 
	* \param is_premise : true if we process the premise, else false.
	* \param num_expr : numero of the expression in the premise or in the hypothesis.
	* \param num_sample : numero of the instance.
	* 
	* \return The number of alternative expressions for the "num_expr"th 
	* expression in the "num_sample"th instance.
*/
unsigned DataSet::get_nb_switch_words(bool is_premise, unsigned num_expr, unsigned num_sample)
{
	return dataset[num_sample]->get_nb_switch_words(is_premise, num_expr);
}


	/* Setters (instances modifications) */


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
		cout << endl;
		++cpt_crochet;
		++nbr_expr;
		word = ss.str();
		
		if(word == "UNKWORDZ")
		{		
			if(nbr_expr <= nb_imp_words_prem)
			{
				pos = dataset[num_sample]->search_position(true, cpt_crochet);
				if(original_lime)
					dataset[num_sample]->modif_LIME_original(true, pos); 
				else
					dataset[num_sample]->modif_LIME(true, pos);
			}
			else
			{
				pos = dataset[num_sample]->search_position(false, cpt_crochet);
				if(original_lime)
					dataset[num_sample]->modif_LIME_original(false, pos); 
				else
					dataset[num_sample]->modif_LIME(false, pos); 
			}			
		
		}
		if(nbr_expr == nb_imp_words_prem)
		{
			cout << "\"HYPOTHESE\"";
			cpt_crochet = 0;
		}
	}
}

void DataSet::modif_LIME_random(char* buffer_in, unsigned num_sample)
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
		cout << endl;
		++cpt_crochet;
		++nbr_expr;
		word = ss.str();
		
		if(word == "UNKWORDZ")
		{		
			if(nbr_expr <= nb_imp_words_prem)
			{
				pos = dataset[num_sample]->search_position(true, cpt_crochet);
				//modif
				dataset[num_sample]->modif_LIME_random(true, pos);
			}
			else
			{
				pos = dataset[num_sample]->search_position(false, cpt_crochet);
				//modif
				dataset[num_sample]->modif_LIME_random(false, pos); 
			}			
		
		}
		if(nbr_expr == nb_imp_words_prem)
		{
			cout << "\"HYPOTHESE\"";
			cpt_crochet = 0;
		}
	}
}


	/* Others */


/**
	* \name reset_data
	* \brief Reset the "num_sample"th instance with the original "num_sample"th instance.
	* 
	* \param data_copy : the original instance we want to reset with.
*/
void DataSet::reset_data(Data const& data_copy, unsigned num_sample)
{
	dataset[num_sample]->reset_data(data_copy);
}


/**
	* \name print_a_sample
	* \brief Print the "num_sample"th instance.
*/
void DataSet::print_a_sample(unsigned num_sample)
{
	dataset[num_sample]->print_a_sample();
}


/**
	* \name print_everything
	* \brief Print all instances.
*/
void DataSet::print_everything()
{
	for(unsigned i=0; i<dataset.size(); ++i)
	{
		cout << "SAMPLE " << i << endl;
		dataset[i]->print_a_sample();
		cout << endl << endl;
	}
}

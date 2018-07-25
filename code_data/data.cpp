#include "data.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;


/**
 * \file data.cpp
*/


	/* Constructors, Destructor */


/**
	* \brief SwitchWords Destructor. Release the allocated memory.
*/
Data::~Data()
{
	for(unsigned i=0; i<premise.size(); ++i)
		premise[i]->~BagOfWords();
	
	for(unsigned i=0; i<hypothesis.size(); ++i)
		hypothesis[i]->~BagOfWords();
	
}


/**
	* \brief Data Constructor. Initializes the label, the premise and 
	* the hypothesis of an instance.
	*
	* \param database : file containing every instances.
*/
Data::Data(ifstream& database)
{
	string word, word2;
	database >> label;
	
	unsigned nb_expr; 
	
	for(unsigned nb_sentences=0; nb_sentences < 2; ++nb_sentences)
	{
		database >> nb_expr; 
		getline(database, word);
		
		// for each expression/word in the premise or the hypothesis
		for(unsigned nb=0; nb < nb_expr; ++nb)
		{
			// read the instance's expression with its alternative expressions
			getline(database, word);
			
			if(nb_sentences==0)
				premise.push_back(new BagOfWords(word));
			else
				hypothesis.push_back(new BagOfWords(word));
		}	
	}
	
	database >> word; // read -3 (end symbol for an instance)
}


/**
	* \brief Data copy Constructor.
	*
	* \param copy : the Data we want to copy.
*/
Data::Data(Data const& copy)
{
	for(unsigned i=0; i<copy.premise.size(); ++i)
		this->premise.push_back(new BagOfWords( *(copy.premise[i]) ));
	for(unsigned i=0; i<copy.hypothesis.size(); ++i)
		this->hypothesis.push_back(new BagOfWords( *(copy.hypothesis[i]) ) );
	this->label = copy.label;
	
}


	/* Getters */
	
/* ICI */
/**
	* \name get_nb_switch_words
	* \brief Give the id of the "num_word_in_sw"th word in the "num_sw"th alternative expression's piece. For example:
	* switch_w[0] = "a", PREV
	* switch_w[1] = "big dog", ACTUAL
	* And you want the "big"'s ID, then num_sw = 1 and num_word_in_sw = 0.
	* 
	* \param num_sw : numero of the "piece" of the alternative expression. 
	* \param num_word_in_sw : numero of the word in the "piece" of the alternative expression.
	* 
	* \return The id of the "num_word_in_sw"th word in the "num_sw"th alternative expression's piece.
*/
unsigned Data::get_nb_switch_words(bool is_premise, unsigned num_expr)
{
	if(is_premise)
		return premise[num_expr]->get_nb_switch_words();
	return hypothesis[num_expr]->get_nb_switch_words();
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
	if(is_premise)
	{
		while(nb_imp != num_buffer_in)
		{
			if( premise[position]->expr_is_important() ) 
				++nb_imp;
			++position;
		}
	}
	else
	{
		while(nb_imp < num_buffer_in) 
		{
			if( hypothesis[position]->expr_is_important() )
				++nb_imp;
			++position;
		}
	}
	
	return position-1;
	
}

void Data::modif_LIME(bool is_premise, unsigned position)
{
	cout << "MODIF LIME\n";
	unsigned nb_switch_words = get_nb_switch_words(is_premise, position); //nb d'expr (de switch words) pouvant remplacer le bow courant
	cout << "nb d'expr (de switch words) pouvant remplacer le bow courant = " <<nb_switch_words << endl;
	
	unsigned alea = rand() % (nb_switch_words); // le numéro du switch words choisi
	cout << "switch choisi = " << alea << endl;
	
	unsigned replacing_word;
	unsigned nb_word_in_sw;
	if(is_premise)
		nb_word_in_sw = premise[position]->get_nb_of_sw(alea); //nb de bloc "SW" dans l'expression qui va remplacer 
	else
		nb_word_in_sw = hypothesis[position]->get_nb_of_sw(alea);
	
	cout << "nb_word_in_sw = " <<nb_word_in_sw << endl;

	unsigned type;
	//replacing_word = switch_words[alea]->get_switch_word();	

	for(unsigned i=0; i < nb_word_in_sw; ++i)
	{
		if(is_premise)
		{
			type = premise[position]->get_type_sw(alea, i); //le bloc numéro i du numéro 'alea' du switch word choisi
			if(type == PREV) 
			{
				cout << " PREV \n";
				if(position-1 >= 0)
					premise[position-1]->modif_BoW(*(premise[position]), alea, i, premise[position-1]->expr_is_important());
				else
				{
					cout << "le prev a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}				
			}				
			else if(type == ACTUAL)
			{
				cout << " ACTUAL \n";
				premise[position]->modif_BoW(alea, i, premise[position]->expr_is_important());
			}
			else if(type == NEXT)
			{
				
				if(position+1 < premise.size())
				{
					cout << " NEXT (position = " << position+1 << " )\n";
					premise[position+1]->modif_BoW(*(premise[position]), alea, i, premise[position+1]->expr_is_important());
					cout << "OK \n";
				}
				else
				{
					cout << "le next a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}
				
			}
		}
		else
		{
			type = hypothesis[position]->get_type_sw(alea, i); //le bloc numéro i du numéro 'alea' du switch word choisi
			if(type == PREV)
			{ 
				if(position-1 >= 0)
					hypothesis[position-1]->modif_BoW(*(hypothesis[position]), alea, i, hypothesis[position-1]->expr_is_important());
				else
				{
					cout << "le prev a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}				
			}
			else if(type == ACTUAL)
				hypothesis[position]->modif_BoW(alea, i, hypothesis[position]->expr_is_important());
			else if(type == NEXT)
			{
				if(position+1 < hypothesis.size())
					hypothesis[position+1]->modif_BoW( *(hypothesis[position]), alea, i, hypothesis[position+1]->expr_is_important());	
				else
				{
					cout << "le next a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}
			}	
			
		}
	}
}


void Data::modif_word(bool is_premise, unsigned num_expr, unsigned num_sw_words)
{
	cout << "MODIF WORD\n";

	unsigned replacing_word;
	unsigned nb_word_in_sw;
	if(is_premise)
		nb_word_in_sw = premise[num_expr]->get_nb_of_sw(num_sw_words); //nb de bloc "SW" dans l'expression qui va remplacer 
	else
		nb_word_in_sw = hypothesis[num_expr]->get_nb_of_sw(num_sw_words);
	
	cout << "nb_word_in_sw = " <<nb_word_in_sw << endl;

	unsigned type;

	for(unsigned i=0; i < nb_word_in_sw; ++i)
	{
		if(is_premise)
		{
			type = premise[num_expr]->get_type_sw(num_sw_words, i); //le bloc numéro i du switch word numéro "num_sw_words"
			if(type == PREV) 
			{
				cout << " PREV \n";
				if(num_expr-1 >= 0)
					premise[num_expr-1]->modif_BoW(*(premise[num_expr]), num_sw_words, i, premise[num_expr-1]->expr_is_important());
				else
				{
					cout << "le prev a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}				
			}				
			else if(type == ACTUAL)
			{
				cout << " ACTUAL \n";
				premise[num_expr]->modif_BoW(num_sw_words, i, premise[num_expr]->expr_is_important());
			}
			else if(type == NEXT)
			{
				
				if(num_expr+1 < premise.size())
				{
					cout << " NEXT (num_expr = " << num_expr+1 << " )\n";
					premise[num_expr+1]->modif_BoW(*(premise[num_expr]), num_sw_words, i, premise[num_expr+1]->expr_is_important());
					cout << "OK \n";
				}
				else
				{
					cout << "le next a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}
				
			}
		}
		else
		{
			type = hypothesis[num_expr]->get_type_sw(num_sw_words, i); //le bloc numéro i du numéro 'num_sw_words' du switch word choisi
			if(type == PREV)
			{ 
				if(num_expr-1 >= 0)
					hypothesis[num_expr-1]->modif_BoW(*(hypothesis[num_expr]), num_sw_words, i, hypothesis[num_expr-1]->expr_is_important());
				else
				{
					cout << "le prev a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}				
			}
			else if(type == ACTUAL)
				hypothesis[num_expr]->modif_BoW(num_sw_words, i, hypothesis[num_expr]->expr_is_important());
			else if(type == NEXT)
			{
				if(num_expr+1 < hypothesis.size())
					hypothesis[num_expr+1]->modif_BoW( *(hypothesis[num_expr]), num_sw_words, i, hypothesis[num_expr+1]->expr_is_important());	
				else
				{
					cout << "le next a dépassé le tableau\n";
					exit(EXIT_FAILURE);
				}
			}	
			
		}
	}
}
void Data::modif_LIME_random(bool is_premise, unsigned position)
{
	cout << "MODIF LIME\n";

	
	if(is_premise)
		premise[position]->modif_BoW_random(premise[position]->expr_is_important());
	else
		hypothesis[position]->modif_BoW_random(hypothesis[position]->expr_is_important());	

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
		premise[i]->print();
		cout << endl;
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
		hypothesis[i]->print();
		cout << endl;
	}
	cout << endl;
	
}

Data* Data::get_data_object()
{
	return this;
}

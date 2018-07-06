#include "data.hpp"
#include <algorithm>

using namespace std;


Data::~Data()
{
	for(unsigned i=0; i<premise.size(); ++i)
		premise[i]->~BagOfWords();
	
	for(unsigned i=0; i<hypothesis.size(); ++i)
		hypothesis[i]->~BagOfWords();
	
}

Data::Data(ifstream& database)
{
	string word, word2;
	database >> label;
	
	unsigned nb_bow;
	
	for(unsigned nb_sentences=0; nb_sentences < 2; ++nb_sentences)
	{
		database >> nb_bow; 
		cout << "lu nb_bow : \"" << nb_bow << "\"\n";
		getline(database, word);
		cout << "lu apres nb_bow : \"" << word << "\"\n";
		
		for(unsigned nb=0; nb < nb_bow; ++nb)
		{
			//database >> word;
			getline(database, word);
			cout << "lu : \"" << word << "\"\n";
			/*while(word[word.size()-1] != ']' && word[word.size()-1] != '}')
			{
				database >> word2;
				word = word + " ";
				word = word + word2;
			}*/
			
			//BagOfWords bow = new BagOfWords(word);
			if(nb_sentences==0)
				premise.push_back(new BagOfWords(word));
			else
				hypothesis.push_back(new BagOfWords(word));
				

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

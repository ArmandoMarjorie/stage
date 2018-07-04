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
	cout << "COUCOU\n";
	words.clear();
	words.push_back(mot_inconnu_ID);
	important_bag = imp;
}

void BagOfWords::print_a_sample()
{
	for(unsigned i=0; i<words.size(); ++i)
		cout << words[i] << " ";
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


unsigned Data::get_nb_imp_words(bool is_premise)
{
	unsigned cpt=0;
	if(is_premise)
	{
		for(unsigned i=0; i<premise.size(); ++i)
			if(premise[i].expr_is_important())
				++cpt;
	}
	else
	{
		for(unsigned i=0; i<hypothesis.size(); ++i)
			if(hypothesis[i].expr_is_important())
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
			if( premise[position].expr_is_important() )
			{
				cout << "(POS " << position << " est imp) ";  
				++nb_imp;
			}
			++position;
		}
		cout << "[PREM] ";
	}
	else
	{
		while(nb_imp != num_buffer_in)
		{
			if( hypothesis[position].expr_is_important() )
				++nb_imp;
			++position;
		}
		cout << "[HYP] ";
	}
	
	cout << "(POS) " << static_cast<int>(position-1) << " "; 
	return position-1;
	
}

void Data::modif_LIME(bool is_premise, unsigned num_buffer_in, unsigned position)
{
	
	if(is_premise)
	{
		cout << "MODIF DE " << premise[position].get_word_id(0) << " EN 0\n";
		premise[position].modif_BoW(0, premise[position].expr_is_important());
	}
	else
		hypothesis[position].modif_BoW(0, hypothesis[position].expr_is_important());
}

void Data::reset_sentences(map<vector<unsigned>, unsigned>& save_expr, bool is_premise)
{
	for(std::map<std::vector<unsigned>, unsigned >::iterator it = save_expr.begin(); it != save_expr.end(); ++it)
	{
		BagOfWords bow(it->first, true);
		if(is_premise)
			premise[it->second] = bow;
		else
			hypothesis[it->second] = bow;
	}
}

bool Data::expr_is_important(bool is_premise, unsigned num_expr)
{
	if(is_premise)
		return premise[num_expr].expr_is_important();
	return hypothesis[num_expr].expr_is_important();
}

void Data::print_a_sample()
{
	char c_o, c_f;
	cout << "\tPREMISE : \n\t";
	for(unsigned i=0; i<premise.size(); ++i)
	{
		if(premise[i].expr_is_important())
		{
			c_o = '['; c_f = ']';
		}
		else
		{
			c_o = '{'; c_f = '}';
		}
		cout << c_o;
		premise[i].print_a_sample();
		cout << c_f;
	}
		
	cout << "\n\tHYPOTHESIS : \n\t";
	for(unsigned i=0; i<hypothesis.size(); ++i)
	{
		if(hypothesis[i].expr_is_important())
		{
			c_o = '['; c_f = ']';
		}
		else
		{
			c_o = '{'; c_f = '}';
		}
		cout << c_o;
		hypothesis[i].print_a_sample();
		cout << c_f;
	}
	cout << endl;
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

bool DataSet::expr_is_important(unsigned num_sample, bool is_premise, unsigned num_expr)
{
	return dataset[num_sample].expr_is_important(is_premise, num_expr);
}


unsigned DataSet::get_word_id(unsigned sentence, unsigned num_sample, unsigned num_expr, unsigned num_words)
{
	return dataset[num_sample].get_word_id(sentence, num_expr, num_words);
}

//nb de mot dans l'expression num_expr
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


void DataSet::reset_sentences(unsigned num_sample, map<vector<unsigned>, unsigned>& save_expr, bool is_premise)
{
	dataset[num_sample].reset_sentences(save_expr, is_premise);
}



void DataSet::save_bow(map<vector<unsigned>, unsigned>& save_expr, unsigned num_sentence, unsigned num_sample, unsigned pos)
{
	unsigned nb_word_in_expr = get_nb_words(num_sentence, num_sample, pos);
	vector<unsigned> tmp;
	for(unsigned j=0; j < nb_word_in_expr; ++j)
		tmp.push_back(get_word_id(num_sentence, num_sample, pos, j));
		
	
	save_expr[tmp] = pos;
}


void DataSet::print_a_sample(unsigned num_sample)
{
	dataset[num_sample].print_a_sample();
}

void DataSet::print_everything()
{
	for(unsigned i=0; i<dataset.size(); ++i)
	{
		cout << "SAMPLE " << i << endl;
		dataset[i].print_a_sample();
		cout << endl << endl;
	}
}

void DataSet::modif_LIME(char* buffer_in, unsigned num_sample, map<vector<unsigned>, unsigned>& save_expr_premise, map<vector<unsigned>, unsigned>& save_expr_hyp)
{
	unsigned cpt_crochet=0;
	unsigned nbr_expr=0;
	unsigned nb_word;
	unsigned pos;
	string word;
	
	
	unsigned nb_imp_words_prem = dataset[num_sample].get_nb_imp_words(true);
	//cout << "IL Y A "<< nb_imp_words_prem << " MOTS IMP DANS LA PREMISE\n";
	//cout << "ON COMMENCE\n";
	
	for(unsigned i=0; i < strlen(buffer_in)-1; ++i)
	{
		//cout << "size buff in = " << strlen(buffer_in) << endl;
		//cout << buffer_in[i] << " ";
		
		if(buffer_in[i] == '[' || buffer_in[i] == ' ')
		{
			//cout << "(SAUT) ";
			continue;
		}
		
		stringstream ss;
		while(i < strlen(buffer_in)-1 && buffer_in[i] != ']')
		{
			ss << buffer_in[i];
			++i;
		}		
		++cpt_crochet;
		++nbr_expr;
		word = ss.str();
		
		//cout << "WORD : " << word << "\n";
		if(word == "UNKWORDZ")
		{		
			if(nbr_expr <= nb_imp_words_prem)
			{
				pos = dataset[num_sample].search_position(true, cpt_crochet);
				
				//save
				//save_bow(save_expr_premise, 1, num_sample, pos);
					
				//modif
				dataset[num_sample].modif_LIME(true, cpt_crochet, pos); //a modifier
				if(nbr_expr == nb_imp_words_prem)
					cpt_crochet = 0;
			}
			else
			{
				pos = dataset[num_sample].search_position(false, cpt_crochet);
				
				//save
				//save_bow(save_expr_hyp, 2, num_sample, pos);
				
				//modif
				dataset[num_sample].modif_LIME(false, cpt_crochet, pos); 
			}			
		
		}
	}
}










#include <algorithm>
#include "instance_expression.hpp"

using namespace std;


/**
 * \file instance_expression.cpp
*/


	/* Constructors, Destructor */


/**
	* \brief InstanceExpression Destructor. Release the allocated memory.
*/
InstanceExpression::~InstanceExpression()
{
	words.clear();
	for(unsigned i=0; i < switch_words.size(); ++i)
		switch_words[i]->~SwitchWords();
}

// original lime
InstanceExpression::InstanceExpression(string& word, bool original_lime)
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
	}
	
}



/**
	* \brief InstanceExpression Constructor. Initializes an InstanceExpression 
	* with an instance's expression and its alternative expressions.
	* 
	* \param line : string line with an expression from an instance and its 
	* alternative expressions.
*/
InstanceExpression::InstanceExpression(string& line)
{
	if(line[0] == '{')
		important_bag = false;
	else if(line[0] == '[')
		important_bag = true;
	else
	{
		cout << "unknown evaluation\n";
		exit(EXIT_FAILURE);
	}
	
	std::string::size_type sz;
	int wordID;	
	string word, word2;
	stringstream fluxstring(line);
	
	// Expressions in the premise and the hypothesis
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
	}	
	
	// Alternative expressions
	if(important_bag)
	{
		unsigned nb_expr;
		fluxstring >> nb_expr;
		for(int nb=0 ; nb < nb_expr; ++nb)
			switch_words.push_back(new SwitchWords(fluxstring));
	}
}


/**
	* \brief InstanceExpression Constructor. 
	* Not used for interpretation.
	* 
	* \param word : word ID.
*/
InstanceExpression::InstanceExpression(unsigned word)
{
	words.push_back(word);
}


/* Pas sûre qu'elle soit utilisée */
InstanceExpression::InstanceExpression(const vector<unsigned>& wordsID, bool imp)
{
	for(unsigned i=0; i<wordsID.size(); ++i)
		words.push_back(wordsID[i]);
	important_bag = imp;
}


/**
	* \brief InstanceExpression copy Constructor.
	*
	* \param copy : the InstanceExpression we want to copy.
*/
InstanceExpression::InstanceExpression(InstanceExpression const& copy)
{
	for(unsigned i=0; i < copy.words.size(); ++i)
		this->words.push_back( copy.words[i] );
		
	for(unsigned i=0; i<copy.switch_words.size(); ++i)
		this->switch_words.push_back( new SwitchWords( *(copy.switch_words[i]) ) );
	
	this->important_bag = copy.important_bag;
}


	/* Getters */


/**
	* \name get_nb_words
	* \brief Gives the number of words in the expression.
	* 
	* \return The number of words in the expression.
*/
unsigned InstanceExpression::get_nb_words()
{
	return words.size();
}


/**
	* \name get_word_id
	* \brief Gives the "num_words"th word's ID in the expression.
	* 
	* \param num_words : position of the word in the expression.
	* 
	* \return The "num_words"th word's ID in the expression.
*/
unsigned InstanceExpression::get_word_id(unsigned num_words)
{
	return words[num_words];
	
}


/**
	* \name expr_is_important
	* \brief Tells if the expression's importance is evaluated.
	* 
	* \return True if the "num_expr"th expression's importance is evaluated, else false.
*/
bool InstanceExpression::expr_is_important()
{
	return important_bag;
	
}


/**
	* \name get_nb_switch_words
	* \brief Gives the number of alternative expressions for this expression.
	* 
	* \return The number of alternative expressions for this expression.
*/
unsigned InstanceExpression::get_nb_switch_words()
{
	return switch_words.size();
}


/**
	* \name get_type_sw
	* \brief Tells if this expression's importance is evaluated.
	* 
	* \return True if this expression's importance is evaluated, else false.
*/
unsigned InstanceExpression::get_type_sw(unsigned num_switch_word, unsigned num_sw)
{
	return switch_words[num_switch_word]->get_type_sw(num_sw);
}


/**
	* \name get_nb_of_sw
	* \brief Give the number of alternative expression pieces, in the alternative expression "num_switch_word". For example: 
	* Alternative expressions : 
	* "a", PREV - "big dog", ACTUAL
	* "woman", ACTUAL
	* "man", ACTUAL
	* Then for num_switch_word = 0, it returns 2.
	* 
	* \return The number of alternative expression pieces.
*/
unsigned InstanceExpression::get_nb_of_sw(unsigned num_switch_word)
{
	return switch_words[num_switch_word]->get_nb_of_sw();
}


	/* Setters (expressions/words modifications) */ 

void InstanceExpression::modif_InstanceExpression(unsigned num_switch_words, unsigned num_sw, bool imp)
{
	cout << "\tmodif instance_expression...\n";
	words.clear();
	unsigned nb_expr_in_sw = switch_words[num_switch_words]->get_nb_expr_sw(num_sw);
	cout << "nb de mot dans la sw = " << nb_expr_in_sw << endl;
	
	for(unsigned i=0; i < nb_expr_in_sw; ++i)
		words.push_back(switch_words[num_switch_words]->get_word_id_sw(num_sw, i));
		
	important_bag = imp;
}
void InstanceExpression::modif_InstanceExpression_random(bool imp)
{
	cout << "\tmodif InstanceExpression...\n";
	words.clear();
	
	unsigned alea = (rand() % (36988)) + 1;
	words.push_back(alea);
		
	important_bag = imp;
}
void InstanceExpression::modif_InstanceExpression(InstanceExpression const& current_InstanceExpression, unsigned num_switch_words, unsigned num_sw, bool imp)
{
	cout << "\tmodif InstanceExpression...\n";
	words.clear();
	unsigned nb_expr_in_sw = current_InstanceExpression.switch_words[num_switch_words]->get_nb_expr_sw(num_sw);
	cout << "nb de mot dans la sw = " << nb_expr_in_sw << endl;
	
	for(unsigned i=0; i < nb_expr_in_sw; ++i)
		words.push_back(current_InstanceExpression.switch_words[num_switch_words]->get_word_id_sw(num_sw, i));
		
	important_bag = imp;
}

void InstanceExpression::modif_InstanceExpression(InstanceExpression& instance_expression) //appelé quand on reset l'instance
{
	this->words.clear();
	for(unsigned i=0; i < instance_expression.words.size(); ++i)
		this->words.push_back(instance_expression.words[i]);
	this->important_bag = instance_expression.important_bag;
}

// for the original LIME
void InstanceExpression::modif_InstanceExpression(unsigned UNK, bool imp)
{
	words.clear();
	words.push_back(UNK);
	important_bag = imp;
}


	/* Printing functions */ 


/**
	* \name print_a_sample
	* \brief Print all words in this expression.
*/
void InstanceExpression::print_a_sample()
{
	for(unsigned i=0; i<words.size(); ++i)
		cout << words[i] << " ";
}


/**
	* \name print
	* \brief Print all alternative expressions for this expression.
*/
void InstanceExpression::print()
{
	for(unsigned i=0; i<switch_words.size(); ++i)
		switch_words[i]->print();
}

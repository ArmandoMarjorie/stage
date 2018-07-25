#include "switch_words.hpp"
#include <algorithm>

using namespace std;


/**
 * \file switch_words.cpp
*/
							/* SW Methods */
							
							
	/* Constructors */


/**
	* \brief SW Constructor. Called in the SwitchWords Constructor.
	*
	* \param fluxstring : a string line containing all the alternative expression for a particular expression.
	* Ex : if the expression is "animal" in the sentence "an animal sleeps", then the line could be :
	* #a# PREV #big dog# ACTUAL -1 #a# PREV #cat# ACTUAL -1 #a# PREV #man# ACTUAL -1 
	* (all the words are tokenized with ID, this is just an example to understand).
	* \param word : a string read by the SwitchWords Constructor to check 
	* if it has to stop (if that's an expression the SwitchWords Constructor continues, 
	* if that's "-1" the SwitchWords Constructor stops). 
*/
SW::SW(stringstream& fluxstring, string& word)
{
	std::string::size_type sz;
	int wordID;	
	string word2;
	
	//Extract an expression
	while(word[word.size()-1] != '#')
	{
		fluxstring >> word2;
		word = word + " ";
		word = word + word2;
	}
	
	//Extract each word in the expression
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
		
		//Push the extracted words
		sw.push_back(static_cast<unsigned>(wordID));
	}	
	//Initialize where to place the expression in the sentence (PREV, ACTUAL or NEXT). 
	int t;
	fluxstring >> t;
	word2 = fluxstring.str();
	type = init_type(t, word2);
}


/**
	* \brief SW copy Constructor.
	*
	* \param copy : the SW we want to copy. 
*/
SW::SW(SW const& copy)
{
	this->type = copy.type;
	for(unsigned i=0; i<copy.sw.size(); ++i)
		this->sw.push_back( copy.sw[i] );		
	
}


	/* Getters */


/**
	* \name get_type_sw
	* \brief Give the type of the alternative expression to know where to place it.
	* 
	* \return The type of the alternative expression to know where to place it (PREV, ACTUAL or NEXT).
*/
unsigned SW::get_type_sw()
{
	return type;
}


/**
	* \name get_nb_expr_sw
	* \brief Give the number of words in the alternative expression. For example: "in front of" --> 3.
	* 
	* \return The number of words in the alternative expression.
*/
unsigned SW::get_nb_expr_sw()
{
	return sw.size();
}


/**
	* \name get_word_id_sw
	* \brief Give the id of the "num_word_in_sw"th word in the alternative expression. For example: "in front of" with num_word_in_sw=0 --> "in" 's ID.
	* 
	* \return The id of the "num_word_in_sw"th word in the alternative expression.
*/
unsigned SW::get_word_id_sw(unsigned num_word_in_sw)
{
	return sw[num_word_in_sw];
}


	/* Others */


inline unsigned SW::init_type(int type, string& line)
{
	switch(type)
	{
		case -6:
			return PREV;
		case -4:
			return ACTUAL;
		case -5:
			return NEXT;
		default:
			cerr << "ACTUAL, PREV or NEXT undefined \nline = " << line;
			exit(EXIT_FAILURE);		
	}
}
	

/**
	* \name print
	* \brief Just print the "piece" of the alternative expression.
*/
void SW::print()
{
	for(unsigned i=0; i<sw.size(); ++i)
		cout << sw[i] << " ";
	if(type == ACTUAL)
		cout << "ACTUAL \n";
	else if(type == PREV)
		cout << "PREV \n";
	else if(type == NEXT)
		cout << "NEXT \n";
	else
		cout << "ERROR\n";
}


							/* SwitchWords Methods */


	/* Constructors, Destructor */
	
	
/**
	* \brief SwitchWords Constructor.
	*
	* \param fluxstring : a string line containing all the alternative expression for a particular expression.
	* Ex : if the expression is "animal" in the sentence "an animal sleeps", then the line could be :
	* #a# PREV #big dog# ACTUAL -1 #a# PREV #cat# ACTUAL -1 #a# PREV #man# ACTUAL -1 
	* (all the words are tokenized with ID, this is just an example to understand).
*/
SwitchWords::SwitchWords(stringstream& fluxstring)
{
	string word;
	fluxstring >> word;
	
	// -1 means it's the end of the alternative expression
	while(word != "-1")
	{
		switch_w.push_back(new SW(fluxstring, word));
		fluxstring >> word;
	}
}


/**
	* \brief SwitchWords copy Constructor.
	*
	* \param copy : the SwitchWords we want to copy.
*/
SwitchWords::SwitchWords(SwitchWords const& copy)
{
	for(unsigned i=0; i<copy.switch_w.size(); ++i)
		this->switch_w.push_back( new SW( *(copy.switch_w[i]) ) );	
}


/**
	* \brief SwitchWords Destructor. Release the allocated memory.
*/
SwitchWords::~SwitchWords()
{
	for(unsigned i=0; i < switch_w.size(); ++i)
		delete switch_w[i];
}


	/* Getters */


/**
	* \name get_word_id_sw
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
unsigned SwitchWords::get_word_id_sw(unsigned num_sw, unsigned num_word_in_sw)
{
	return switch_w[num_sw]->get_word_id_sw(num_word_in_sw);
}


/**
	* \name get_nb_expr_sw
	* \brief Give the number of words in the "num_sw"th alternative expression's piece. For example: 
	* switch_w[0] = "a", PREV
	* switch_w[1] = "big dog", ACTUAL
	* Then the number of words in switch_w[1] is 2 (big and dog).
	* 
	* \param num_sw : numero of the "piece" of the alternative expression. 
	* 
	* \return The number of words in the alternative expression.
*/
unsigned SwitchWords::get_nb_expr_sw(unsigned num_sw)
{
	return switch_w[num_sw]->get_nb_expr_sw();
}


/**
	* \name get_type_sw
	* \brief Tell where to place the "num_sw"th alternative expression's piece. For example: 
	* switch_w[0] = "a", PREV
	* switch_w[1] = "big dog", ACTUAL
	* And num_sw = 0, then it returns PREV.
	* 
	* \param num_sw : numero of the "piece" of the alternative expression. 
	* 
	* \return Where to place the "num_sw"th alternative expression's piece (PREV, ACTUAL or NEXT).
*/
unsigned SwitchWords::get_type_sw(unsigned num_sw)
{
	return switch_w[num_sw]->get_type_sw();
}


/**
	* \name get_nb_of_sw
	* \brief Give the number of alternative expression pieces. For example: 
	* switch_w[0] = "a", PREV
	* switch_w[1] = "big dog", ACTUAL
	* Then it returns 2.
	* 
	* \return The number of alternative expression pieces.
*/
unsigned SwitchWords::get_nb_of_sw()
{
	return switch_w.size();
}


	/* Other */


/**
	* \name print
	* \brief Just print the alternative expression.
*/
void SwitchWords::print()
{
	for(unsigned i=0; i<switch_w.size(); ++i)
		switch_w[i]->print();
}

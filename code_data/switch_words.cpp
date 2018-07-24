#include "switch_words.hpp"
#include <algorithm>

using namespace std;

/**
	* \brief SW Constructor.
	*
	* \param fluxstring : 
	* \param word : 
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





void SwitchWords::print()
{
	for(unsigned i=0; i<switch_w.size(); ++i)
	{
		cout << "SW " << i << endl;
		switch_w[i]->print();
	}
}

unsigned SwitchWords::get_word_id_sw(unsigned num_sw, unsigned num_word_in_sw)
{
	cout << "ok SwitchWords::get_word_id_sw = \n";
	return switch_w[num_sw]->get_word_id_sw(num_word_in_sw);
}


unsigned SwitchWords::get_nb_expr_sw(unsigned num_sw)
{
	cout << "SwitchWords::get_nb_expr_sw = " << switch_w[num_sw]->get_nb_expr_sw() << endl;
	return switch_w[num_sw]->get_nb_expr_sw();
}

unsigned SwitchWords::get_type_sw(unsigned num_sw)
{
	return switch_w[num_sw]->get_type_sw();
}
unsigned SwitchWords::get_nb_of_sw()
{
	return switch_w.size();
}

SwitchWords::SwitchWords(stringstream& fluxstring)
{
	string word;
	fluxstring >> word;
	while(word != "-1")
	{
		switch_w.push_back(new SW(fluxstring, word));
		fluxstring >> word;
	}
}

SwitchWords::~SwitchWords()
{
	for(unsigned i=0; i < switch_w.size(); ++i)
		delete switch_w[i];
}

SW::SW(SW const& copy)
{
	this->type = copy.type;
	for(unsigned i=0; i<copy.sw.size(); ++i)
		this->sw.push_back( copy.sw[i] );		
	
}

SwitchWords::SwitchWords(SwitchWords const& copy)
{
	for(unsigned i=0; i<copy.switch_w.size(); ++i)
		this->switch_w.push_back( new SW( *(copy.switch_w[i]) ) );	
}

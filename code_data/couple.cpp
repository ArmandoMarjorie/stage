#include "couple.hpp"
#include <algorithm>

using namespace std;
	
	
	/* Constructor */
	
/**
	* \brief Couple Constructor 
	* Initialize the couples with a file. 
	* This constructor is called by the Data constructor to read and to inizialise the couples for each sample.
	* 
	* \param test_explication : File containing the samples in this form :
	*       label
	*       premise -1 premise's length
	*       hypothesis -1 hypothesis' length
	* 		premise's words -2 hypothesis' words -1 // list of couples (-2 is a separator, -1 means end of couple)
	* 		-5 // this means end of the couples' list
	* 		ith couple's label // list of the couple's label
	* 		-3 // this means end of sample
*/
/* Ex couple :
	22 15 8 16 507 17 -2 507 1 -1 1
	0 -4 -2 8634 3 -1 0
	0 -4 -2 5524 6 -1 0
	-5
*/
Couple::Couple(ifstream& test_explication)
{
	int val = 0;
	bool ok;
	int position = 0;
	while(val != -5) //read all the couples (-5 = end of the couples list)
	{
		test_explication >> val;
		if(val == -5)
			continue;
		test_explication >> position;
		ok = true;
		vector<pair<unsigned,int>> tmp;
		while(ok && val != -1)
		{
			tmp.push_back(make_pair(static_cast<unsigned>(val) , position));
			test_explication >> val;
			if(val == -2)
			{
				imp_word_premise.push_back(tmp);
				ok = false;
			}
			if(ok && val != -1)
				test_explication >> position;
		}
		if(val == -1)
			imp_word_hypothesis.push_back(tmp);
			
	}	
	test_explication >> val; //read the first label
	while(val != -3)
	{
		labels.push_back(static_cast<unsigned>(val));
		test_explication >> val;
	}
}

	/* Getters and setters */

/**
	* \name get_id
	* \brief Give the id of the word "num_mot" of the couple "num_couple"
	*
	* \param num_couple : The numero of the couple
	* \param num_mot : The position of the word
	* \param premise : True if you want to get the id of the word from the premise, false if it's from the hypothesis
	* 
	* \return The id of the word
*/
unsigned Couple::get_id(unsigned num_couple, unsigned num_mot, bool premise)
{
	if(premise)
		return imp_word_premise[num_couple][num_mot].first; 
	return imp_word_hypothesis[num_couple][num_mot].first;
}

/**
	* \name get_position
	* \brief Give the position of the word "num_mot" of the couple "num_couple"
	*
	* \param num_couple : The numero of the couple
	* \param num_mot : The position of the word in the couple 
	* \param premise : True if you want to get the position of the word from the premise, false if it's from the hypothesis
	* 
	* \return The position of the word in the premise or in the hypothesis
*/
int Couple::get_position(unsigned num_couple, unsigned num_mot, bool premise)
{
	if(premise)
		return imp_word_premise[num_couple][num_mot].second; 
	return imp_word_hypothesis[num_couple][num_mot].second;
}

/**
	* \name get_nb_words
	* \brief Give the number of words in the couple "num_couple"
	*
	* \param num_couple : The numero of the couple
	* \param premise : True if you want to get the number of words in the couple in the premise side, false if it's in the hypothesis side
	* 
	* \return The number of words in the premise side or in the hypothesis side
*/
unsigned Couple::get_nb_words(unsigned num_couple, bool premise)
{
	if(premise)
		return imp_word_premise[num_couple].size(); 
	return imp_word_hypothesis[num_couple].size();
}

/**
	* \name get_size
	* \brief Give the number of couples
	*
	* \param num_couple : The numero of the couple
	* 
	* \return The number of couples
*/
unsigned Couple::get_size()
{
	return imp_word_hypothesis.size();
}

/**
	* \name get_label
	* \brief Give the label of the couple
	*
	* \param num_couple : The numero of the couple
	* 
	* \return The label of the couple
*/
unsigned Couple::get_label(unsigned num_couple)
{
	return labels[num_couple];
}

	/* Printing functions */

/**
	* \name print_couples
	* \brief Print the couple numero "num_couple" in this form :
	* 	Premise :
	* 	word POSITION[position of the word in the original premise] ...
	*	Hypothesis :
	* 	word POSITION[position of the word in the original hypothesis] ...
	*
	* \param num_couple : The numero of the couple
*/
void Couple::print_couples(unsigned num_couple)
{
	cerr << "Premise : \n";
	unsigned i=num_couple;
	for(unsigned j=0; j<imp_word_premise[i].size(); ++j)
		cerr << imp_word_premise[i][j].first << " POSITION[" << imp_word_premise[i][j].second << "] ";
	cerr << endl;
	cerr << "Hypothesis : \n";
	for(unsigned j=0; j<imp_word_hypothesis[i].size(); ++j)
		cerr << imp_word_hypothesis[i][j].first << " POSITION[" << imp_word_hypothesis[i][j].second << "] ";
	cerr << endl;
	cerr<<endl;
}

#include "switch_words.hpp"
#include <algorithm>

using namespace std;

/*
Switch_Words::Switch_Words(char* filename)
{
	ifstream database(filename, ios::in);
	if(!database)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	int word;
	while(database >> word)
	{
		SW sample(database);
		sw.push_back(sample);
	}
	
	database.close();
}

unsigned Switch_Words::get_nb_switch_words(unsigned word_position, bool is_premise, unsigned num_sample)
{
	return sw[num_sample].get_nb_switch_words(word_position, is_premise);
}

unsigned Switch_Words::get_switch_word(unsigned word_position, bool is_premise, unsigned num_switch_word, unsigned num_sample)
{
	return sw[num_sample].get_switch_word(word_position, is_premise, num_switch_word);
}
*/ 

/*
SW::SW(ifstream& database)
{
	int word;
	unsigned cpt=0;
	bool is_premise = true, ok = true;
	while(ok && database >> word) //read a 'real' word (appearing in the sentences test file)
	{
		++cpt;
		if(word == -2)
		{
			cpt=0;
			is_premise = false;
			continue;
		}
		else if(word == -3)
		{
			is_premise = true;
			ok = false;
		}
		else
		{
			vector<unsigned> tmp;
			//tmp.push_back(static_cast<unsigned>(word));
			
			database >> word;
			while( word != -1 )
			{
				tmp.push_back(static_cast<unsigned>(word));
				database >> word;
			}
			if(is_premise)
				prem.push_back(tmp);
			else
				hyp.push_back(tmp);
		}

		
	}
}

unsigned SW::get_nb_switch_words(unsigned word_position, bool is_premise)
{
	if(is_premise)
		return prem[word_position].size();
	else
		return hyp[word_position].size();
}

unsigned SW::get_switch_word(unsigned word_position, bool is_premise, unsigned num_switch_word)
{
	if(is_premise)
		return prem[word_position][num_switch_word];
	else
		return hyp[word_position][num_switch_word];
}
*/

Switch_Words::Switch_Words(char* filename)
{
	ifstream database(filename, ios::in);
	if(!database)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	int word;
	while(database >> word)
	{
		SW premise(database);
		prem.push_back(premise);
		SW hypothesis(database);
		hyp.push_back(hypothesis);
	}
	
	database.close();
}

SW::SW(ifstream& database)
{
	int word, nb_words, i;
	unsigned cpt=0, position;
	bool is_premise = true, ok = true;
	while(ok && database >> nb_words) //read a 'real' word (appearing in the sentences test file)
	{
		if(nb_words == -2)
		{
			ok = false;
			continue;
		}
		
		// Gère les vrais mots (ceux dans la phrase de base)
		for(i=0; i<nb_words; ++i)
		{
			database >> word;
			database >> position;
			real_words.push_back(make_pair(static_cast<unsigned>(word), position));
		}
		
		// Gère les mots/expressions de remplacement
		database >> nb_words; //nb de mots/d'expressions pouvant le remplacer	
		for(i=0; i<nb_words; ++i)
		{
			vector<pair<unsigned,unsigned>> tmp;
			database >> word;
			while(word!=-1)
			{
				database >> position;
				tmp.push_back(make_pair(static_cast<unsigned>(word), position));
				database >> word;
			}
			remplacing_words.push_back(tmp);
		}		
		
	}
}





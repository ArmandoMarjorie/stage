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
	unsigned cpt=0;
	while(database >> word) //read a 'real' word in the test file
	{
		vector<unsigned> tmp;
		tmp.push_back(static_cast<unsigned>(word));
		
		database >> word;
		while( word != -1 )
		{
			tmp.push_back(static_cast<unsigned>(word));
			database >> word;
		}
		
		sw.push_back(tmp);
		correspondance[tmp[0]] = cpt;
		++cpt;
	}
	
	database.close();
}


unsigned Switch_Words::get_corresponding(unsigned word_id)
{
	return correspondance[word_id];
}


/**
	* \name get_nb_switch_words
	* \brief Give the number of words that can replace word_id
	*
	* \param word_id : id of the word
	* 
	* \return The number of words that can replace word_id

unsigned Switch_Words::get_nb_switch_words(unsigned word_id)
{
	return sw[correspondance[word_id]].size();
}

unsigned Switch_Words::get_switch_word(unsigned word_id, unsigned num_switch_word)
{
	return sw[correspondance[word_id]][num_switch_word];
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
			continue;
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
		}
		if(is_premise)
			prem[cpt].push_back(tmp);
		else
			hyp[cpt].push_back(tmp);
		
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








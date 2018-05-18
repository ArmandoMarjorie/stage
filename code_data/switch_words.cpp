#include "switch_words.hpp"
#include <algorithm>

using namespace std;

Switch_Words::Switch_Words(char* filename)
{
	ifstream database(filename, ios::in);
	if(!database)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	unsigned word;
	unsigned cpt=0;
	while(database >> word) //read a 'real' word in the test file
	{
		vector<unsigned> tmp;
		tmp.push_back(word);
		
		database >> word;
		while( word != -1 )
		{
			tmp.push_back(word);
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
*/
unsigned Switch_Words::get_nb_switch_words(unsigned word_id)
{
	return sw[correspondance[word_id]].size();
}

unsigned Switch_Words::get_switch_word(unsigned word_id, unsigned num_switch_word)
{
	return sw[correspondance[word_id]][num_switch_word];
}



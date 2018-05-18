#include "proba_bigram.hpp"
#include <algorithm>

using namespace std;

Proba_Bigram::Proba_Bigram(char* filename, char* uni_filename)
{
	ifstream proba_file(filename, ios::in);
	if(!proba_file)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	ifstream unigram_file(uni_filename, ios::in);
	if(!unigram_file)
	{ 
		cerr << "Impossible to open the file " << uni_filename << endl;
		exit(EXIT_FAILURE);
	}
	unsigned word1, word2;
	double prob;
	proba_file >> v;
	proba_file >> alpha;
	while(proba_file >> word1)
	{
		proba_file >> word2;
		proba_file >> prob;
		proba[make_pair(word1, word2)] = prob;
	}
	
	while(unigram_file >> word1)
	{
		unigram_file >> word2; 
		unigram[word1] = word2;
	}
	
	unigram_file.close();
	proba_file.close();
}


unsigned Proba_Bigram::get_proba_log(unsigned word1, unsigned word2)
{
	map<unsigned, unsigned>::iterator trouve = unigram.find(word1);
	pair<unsigned,unsigned > pair_of_words(word1, word2);
	map< pair<unsigned, unsigned>, double >::iterator trouver = proba.find(pair_of_words);
	
	if(trouver != proba.end()) //si le couple de mot existe
		return proba[pair_of_words];
		
	if(trouver == proba.end() && trouve != unigram.end() ) //si le couple de mot n'existe pas mais que l'occurence de wi-1 existe
		return alpha/(double)(unigram[word1] + v * alpha);
		
	if(trouver == proba.end() && trouve == unigram.end() ) //si rien n'existe (wi-1 : mot inconnu)
		return alpha/(double)(v*alpha);
}





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
	unigram_file >> word1;
	while(unigram_file >> word1)
	{
		unigram_file >> word2; 
		unigram[word1] = word2;
	}
	
	unigram_file.close();
	proba_file.close();
}


double Proba_Bigram::get_proba_log(unsigned word1, unsigned word2)
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
		
	return -1;
}

double Proba_Bigram::get_proba_log(unsigned word)
{
	/*map<unsigned, unsigned>::iterator trouve = unigram.find(word1);
	pair<unsigned,unsigned > pair_of_words(word1, word2);
	map< pair<unsigned, unsigned>, double >::iterator trouver = proba.find(pair_of_words);
	
	if(trouver != proba.end()) //si le couple de mot existe
		return proba[pair_of_words];
		
	if(trouver == proba.end() && trouve != unigram.end() ) //si le couple de mot n'existe pas mais que l'occurence de wi-1 existe
		return alpha/(double)(unigram[word1] + v * alpha);
		
	if(trouver == proba.end() && trouve == unigram.end() ) //si rien n'existe (wi-1 : mot inconnu)
		return alpha/(double)(v*alpha);
		
	return -1;*/
}












/* Debuggage */

void Proba_Bigram::print_prob(char* fi, char* fi2)
{
	//			std::map< std::pair<unsigned, unsigned>, double > proba;
	//		std::map<  unsigned, unsigned > unigram;
	
	char* name1 = "proba_log_test";
	char* name2 = "unigram_test";
	ofstream output_proba_log(name1, ios::out | ios::trunc);
	if(!output_proba_log)
	{
		cerr << "Problem with the output file in print_prob" << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output(name2, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file in print_prob" << endl;
		exit(EXIT_FAILURE);
	}
	
	output_proba_log << v << endl << alpha << endl;
	for(map<pair<unsigned, unsigned>, double >::iterator it=proba.begin(); it!=proba.end(); ++it) //parcours de la 2e map
	{
		output_proba_log << it->first.first << " " << it->first.second << " " << it->second << endl;
	}	
	for(map<unsigned, unsigned>::iterator it=unigram.begin(); it!=unigram.end(); ++it) //parcours de la 2e map
	{
		output << it->first << " " << it->second << " " << endl;
	}	
	output.close();
	output_proba_log.close();
	
	is_equal(fi, name1);
	is_equal(fi2, name2);
}

bool Proba_Bigram::is_equal(char* true_filename, char* generating_filename)
{
	ifstream f(true_filename, ios::in);
	if(!f)
	{ 
		cerr << "Impossible to open the file " << true_filename << endl;
		exit(EXIT_FAILURE);
	}
	ifstream f2(generating_filename, ios::in);
	if(!f2)
	{ 
		cerr << "Impossible to open the file " << generating_filename << endl;
		exit(EXIT_FAILURE);
	}
	
	string line, line2;
	unsigned nb_line=0;
	while( getline(f,line) && getline(f2, line2) )
	{
		++nb_line;
		if(line != line2)
		{
			cout << "file " << true_filename << endl;
			cout << "difference in line " << nb_line << endl;
			cout << line << endl;
			cout << line2 << endl;
			return false;
		}
		
	}
	cout << "ok\n";
	return true;

}

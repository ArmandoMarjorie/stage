#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <utility>
#include <algorithm>
#include <locale>
#include <cmath>
#define ALPHA 0.9


/** LEXIQUE = VOCABULAIRE + LEUR ID */

using namespace std;

void make_file(map< int,int >& occurrences_uni, map< pair<int, int>, int >& occurrences_bi, char* output_name_uni, char* output_name_bi, char* output_name_proba_log,
	char* o, unsigned n)
{
	//unigram file
	ofstream output_uni(output_name_uni, ios::out | ios::trunc);
	if(!output_uni)
	{
		cerr << "Problem with the output file " << output_name_uni << endl;
		exit(EXIT_FAILURE);
	}
	//bigram file
	ofstream output_bi(output_name_bi, ios::out | ios::trunc);
	if(!output_bi)
	{
		cerr << "Problem with the output file " << output_name_bi << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output_proba_log(output_name_proba_log, ios::out | ios::trunc);
	if(!output_proba_log)
	{
		cerr << "Problem with the output file " << output_name_proba_log << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output_proba_log_uni(o, ios::out | ios::trunc);
	if(!output_proba_log_uni)
	{
		cerr << "Problem with the output file " << o << endl;
		exit(EXIT_FAILURE);
	}
	
	string ligne;
	unsigned v1=0, v2=0;
	
	//unigram file
	for(map<int,int>::iterator it1=occurrences_uni.begin(); it1!=occurrences_uni.end(); ++it1) //parcours de la 1ere map
	{
		++v1;
		output_uni << it1->first << " " << it1->second << endl; //écriture dans le fichier de compte
	}
		
	//bigram file
	for(map<pair<int, int>, int >::iterator it=occurrences_bi.begin(); it!=occurrences_bi.end(); ++it) //parcours de la 2e map
	{
		++v2;
		output_bi << it->first.first << " " << it->first.second << " " << it->second << endl; //écriture dans le fichier de compte
	}
	output_proba_log << v2 << endl << ALPHA << endl;
	for(map<pair<int, int>, int >::iterator it=occurrences_bi.begin(); it!=occurrences_bi.end(); ++it) //parcours de la 2e map
	{
		output_proba_log << it->first.first << " " << it->first.second << " " << log10( (it->second + ALPHA)/(double)(occurrences_uni[it->first.first] + v2 * ALPHA) ) << endl;
	}
	output_uni << v1 << endl;
	output_bi << v2 << endl;
	output_proba_log_uni << n << endl << v1 << endl;
	for(map<int,int>::iterator it1=occurrences_uni.begin(); it1!=occurrences_uni.end(); ++it1) //parcours de la 1ere map
		output_proba_log_uni << it1->first << " " << log10((it1->second+ALPHA)/(double)(n+v1*ALPHA)) << endl;
	
	
	
	output_bi.close();
	output_uni.close();
	output_proba_log.close();
	output_proba_log_uni.close();
	
}

void reading_train(char* filename, char* output_name_uni, char* output_name_bi, char* output_name_proba_log,
	char* o)
{
	//train file tokenized
	ifstream train(filename, ios::in);
	if(!train)
	{ 
		cerr << "Impossible to open the file " << filename << endl;
		exit(EXIT_FAILURE);
	}	
	
	map< int,int > occurrences_uni;
	map< pair<int, int>, int > occurrences_bi;
	unsigned n=0;

	int word1, word2, i;
	while( train >> word1 ) //read the label
	{
		for(i=0; i<2; ++i)
		{
			word1=0;
			++occurrences_uni[word1];
			train >> word2;
			++n;
			++occurrences_uni[word2]; //augmente l'occurence du code que l'on vient de lire
			++occurrences_bi[make_pair(word1, word2)];
			word1 = word2;
			train >> word2;
			++n;
			while(word2 != -1)
			{
				++occurrences_uni[word2]; //augmente l'occurence du code que l'on vient de lire
				++occurrences_bi[make_pair(word1, word2)];	
				word1 = word2;		
				train >> word2;
				++n;
			}
			--n;
			train >> word1; //read the length
		}
	}
	
	make_file(occurrences_uni, occurrences_bi, output_name_uni, output_name_bi, output_name_proba_log, o, n);

}


int main(int argc, char** argv)
{
	if(argc != 6)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "tokenized train\n"
			 << "output_name unigram\n"
			 << "output_name bigram\n"
			 << "output_name bigram proba log\n"
			 << "output_name unigram proba log \n";
		exit(EXIT_FAILURE);
	}
	reading_train(argv[1], argv[2], argv[3], argv[4], argv[5]);
	
	
	return 0;
}



#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <locale>

using namespace std;


void extract_words_ref(ifstream& expl_ref, unsigned& nb_expl_ref, vector<string>& mots_expl_ref)
{
	cout << "EXPL REF\n";
	string word,w;
	for(unsigned tmp=0; tmp<4; ++tmp)
	{
		getline(expl_ref,word);//lit le label, la prem, l'hyp, le nb d'expl (1 seule)
		cout << word << endl;
	}
	for(unsigned nb_phrase=0; nb_phrase<2; ++nb_phrase)
	{
		getline(expl_ref,word);
		cout << word << endl;
		stringstream fluxstring(word);
		while(fluxstring >> w)
		{
			if(w == "-1")
				continue;
			mots_expl_ref.push_back(w);
			fluxstring >> w; //lit la position
			++nb_expl_ref;
		}
	}
	getline(expl_ref,word);
	getline(expl_ref,word);
}


void extract_words_lime(ifstream& expl, unsigned nb_expl_ref, vector<string>& mots_expl)
{
	cout << "EXPL LIME\n";
	string word,w;
	char tmp[100];
	unsigned nb=0,j,i;
	for(i=0; i<3; ++i)
	{
		getline(expl, word);
		cout << word << endl;
	}
	for(unsigned nb=0; nb<nb_expl_ref; ++nb)
	{
		getline(expl, word);
		cout << word << endl;
		stringstream f(word);
		f >> w;
		for(i=2,j=0; i < w.size()-2; ++i,++j)
			tmp[j] = w[i];
		tmp[j] = '\0';
		mots_expl.push_back(std::string(tmp));
	}
	do
	{
		getline(expl, word);
	}while(word != "-3");
		
}

void calcul_score(vector<string>& mots_expl_ref, vector<string>& mots_expl, unsigned& nb_corrects)
{
	for(unsigned i=0; i<mots_expl.size(); ++i)
		if ( std::find(mots_expl_ref.begin(), mots_expl_ref.end(), mots_expl[i]) != mots_expl_ref.end() ) //mot correct
			++nb_corrects;
}

void acc(char* expl_ref_filename, char* expl_filename )
{
	ifstream expl_ref(expl_ref_filename, ios::in);
	if(!expl_ref)
	{ 
		cerr << "Impossible to open the file " << expl_ref_filename << endl;
		exit(EXIT_FAILURE);
	}		
	ifstream expl(expl_filename, ios::in);
	if(!expl)
	{ 
		cerr << "Impossible to open the file " << expl_filename << endl;
		exit(EXIT_FAILURE);
	}		


	string word;
	unsigned nb_words_expl=0;
	unsigned nb_corrects=0;
	
	while(getline(expl_ref,word)) //lit l'id
	{	
		cout << "SAMPLE" << word << endl;
		if(word == "#END#")
			break;
		
		vector<string> mots_expl_ref;
		unsigned nb_expl_ref=0;
		extract_words_ref(expl_ref, nb_expl_ref, mots_expl_ref);
		nb_words_expl += nb_expl_ref;

		vector<string> mots_expl;
		extract_words_lime(expl, nb_expl_ref, mots_expl);
		
		calcul_score(mots_expl_ref, mots_expl, nb_corrects);
	}
	 
	cout << "ACCURACY = " << (double)nb_corrects/(double)nb_words_expl << "(" << nb_corrects << "/" << nb_words_expl << ")" << endl;
	
	expl.close();
	expl_ref.close();
}





int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique\n"
			 << "file mots remplacant\n";
		exit(EXIT_FAILURE);
	}
	
	//fichier_pour_lime(argv[2], argv[3]);
	acc(argv[1], argv[2]);
	//verif_nb_expressions(argv[2]);
	return 0;
}

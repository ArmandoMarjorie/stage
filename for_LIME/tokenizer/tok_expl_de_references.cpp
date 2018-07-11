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


void placement_lime(ifstream& expl, string& label)
{
	string w, word;
	getline(expl,w); //lit 'sample numero x'
	while(w != label)
	{
		while(w[0] != 'E')
			getline(expl,word);
		stringstream f(word);
		if(w != "neutral" && w != "entailment" && w != "contradiction")
			f >> w;
	}
	
}

void acc(char* expl_filename, char* expl_ref_filename)
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
	std::string::size_type sz;
	
	
	while(getline(expl_ref,word)) //lit l'id
	{	
		//cout << "SAMPLE " << num << endl;
		getline(expl_ref,word); //lit le label
		
		placement_lime(expl, word); //les prochains getline seront les mots importants
		
		for(unsigned tmp=0; tmp<3; ++tmp)
			getline(expl_ref,word);//lit la prem, lit l'hyp, lit le nb d'expl (1 seule)
		
		
		vector<string> mots_expl_ref;
		unsigned nb_expl_ref=0;
		for(unsigned nb_phrase=0; nb_phrase<2; ++nb_phrase)
		{
			getline(expl_ref,word);
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
		vector<string> mots_expl;
		extract_words(expl, nb_expl_ref, mots_expl);
		
		getline(expl_ref,word);
		getline(expl_ref,word);		
	}
	
}

void extract_words(ifstream& expl, unsigned nb_expl_ref, vector<string>& mots_expl)
{
	string word="tmp", w, tmp="";
	unsigned nb=0;
	getline(expl, word);
	while(nb<nb_expl_ref || (word != "Explanation" && word != "\n") )
	{
		stringstream f(word);
		f >> w;
		for(unsigned i=2,j=0; i < w.size()-2; ++i,++j)
			tmp[j] = w[i];
		tmp[j] = '\0';
		mots_expl.push_back(tmp);
		getline(expl, word);
		++nb;
	}
}



int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique\n"
			 << "file mots remplacant\n"
			 << "output (mots remplacant tokenisé)\n";
		exit(EXIT_FAILURE);
	}
	
	//fichier_pour_lime(argv[2], argv[3]);
	tokeniser_nouveau_remplacement(argv[1], argv[2], argv[3]);
	//verif_nb_expressions(argv[2]);
	return 0;
}

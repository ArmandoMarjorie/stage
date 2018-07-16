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


void extract_words_ref(ifstream& expl_ref, unsigned& nb_expl_ref, vector<string>& mots_expl_ref, int& label)
{
	cout << "EXPL REF\n";
	string word,w, lab;
	getline(expl_ref, lab);
	stringstream ss(lab);
	ss >> word;
	cout << word;
	if(word == "neutral")
		label = 0;
	else if(word == "entailment")	
		label=1;
	else	
		label=2;
	
	for(unsigned tmp=0; tmp<3; ++tmp)
	{
		getline(expl_ref,word);//la prem, l'hyp, le nb d'expl (1 seule)
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
		for(i=2,j=0; i < w.size()-3; ++i,++j)
			tmp[j] = w[i];
		tmp[j] = '\0';
		mots_expl.push_back(std::string(tmp));
	}
	do
	{
		getline(expl, word);
	}while(word != "-3");
	getline(expl, word); getline(expl, word);
}

unsigned calcul_score(vector<string>& mots_expl_ref, vector<string>& mots_expl, unsigned& nb_corrects)
{
	unsigned c=0;
	for(unsigned i=0; i<mots_expl.size(); ++i)
	{
		if ( std::find(mots_expl_ref.begin(), mots_expl_ref.end(), mots_expl[i]) != mots_expl_ref.end() ) //mot correct
		{
			++nb_corrects;
			++c;
		}
	}
	return c;
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
	unsigned nb_corrects=0, c;
	int label;
	vector<unsigned> correct_per_labels(3);
	vector<unsigned> nb_labels(3);
	for(unsigned i=0; i<3; ++i)
	{
		nb_labels[i] = correct_per_labels[i] = 0;
	}
	
	while(getline(expl_ref,word)) //lit l'id
	{	
		cout << "SAMPLE" << word << endl;
		if(word == " ")
			break;
		
		vector<string> mots_expl_ref;
		unsigned nb_expl_ref=0;
		extract_words_ref(expl_ref, nb_expl_ref, mots_expl_ref, label);
		
		nb_words_expl += nb_expl_ref;
		nb_labels[label] += nb_expl_ref;

		vector<string> mots_expl;
		extract_words_lime(expl, nb_expl_ref, mots_expl);
		
		cout << "EXPL\n";
		for(unsigned i=0; i<mots_expl.size(); ++i)
			cout << mots_expl[i] << " ";
		cout << endl;
		
		c = calcul_score(mots_expl_ref, mots_expl, nb_corrects);
		correct_per_labels[label] += c;
	}
	 
	cout << "ACCURACY = " << (double)nb_corrects/(double)nb_words_expl << 
		"(" << nb_corrects << "/" << nb_words_expl << ")" << endl;
		
	for(unsigned i=0; i<3; ++i)
		cout << "ACC " << i << " = " << correct_per_labels[i] / (double) nb_labels[i] << endl <<
			"(" << correct_per_labels[i] << "/" << nb_labels[i] << ")" << endl;
	
	expl.close();
	expl_ref.close();
}





int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "expl_ref_filename\n"
			 << "expl_filename sortie\n";
		exit(EXIT_FAILURE);
	}
	
	//fichier_pour_lime(argv[2], argv[3]);
	acc(argv[1], argv[2]);
	//verif_nb_expressions(argv[2]);
	return 0;
}

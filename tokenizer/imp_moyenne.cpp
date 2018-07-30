#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <locale>
#include <set>
#include <functional>

using namespace std;



string extract_words_lime(ifstream& expl, float& poids)
{
	cout << "EXPL LIME\n";
	string word,w;
	char tmp[100];
	unsigned nb=0,j,i;
	std::string::size_type sz; 

	getline(expl, word);
	if(word =="-3")
	{
		getline(expl, w);
		getline(expl, w);
		return word;
	}
	cout << word << endl;
	stringstream f(word);
	f >> w;
	for(i=2,j=0; i < w.size()-3; ++i,++j)
		tmp[j] = w[i];
	tmp[j] = '\0';
	word = std::string(tmp);
	
	f >> w;
	poids = std::stof(w,&sz);
	
	poids = abs(poids);
	
	return word;

		
}

string to_string_lab(unsigned label)
{
	string s;
	if(label==0)
		return "neutral";
	if(label==1)
		return "entailment";
	return "contradiction";
}



void calcul_imp_avg(map<string, float>& words_importance_neutral, map<string, float>& words_importance_inf, 
	map<string, float>& words_importance_contradiction, map<string, unsigned>& nb_neutral,
	map<string, unsigned>& nb_inf, map<string, unsigned>& nb_contradiction, char* output_f)
{
	
	typedef std::function<bool(std::pair<std::string, float>, std::pair<std::string, float>)> Comparator;
	Comparator compFunctor =
		[](std::pair<std::string, float> elem1 ,std::pair<std::string, float> elem2)
		{
			return elem1.second > elem2.second;
		};
 

	
	map<string, float> words_importance_neutral_avg;
	map<string, float> words_importance_inf_avg;
	map<string, float> words_importance_contradiction_avg;
	ofstream output(output_f, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_f << endl;
		exit(EXIT_FAILURE);
	}
	for(unsigned i=0; i<3; ++i)
	{
		if(i==0)
		{
			for(map<string,float>::iterator it1=words_importance_neutral.begin(); it1!=words_importance_neutral.end(); ++it1)
			{
				if(nb_neutral[it1->first]!=0)
					words_importance_neutral_avg[it1->first] = it1->second / (double)nb_neutral[it1->first];
			}
		}
		else if(i==1)
		{
			for(map<string,float>::iterator it1=words_importance_inf.begin(); it1!=words_importance_inf.end(); ++it1)
			{
				if(nb_inf[it1->first]!=0)
					words_importance_inf_avg[it1->first] = it1->second / (double)nb_inf[it1->first];
			}			
		}		
		else
		{
			for(map<string,float>::iterator it1=words_importance_contradiction.begin(); it1!=words_importance_contradiction.end(); ++it1)
			{
				if(nb_contradiction[it1->first]!=0)
					words_importance_contradiction_avg[it1->first] = it1->second / (double)nb_contradiction[it1->first];
			}			
		}		
	}
	
	{
		output << "Neutral" << endl;
		// Declaring a set that will store the pairs using above comparision logic
		std::set<std::pair<std::string, float>, Comparator> setOfWords(
				words_importance_neutral_avg.begin(), words_importance_neutral_avg.end(), compFunctor);
	 
		// Iterate over a set using range base for loop
		// It will display the items in sorted order of values
		for (std::pair<std::string, float> element : setOfWords)
			output << element.first << " :: " << element.second << std::endl;		
	}
	output << endl;
	{
		output << "Entailment" << endl;
		// Declaring a set that will store the pairs using above comparision logic
		std::set<std::pair<std::string, float>, Comparator> setOfWords(
				words_importance_inf_avg.begin(), words_importance_inf_avg.end(), compFunctor);
	 
		// Iterate over a set using range base for loop
		// It will display the items in sorted order of values
		for (std::pair<std::string, float> element : setOfWords)
			output << element.first << " :: " << element.second << std::endl;		
	}
	output << endl;
	{
		output << "Contradiction" << endl;
		// Declaring a set that will store the pairs using above comparision logic
		std::set<std::pair<std::string, float>, Comparator> setOfWords(
				words_importance_contradiction_avg.begin(), words_importance_contradiction_avg.end(), compFunctor);
	 
		// Iterate over a set using range base for loop
		// It will display the items in sorted order of values
		for (std::pair<std::string, float> element : setOfWords)
			output << element.first << " :: " << element.second << std::endl;		
	}
	
	output.close();
}


void remplir_map(map<string, float>& imp, map<string, unsigned>& nb, string mot, float poids)
{
	map<string, float>::iterator it;				
	it = imp.find(mot);
	if (it == imp.end())
	{
		imp[mot] = 0;
		nb[mot] = 0;
	}
	imp[mot] += poids;
	++nb[mot];
}


void calcul_importance_moyenne(char* expl_filename, char* mots_lime_filename, char* output_f)
{
	
	ifstream expl(expl_filename, ios::in);
	if(!expl)
	{ 
		cerr << "Impossible to open the file " << expl_filename << endl;
		exit(EXIT_FAILURE);
	}		
	ifstream  mots_lime( mots_lime_filename, ios::in); // juste pour avoir les labels car erreur nom de labels dans les fichier d'explications
	if(!expl)
	{ 
		cerr << "Impossible to open the file " <<  mots_lime_filename << endl;
		exit(EXIT_FAILURE);
	}		
	
	map<string, float> words_importance_neutral;
	map<string, float> words_importance_inf;
	map<string, float> words_importance_contradiction;
	map<string, unsigned> nb_neutral;
	map<string, unsigned> nb_inf;
	map<string, unsigned> nb_contradiction;

	string word, mot;
	unsigned lab;
	unsigned num_sample=0;
	float poids=0;
	
	while(getline(mots_lime,word)) //lit -3
	{	
		// Extract the label (mots_pour_lime file)
		cout << "SAMPLE" << word << endl;
		if(word == " ")
			break;
			
		getline(mots_lime,word); //lit le label
		if(word == "neutral")
			lab=0;
		else if(word == "contradiction")
			lab=2;
		else if(word == "entailment")
			lab=1;
		else	
			cout << "ERROR LABEL" << endl;
		
		for(unsigned k=0; k<9;++k)
			getline(mots_lime,word);
			
		mot = "k";
		
		
		//calcul (expl file)
		for(unsigned i=0; i<3; ++i)
		{
			getline(expl, word);
			cout << word << endl;
		}		
		while(mot!="-3")
		{
			mot = extract_words_lime(expl, poids);
			if(mot=="-3")
				continue;
			if(lab==0)
				remplir_map(words_importance_neutral, nb_neutral, mot, poids);
			else if(lab==1)
				remplir_map(words_importance_inf, nb_inf, mot, poids);
			else	
				remplir_map(words_importance_contradiction, nb_contradiction, mot, poids);

		}
	}
	 
	calcul_imp_avg(words_importance_neutral, words_importance_inf, words_importance_contradiction, nb_neutral,
		nb_inf, nb_contradiction, output_f);

	expl.close();
	mots_lime.close();
}






int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "file explications fourni par une methode\n"
			 << "file mots pour lime\n"
			 << "sortie\n";
		exit(EXIT_FAILURE);
	}
	
	//fichier_pour_lime(argv[2], argv[3]);
	calcul_importance_moyenne(argv[1], argv[2], argv[3]);
	//verif_nb_expressions(argv[2]);
	return 0;
}


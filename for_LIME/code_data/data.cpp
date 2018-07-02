#include "data.hpp"
#include <algorithm>

using namespace std;
using namespace dynet;

/**
 * \file data.cpp
*/



BagOfWords::BagOfWords(string& word)
{
	if(word[0] == '{')
		important_bag = false;
	std::string::size_type sz;
	int wordID;
	for(unsigned i=1; i < word.size()-1; ++i)
	{
		stringstream ss;
		while(i < word.size()-1 && word[i] != ' ')
		{
			ss << word[i];
			++i;
		}
		w = ss.str();
		wordID = std::stoi(w,&sz);
		
		words.push_back(static_cast<unsigned>(wordID));
		cout << "ID = " << wordID << " , mot = " << word << endl;
	}
	
}


Data::Data(ifstream& database)
{
	string word, word2;
	database >> label;
	
	unsigned nb_bow;
	
	for(unsigned nb_sentences=0; nb_sentences < 2; ++nb_sentences)
	{
		database >> nb_bow;
		
		for(unsigned nb=0; nb < nb_bow; ++nb)
		{
			database >> word;
			while(word[word.size()-1] != ']' && word[word.size()-1] != '}')
			{
				database >> word2;
				word = word + " ";
				word = word + word2;
			}
			
			BagOfWords bow(word);
			if(nb_sentences==0)
				premise.push_back(bow);
			else
				hypothesis.push_back(bow);
				
			getline(database, word); //ignore mot de remplacement
		}	
	}
	
	database >> word; //lit -3 de fin
}



DataSet::DataSet(char* filename)
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
		Data data(database);
		dataset.push_back(data);
	}
	
	database.close();
}


#include "switch_words.hpp"
#include <algorithm>

using namespace std;

SW::SW(stringstream& fluxstring, string& word)
{
	std::string::size_type sz;
	int wordID;	
	string word2;
	
	while(word[word.size()-1] != '#')
	{
		fluxstring >> word2;
		word = word + " ";
		word = word + word2;
	}
	for(unsigned i=1; i < word.size()-1; ++i)
	{
		stringstream ss;
		while(i < word.size()-1 && word[i] != ' ')
		{
			ss << word[i];
			++i;
		}
		word2 = ss.str();
		wordID = std::stoi(word2,&sz);
		
		sw.push_back(static_cast<unsigned>(wordID));
		//cout << "\tID = " << wordID << " , mot = " << word << " (" <<  important_bag << ") "<< endl;
	}	
	float t;
	fluxstring >> t;
	if(t == -6)
		type = PREV;
	else if(t == -4)
		type = ACTUAL;
	else if(t == -5)
		type = NEXT;
	else
	{
		cerr << "ACTUAL, PREV ou NEXT indefini\n ligne = " << fluxstring.str();
		exit(EXIT_FAILURE);
	}
	
}

SwitchWords::SwitchWords(stringstream& fluxstring)
{
	string word;
	fluxstring >> word;
	while(word != "-1")
	{
		switch_w.push_back(new SW(fluxstring, word));
		fluxstring >> word;
	}
}

SwitchWords::~SwitchWords()
{
	for(unsigned i=0; i < switch_w.size(); ++i)
		delete switch_w[i];
}

SW::SW(SW const& copy)
{
	this->type = copy.type;
	for(unsigned i=0; i<copy.sw.size(); ++i)
		this->sw.push_back( copy.sw[i] );		
	
}

SwitchWords::SwitchWords(SwitchWords const& copy)
{
	for(unsigned i=0; i<copy.switch_w.size(); ++i)
		this->switch_w.push_back( new SW( *(copy.switch_w[i]) ) );	
}

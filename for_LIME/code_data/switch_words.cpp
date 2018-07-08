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

unsigned SW::get_type_sw()
{
	return type;
}

unsigned SW::get_nb_expr_sw()
{
	return sw.size();
}

void SW::print()
{
	for(unsigned i=0; i<sw.size(); ++i)
		cout << sw[i] << " ";
	if(type == ACTUAL)
		cout << "ACTUAL \n";
	else if(type == PREV)
		cout << "PREV \n";
	else if(type == NEXT)
		cout << "NEXT \n";
	else
		cout << "ERROR\n";
}

void SwitchWords::print()
{
	for(unsigned i=0; i<switch_w.size(); ++i)
	{
		cout << "SW " << i << endl;
		switch_w[i]->print();
	}
}

unsigned SwitchWords::get_word_id_sw(unsigned num_sw, unsigned num_word_in_sw)
{
	cout << "ok SwitchWords::get_word_id_sw = \n";
	return switch_w[num_sw]->get_word_id_sw(num_word_in_sw);
}
unsigned SW::get_word_id_sw(unsigned num_word_in_sw)
{
	cout << sw[num_word_in_sw] << endl;
	return sw[num_word_in_sw];
}

unsigned SwitchWords::get_nb_expr_sw(unsigned num_sw)
{
	cout << "SwitchWords::get_nb_expr_sw = " << switch_w[num_sw]->get_nb_expr_sw() << endl;
	return switch_w[num_sw]->get_nb_expr_sw();
}

unsigned SwitchWords::get_type_sw(unsigned num_sw)
{
	return switch_w[num_sw]->get_type_sw();
}
unsigned SwitchWords::get_nb_of_sw()
{
	return switch_w.size();
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

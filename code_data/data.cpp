#include "data.hpp"
#include <algorithm>

using namespace std;
using namespace dynet;

/**
 * \file data.cpp
*/

	/* Constructors */
	
/**
	* \brief Data constructor for the c++ server (to use LIME)
	* 
	* \param sentence : The concatenation of the premise and the hypothesis send by the client
	* \param word_to_id : Map containing the ID of each words
	* \param length_tab : Vector containing the length (number of words) of the premise and the hypothesis, for each sample :
	* 		For the ith sample : length premise is at num_sample*2, length hypothesis is at num_sample*2+1
	* \param num_sample : The sample
	* \param sample_label : The sample's label
*/ 
Data::Data(char* buffer_in, map<string,unsigned>& word_to_id, char* buffer_in_bis, unsigned num_sample, bool is_premise)
{
	//bool is_premise = true;
	unsigned cpt_words = 0;
	string word;
	//unsigned len_total = strlen(sentence);
	//unsigned len_sentence = length_tab[num_sample*2];
	unsigned i=0;
	vector<unsigned> tmp;
	//label.push_back(sample_label);
	label.push_back(0); //osef
	
	while(i < strlen(buffer_in)) //la prémisse si is_premise = true
	{
		stringstream ss;
		while(i<strlen(buffer_in) && buffer_in[i] != ' ')
		{
			ss << buffer_in[i];
			++i;
		}
		word = ss.str();
		if(word == "UNKWORDZ")
			tmp.push_back(0);
		else
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
			tmp.push_back(word_to_id[word]);			
		}
		++i; //passe a la lettre suivante
		if(i >= strlen(buffer_in))
		{
			if(is_premise)
			{
				premise.push_back(tmp);
			}
			else
			{
				hypothesis.push_back(tmp);
			}			
		}
	}
	i=0;
	while(i < strlen(buffer_in_bis)) //la prémisse si is_premise = true
	{
		stringstream ss;
		while(i<strlen(buffer_in_bis) && buffer_in_bis[i] != ' ')
		{
			ss << buffer_in_bis[i];
			++i;
		}
		word = ss.str();
		if(word == "UNKWORDZ")
			tmp.push_back(0);
		else
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
			tmp.push_back(word_to_id[word]);			
		}
		++i; //passe a la lettre suivante
		if(i >= strlen(buffer_in_bis))
		{
			if(is_premise)
			{
				hypothesis.push_back(tmp);
			}
			else
			{
				premise.push_back(tmp);
			}			
		}
	}
}	
	
	
/**
	* \brief Data constructor not used for interpretation
	* 
	* \param data_filename : File containing the samples in this form :
	*       label
	*       premise -1 premise's length
	*       hypothesis -1 hypothesis' length
*/
Data::Data(char* data_filename)
{
	ifstream data_file(data_filename, ios::in);
	if(!data_file)
	{ 
		cerr << "Impossible to open the file " << data_filename << endl;
		exit(EXIT_FAILURE);
	}
	int val;
	cerr << "Reading data from " << data_filename << " ...\n";

	while(data_file >> val) //read a label
	{
		label.push_back(val);
		init_rate(val);
		
		for(unsigned sentence=0; sentence<2; ++sentence)
		{
			vector<unsigned> tmp_data;
			data_file >> val; //read a word id
			while(val != -1)
			{
				tmp_data.push_back(static_cast<unsigned>(val));
				data_file >> val;
			}
			data_file >> val; //read the sentence's length
			if(sentence==0)
				premise.push_back(tmp_data);
			else
				hypothesis.push_back(tmp_data);
		}
	}
	
	data_file.close();
}

/**
	* \brief Data Constructor used for interpretation
	* 
	* \param test_explication_filename : File containing the samples in this form :
	*       label
	*       premise -1 premise's length
	*       hypothesis -1 hypothesis' length
	* 		important words in the premise -1
	* 		important words in the hypothesis -1
	* \param mode : Not really usefull (just to differientiate this constructor from another one)
*/
Data::Data(char* test_explication_filename, unsigned mode) 
{
	ifstream test_explication(test_explication_filename, ios::in);
	if(!test_explication)
	{ 
		cerr << "Impossible to open the file " << test_explication_filename << endl;
		exit(EXIT_FAILURE);
	}
	int val;
	cerr << "Reading data from " << test_explication_filename << " ...\n";
	unsigned sentence=0;
	while(test_explication >> val) //read a label
	{
		label.push_back(val);
		//init_rate(val);
		//cout << "\tSAMPLE "<< cpt << endl; 
		//++cpt;
		// reading the premise and the hypothesis
		for(sentence=0; sentence<2; ++sentence)
		{
			vector<unsigned> tmp_data;
			test_explication >> val; //read a word id
			while(val != -1)
			{
				tmp_data.push_back(static_cast<unsigned>(val));
				//cout << val << " ";
				test_explication >> val;
			}
			test_explication >> val; //read the sentence's length
			if(sentence==0)
				premise.push_back(tmp_data);
			else
				hypothesis.push_back(tmp_data);
			//cout << endl;
		}
		
		// reading the important words in the premise and the hypothesis
		for(sentence=0; sentence<2; ++sentence)
		{
			vector<int> tmp_data;
			test_explication >> val; //read a word id
			while(val != -1)
			{
				tmp_data.push_back(val);
				test_explication >> val;
			}				
			if(sentence==0)
				imp_words_premise.push_back(tmp_data);
			else
				imp_words_hypothesis.push_back(tmp_data);		
		}
	}
	
	test_explication.close();

}

/**
	* \brief Data constructor not used for interpretation
	* read an already-tokenized sample via stdin in this form :
	* label	premise's words -1 hypothesis' words -1
*/
Data::Data()
{
	//read the label
	int labels;
	cin >> labels; 
	if(labels > 2 || labels < 0)
	{
		cerr << "label must be : (0 : neutral, 1 : inference, or 2 : contradiction)\n";
		exit(EXIT_FAILURE);
	}
	label.push_back(static_cast<unsigned>(labels));
	
	int val=0;
	for(unsigned sentence=0; sentence<2; ++sentence)
	{
		vector<unsigned> tmp_data;
		cin >> val; //read a word id
		while(val != -1)
		{
			/*if(val == -2 && sentence==0)
				exit(EXIT_FAILURE);*/
			tmp_data.push_back(static_cast<unsigned>(val));
			cin >> val;
		}
		if(sentence==0)
			premise.push_back(tmp_data);
		else
			hypothesis.push_back(tmp_data);
	}
	
}

/**
	* \brief Data Constructor not used for interpretation
	* read a non-tokenized sample via stdin in this form :
	* label	premise's words -1 hypothesis' words -1
	* 
	* \param mode : Not really usefull (just to differientiate this constructor from another one)
	* \param lexique_filename : File containing the id of each vocabulary's word
*/

Data::Data(unsigned mode, char* lexique_filename)
{
	//read the label
	int labels;
	cin >> labels; 
	if(labels > 2 || labels < 0)
	{
		cerr << "label must be : (0 : neutral, 1 : inference, or 2 : contradiction)\n";
		exit(EXIT_FAILURE);
	}
	label.push_back(static_cast<unsigned>(labels));
	
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}
	
	cerr << "Reading " << lexique_filename << endl;
	
	string word;
	unsigned id;
	map<string, unsigned> word_to_id;
	while(lexique_file >> word && lexique_file >> id)
		word_to_id[word] = id;
	cerr << lexique_filename << " has been read" << endl;

	for(unsigned sentence=0; sentence<2; ++sentence)
	{
		vector<unsigned> tmp_data;
		cin >> word; //read a word
		while(word != "-1")
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
			//cerr << word << " ";
			tmp_data.push_back( word_to_id[word] );
			cin >> word;
		}
		if(sentence==0)
			premise.push_back(tmp_data);
		else
			hypothesis.push_back(tmp_data);
	}
	
}

/**
	* \name init_rate
	* \brief Initialize the number of each label in the dataset
	* Used in the Data constructor
	* 
	* \param label : The label
*/
void inline Data::init_rate(unsigned label)
{
	switch(label)
	{
		case NEUTRAL:
		{
			++nb_neutral;
			break;
		}
		case INFERENCE:
		{
			++nb_inference;
			break;
		}
		case CONTRADICTION:
		{
			++nb_contradiction;
			break;
		}
	}
}

	/* Getters and setters */

/**
	* \name get_couple_label
	* \brief Give the label of the couple "num_couple" from the sample "num_sample"
	* 
	* \param num_sample : The numero of the sample you're looking at
	* \param num_couple : The numero of the couple you're looking at
	* 
	* \return The label of the couple
*//*
unsigned Data::get_couple_label(unsigned num_sample, unsigned num_couple)
{
	return important_couples[num_sample].get_label(num_couple);
}*/

/**
	* \name get_nb_couple
	* \brief Give the number of couple for the sample "num_sample"
	* 
	* \param num_sample : The numero of the sample you're looking at
	* 
	* \return The number of couple
*//*
unsigned Data::get_nb_couple(unsigned num_sample)
{
	return important_couples[num_sample].get_size();
}*/

/**
	* \name get_couple_nb_words
	* \brief Give the number of words of the couple "num_couple" from the sample "num_sample", for the premise or for the hypothesis
	* 
	* \param num_sample : The numero of the sample you're looking at
	* \param num_couple : The numero of the couple you're looking at 
	* \param premise : True if you want to look at the important words in the premise, false if it's for the hypothesis
	* 
	* \return The number of words in the premise side or in the hypothesis side
*//*
unsigned Data::get_couple_nb_words(unsigned num_sample, unsigned num_couple, bool premise)
{
	return important_couples[num_sample].get_nb_words(num_couple, premise);
}*/

/**
	* \name get_couple_id
	* \brief Give the id of the word "num_mot" of the couple "num_couple" for the sample "num_sample", for the premise or for the hypothesis
	* 
	* \param num_sample : The numero of the sample you're looking at
	* \param num_couple : The numero of the couple you're looking at 
	* \param num_mot : Position of the word you want the id from (0 = first word, 1 = second word, ...)
	* \param premise : True if you want to look at the important words in the premise, false if it's for the hypothesis
	* 
	* \return The id of the word
*//*
unsigned Data::get_couple_id(unsigned num_sample, unsigned num_couple, unsigned num_mot, bool premise)
{
	return important_couples[num_sample].get_id(num_couple, num_mot, premise);
}*/

/**
	* \name get_nb_contradiction
	* \brief Give the number of contradiction label in the dataset
*/
unsigned Data::get_nb_contradiction() { return nb_contradiction; }
/**
	* \name get_nb_inf
	* \brief Give the number of inference label in the dataset
*/
unsigned Data::get_nb_inf() { return nb_inference; }
/**
	* \name get_nb_neutral
	* \brief Give the number of neutral label in the dataset
*/
unsigned Data::get_nb_neutral() { return nb_neutral; }

/**
	* \name get_word_id
	* \brief Give the word id
	* 
	* \param sentence : The sentence from where you pick the word (1 if you want the premise, 2 if you want the hypothesis)
	* \param num_sentence : The number of the sample processed
	* \param word_position : The position of the word
	* 
	* \return The word id
*/
unsigned Data::get_word_id(unsigned sentence, unsigned num_sentence, unsigned word_position)
{
	if(sentence==1)
		return premise[num_sentence][word_position];
	return hypothesis[num_sentence][word_position];
}

/**
	* \name get_label
	* \brief Give the label
	* 
	* \param num_sentence : The number of the sample processed
	* 
	* \return The label
*/
unsigned Data::get_label(unsigned num_sentence) { return label[num_sentence]; }

/**
	* \name get_nb_words
	* \brief Give the number of words in the sentence
	* 
	* \param sentence : The sentence from where you want to know the number of words 
	* (1 if you want the premise, 2 if you want the hypothesis)
	* \param num_sentence : The number of the sample processed
	* 
	* \return The number of words in the sentence
*/
unsigned Data::get_nb_words(unsigned sentence, unsigned num_sentence)
{
	if(sentence==1)
		return premise[num_sentence].size();
	return hypothesis[num_sentence].size();
}

/**
	* \name get_nb_sentences
	* \brief Give the number of samples
	* 
	* \return The number of samples
*/
unsigned Data::get_nb_sentences() { return premise.size(); }

unsigned Data::get_nb_words_total()
{
	unsigned nb_words = 0;
	for(unsigned i=0; i<premise.size(); ++i)
	{
		nb_words += get_nb_words(1, i);
		nb_words += get_nb_words(2, i);
	}
	return nb_words;
}


	/* Printing functions */

void Data::print_infos(unsigned type)
{
	const float nb_samples = static_cast<float>(label.size());
	
	if(type==0)
		cerr << "**TRAIN SET**\n";
	else if(type==1)
		cerr << "**DEV SET**\n";
	else
		cerr << "**TEST SET**\n";
	cerr << "\tNumber of samples = " << label.size() << endl;	
	cerr << "\tContradiction rate = " << nb_contradiction / nb_samples << endl;
	cerr << "\tInference rate = " << nb_inference / nb_samples << endl;
	cerr << "\tNeutral rate = " << nb_neutral / nb_samples << endl;
}


/**
	* \name print_sentences
	* \brief Print the entire dataset in a file in this form :
	*       label
	*       premise
	*       hypothesis
	* 
	* Just to debug.
	* \param name : The name of the output file
*/
void Data::print_sentences(char const* name)
{
	ofstream output_file(name, ios::out | ios::trunc);
	if(!output_file)
	{
		cerr << "Problem with the output file "<< name << endl;
		exit(EXIT_FAILURE);
	}
	unsigned k;
	unsigned j;
	for(unsigned i=0; i<premise.size(); ++i)
	{
		output_file << label[i] << endl;
		for(k=0; k<2; ++k)
		{
			if(k==0)
			{
				for(j=0; j<premise[i].size(); ++j)
					output_file << premise[i][j] <<' ';
				output_file << endl;
			}
			else
			{
				for(j=0; j<hypothesis[i].size(); ++j)
					output_file << hypothesis[i][j] <<' ';
				output_file << endl;
			}
		}
	}
	output_file.close();
}

/**
	* \name print_sentences_of_a_sample
	* \brief Print a sample in this form :
	*       premise
	*       hypothesis
	* 
	* Just to debug.
	* \param num_sample : The numero of the sample you want to print
*/
void Data::print_sentences_of_a_sample(unsigned num_sample)
{

	unsigned j;
	cerr << "premise = ";

	for(j=0; j<premise[num_sample].size(); ++j)
		cerr << premise[num_sample][j] <<' ';
	cerr << endl;
	
	cerr << "hypothesis = ";

	for(j=0; j<hypothesis[num_sample].size(); ++j)
		cerr << hypothesis[num_sample][j] <<' ';
	cerr << endl;
	
}

/**
	* \name print_sentences_of_a_sample
	* \brief Print a sample in a file in this form :
	*       premise
	*       hypothesis
	*
	* \param num_sample : The numero of the sample you want to print
	* \param output : file where to write 
*/
void Data::print_sentences_of_a_sample(unsigned num_sample, ofstream& output)
{

	unsigned j;

	for(j=0; j<premise[num_sample].size(); ++j)
		output << premise[num_sample][j] <<' ';
	output << "-1\n";

	for(j=0; j<hypothesis[num_sample].size(); ++j)
		output << hypothesis[num_sample][j] <<' ';
	output << "-1\n";
	
}

	/* Functions for interpretation */

/**
	* \name remove_couple
	* \brief Remove the couple-s listed in "num_couple" in the sample "num_sample"
	* For example if the couple in "num_couple" is "dog ; cat"
	* and premise = "the dog is sleeping" ; hypothesis = "the cat is sleeping"
	* then we have now premise = "the is sleeping" ; hypothesis = "the is sleeping"
	* (it does not really remove words, it simply set them to '0' which means 'no word here' for the RNN)
	* 
	* \param num_couple : Vector containing the couple's numero that you want to remove
	* \param num_sample : The numero of the sample from where you want to remove the couple-s
*//*
void Data::remove_couple(vector<unsigned>& num_couple, unsigned num_sample)
{
	unsigned word;
	int position;
	for(unsigned nb_couples=0; nb_couples < num_couple.size(); ++nb_couples) //parcourt des couples à supprimer (ex 0, 1, 2) dans le vecteur passé en arguments
	{
		
		//cerr << "couple numero " << num_couple[nb_couples] << endl;
		if( num_couple[nb_couples] >= important_couples[num_sample].get_size() ) //ne pas supprimer un couple qui n'existe pas
			continue;
		for(word=0; word < important_couples[num_sample].get_nb_words(num_couple[nb_couples], true); ++word) // parcours des mots du couple (partie prémisse)
		{
			position = important_couples[num_sample].get_position(num_couple[nb_couples], word, true);
			if(position != -4)
				premise[num_sample][position] = 0;
		}
		//idem pour l'hypothèse :
		for(word=0; word < important_couples[num_sample].get_nb_words(num_couple[nb_couples], false); ++word) // parcours des mots du couple (partie hypothèse)
		{
			position = important_couples[num_sample].get_position(num_couple[nb_couples], word, false);
			if(position != -4)
				hypothesis[num_sample][position] = 0;
		}
				
	}
	
}*/

/**
	* \name reset_couple
	* \brief Reset the couple-s precedently removed, listed in "num_couple" in the sample "num_sample"
	* 
	* \param num_couple : Vector containing the couple's numero that you want to reset
	* \param num_sample : The numero of the sample from where you want to reset the couple-s
*//*
void Data::reset_couple(vector<unsigned>& num_couple, unsigned num_sample)
{
	unsigned word;
	int position;
	for(unsigned nb_couples=0; nb_couples < num_couple.size(); ++nb_couples) //parcourt des couples à remettre (ex 0, 1, 2) dans le vecteur passé en arguments
	{
		//cerr << "couple numero " << num_couple[nb_couples] << endl;
		if( num_couple[nb_couples] >= important_couples[num_sample].get_size() ) //ne pas remettre un couple qui n'existe pas
			continue;
		for(word=0; word < important_couples[num_sample].get_nb_words(num_couple[nb_couples], true); ++word) // parcours des mots du couple (partie prémisse)
		{
			position = important_couples[num_sample].get_position(num_couple[nb_couples], word, true);
			if(position != -4)
				premise[num_sample][position] = important_couples[num_sample].get_id(num_couple[nb_couples], word, true);
		}
		//idem pour l'hypothèse :
		for(word=0; word < important_couples[num_sample].get_nb_words(num_couple[nb_couples], false); ++word) // parcours des mots du couple (partie hypothèse)
		{
			position = important_couples[num_sample].get_position(num_couple[nb_couples], word, false);
			if(position != -4)
				hypothesis[num_sample][position] = important_couples[num_sample].get_id(num_couple[nb_couples], word, false);
		}
				
	}	
}*/

/**
	* \name taking_couple
	* \brief Remove all the words in the premise and the hypothesis of the sample "num_sample", except for the words of the couple "num_couple"
	* For exemple if the couple is "dog ; cat" then the premise is now "dog" and the hypothesis is now "cat"
	* (it does not really remove words, it simply set them to '0' which means 'no word here' for the RNN)
	* 
	* \param num_couple : The couple's numero that you want to take
	* \param num_sample : The numero of the sample you're looking at
*//*
void Data::taking_couple(unsigned num_couple, unsigned num_sample)
{
	if( num_couple >= important_couples[num_sample].get_size() )
	{
		cerr << "Error : couple num " << num_couple << " doesn't exist (nb of couples for the sample " 
			 << num_sample << " = " << important_couples[num_sample].get_size() << ")\n";
		return;
	}
	unsigned word;
	unsigned word_in_sentences;
	bool remove;
	bool not_stoping = true;
	for(word_in_sentences=0; word_in_sentences < premise[num_sample].size() && not_stoping; ++word_in_sentences)
	{
		remove = true;
		for(word=0; word < important_couples[num_sample].get_nb_words(num_couple, true) && remove; ++word) // parcours des mots du couple (partie prémisse)
		{
			if(important_couples[num_sample].get_position(num_couple, word, true) == -4)
			{
				remove = false;
				not_stoping = false;
			}
			else
			{
				if(static_cast<int>(word_in_sentences) != important_couples[num_sample].get_position(num_couple, word, true))
					remove = true;
				else
					remove = false;
			}
		}
		if(remove)	
			premise[num_sample][word_in_sentences] = 0;
	}
	
	not_stoping = true;
	//idem pour l'hypothèse :
	for(word_in_sentences=0; word_in_sentences < hypothesis[num_sample].size() && not_stoping; ++word_in_sentences)
	{
		remove = true;
		for(word=0; word < important_couples[num_sample].get_nb_words(num_couple, false) && remove; ++word) // parcours des mots du couple (partie prémisse)
		{
			//si on ne peut pas le mettre en relation avec mot de la phrase adverse, alors on le met en relation avec tte la phrase
			// couple = ( tte la phrase ; mot )
			if(important_couples[num_sample].get_position(num_couple, word, false) == -4) 
			{
				remove = false;
				not_stoping = false;				
			}
			else
			{
				if(static_cast<int>(word_in_sentences) != important_couples[num_sample].get_position(num_couple, word, false))
					remove = true;
				else
					remove = false;
			}
		}
		if(remove)	
			hypothesis[num_sample][word_in_sentences] = 0;
	}
}*/

/**
	* \name reset_sentences
	* \brief Reset all the words in the premise or the hypothesis of the sample "num_sample"
	* 
	* \param original_premise : The original premise of the sample
	* \param original_hypothesis : The original hypothesis of the sample
	* \param num_sample : The numero of the sample you want to reset
	* \param is_premise : True if you want to reset the premise, false if you want to reset the hypothesis
*/
void Data::reset_sentences(vector<unsigned>& original_premise,vector<unsigned>& original_hypothesis, unsigned num_sample, bool is_premise)
{
	if(is_premise)
		for(unsigned j=0; j<original_premise.size(); ++j)
			premise[num_sample][j] = original_premise[j];
	else
		for(unsigned j=0; j<original_hypothesis.size(); ++j)
			hypothesis[num_sample][j] = original_hypothesis[j];

}

/**
	* \name is_empty
	* \brief Detect if a sentence (premise or hypothesis) of the sample "num_sample" is containing 0 words
	* Could be used for debugging
	* 
	* \param num_sample : The numero of the sample you want to check
	* \param is_premise : True if you want to check the premise, false if you want to check the hypothesis
	* 
	* \return True if the sentence (premise or hypothesis) is empty
*/
bool Data::is_empty(unsigned num_sample, bool is_premise)
{
	if(is_premise)
	{
		for(unsigned i=0; i<premise[num_sample].size(); ++i)
		{
			
			if(premise[num_sample][i] != 0)
				return false;
		}
		cerr << endl;
	}
	else
	{
		for(unsigned i=0; i<hypothesis[num_sample].size(); ++i)
		{
			
			if(hypothesis[num_sample][i] != 0)
				return false;
		}
		cerr << endl;
	}
	return true;
}

void Data::remove_words_from_stack(vector<pair<bool,bool>>& stack, unsigned num_sample)
{
	unsigned nb_words = premise[num_sample].size();
	unsigned i,j;
	for(i=0; i<nb_words; ++i)
		if(!stack[i].first)
			premise[num_sample][i] = 0;
	for(j=0 ; i<stack.size(); ++i, ++j)
		if(!stack[i].first)
			hypothesis[num_sample][j] = 0;
}

void Data::reset_words_from_stack(vector<pair<bool,bool>>& stack, vector<unsigned>& original_premise, vector<unsigned>& original_hypothesis, unsigned num_sample)
{
	unsigned nb_words = premise[num_sample].size();
	unsigned i, j;
	for(i=0; i<nb_words; ++i)
		if(!stack[i].first)
			premise[num_sample][i] = original_premise[i];
	for(j=0 ; i<stack.size(); ++i, ++j)
		if(!stack[i].first)
			hypothesis[num_sample][j] = original_hypothesis[j];
}

void Data::remove_words_from_vectors(unsigned word_position, unsigned num_sample, bool is_premise)
{
	if(is_premise)
		premise[num_sample][word_position] = 0;
	else
		hypothesis[num_sample][word_position] = 0;
}

void Data::reset_words_from_vectors(vector<unsigned>& sentence, unsigned word_position, unsigned num_sample, bool is_premise)
{
	if(is_premise)
		premise[num_sample][word_position] = sentence[word_position];
	else
		hypothesis[num_sample][word_position] = sentence[word_position];	
}

void Data::set_word(bool is_premise, unsigned word_position, unsigned word, unsigned num_sample)
{
	if(is_premise)
		premise[num_sample][word_position] = word;
	else
		hypothesis[num_sample][word_position] = word;
}


/* Important words */

unsigned Data::get_important_words(bool is_premise, unsigned num_sample, unsigned num_imp_word)
{
	num_imp_word *= 2; 
	if(is_premise) 
		return imp_words_premise[num_sample][num_imp_word];
	else
		return imp_words_hypothesis[num_sample][num_imp_word];
}

unsigned Data::get_important_words_position(bool is_premise, unsigned num_sample, unsigned num_imp_word)
{
	num_imp_word = num_imp_word * 2 + 1; 
	if(is_premise) 
		return imp_words_premise[num_sample][num_imp_word];
	else
		return imp_words_hypothesis[num_sample][num_imp_word];
}

unsigned Data::get_nb_important_words(bool is_premise, unsigned num_sample)
{
	if(is_premise)
	{
		if(imp_words_premise[num_sample][0] == -2)
			return 0;
		return imp_words_premise[num_sample].size() / 2;
	}
	else
	{
		if(imp_words_hypothesis[num_sample][0] == -2)
			return 0;
		return imp_words_hypothesis[num_sample].size() / 2;
	}
	
}

unsigned Data::get_nb_words_imp_total(unsigned limit)
{
	unsigned nb_words=0;
	for(unsigned num_sample=0; num_sample<limit; ++num_sample)
	{
		nb_words += get_nb_important_words(true,num_sample);
		nb_words += get_nb_important_words(false,num_sample);
	}
	return nb_words;
	
}

unsigned Data::get_nb_important_words_in_label(unsigned label, unsigned limit)
{
	unsigned nb = 0;
	for(unsigned i=0; i<limit; ++i)
	{
		if(get_label(i) == label)
		{
			nb += get_nb_important_words(true,i);
			nb += get_nb_important_words(false,i);
		}
	}
	return nb;
}


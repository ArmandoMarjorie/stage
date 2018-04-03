#include "data.hpp"

using namespace std;
using namespace dynet;

/**
 * \file data.cpp
*/

/* DATA CLASS */

/**
	* \brief Data constructor
	* 
	* \param data_filename : File containing the samples in this form :
	* 	label
	* 	premise
	* 	hypothesis
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
				sentence1.push_back(tmp_data);
			else
				sentence2.push_back(tmp_data);
		}
	}
	data_file.close();
}

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
		return sentence1[num_sentence][word_position];
	return sentence2[num_sentence][word_position];
}

/**
	* \name get_label
	* \brief Give the label
	* 
	* \param num_sentence : The number of the sample processed
	* 
	* \return The label
*/
unsigned Data::get_label(unsigned num_sentence)
{
	return label[num_sentence];
}

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
		return sentence1[num_sentence].size();
	return sentence2[num_sentence].size();
}

/**
	* \name get_nb_sentences
	* \brief Give the number of samples
	* 
	* \return The number of samples
*/
unsigned Data::get_nb_sentences()
{
	return sentence1.size();
}

/**
	* \name print_sentences
	* \brief Print the entire dataset in a file in this form :
	* 	label
	* 	premise
	* 	hypothesis
	* 
	* Just to debug.
	* \param name : The name of the output file
*/
void Data::print_sentences(char* name)
{
	ofstream output_file(name, ios::out | ios::trunc);
	if(!output_file)
	{
		cerr << "Problem with the output file "<< name << endl;
		exit(EXIT_FAILURE);
	}
	for(unsigned i=0; i<sentence1.size(); ++i)
	{
		output_file << label[i] << endl;	
		for(unsigned k=0; k<2; ++k)
		{
			for(unsigned j=0; j<sentence1[0].size(); ++j)
			{
				if(k==0)
					output_file << sentence1[i][j] <<' ';
				else
					output_file << sentence2[i][j] <<' ';
			}
			output_file << endl;
		}
	}
	output_file.close();
}

/* EMBEDDING CLASS */

/**
	* \brief Embeddings Constructor (training phase -- random embedding)
	*
	* \param model : The model
	* \param dim : The dimension of the embeddings
*/
Embeddings::Embeddings(ParameterCollection& model, unsigned dim) : p_c(), dim_embedding(dim)
{
	cerr<< "Initializing embeddings...\n";
	p_c = model.add_lookup_parameters(VOCAB_SIZE, {dim_embedding});
	vector<float> vector_zero(dim_embedding, 0); 
	p_c.initialize(0, vector_zero);
}

/**
	* \brief Embeddings Constructor (testing phase)
	* Initialize the word embedding with a file
	* 
	* \param embedding_filename : File containing the embedding
	* \param model : The model
	* \param dim : The dimension of the embeddings
*/
Embeddings::Embeddings(char* embedding_filename, ParameterCollection& model, unsigned dim, bool testing) : p_c(), dim_embedding(dim)
{
	ifstream emb_file(embedding_filename, ios::in);
	if(!emb_file)
	{ 
		cerr << "Impossible to open the file " << embedding_filename << endl;
		exit(EXIT_FAILURE);
	}
	p_c = model.add_lookup_parameters(VOCAB_SIZE, {dim_embedding});
	unsigned tmp;
	string word;
	vector<float> embedding(dim_embedding);
	unsigned index;
	if(testing)
		index=0;
	else
		index=1;
	while( emb_file >> word )
	{
		for(unsigned i=0; i<dim_embedding; ++i)
			emb_file >> embedding[i];
		p_c.initialize(index, embedding);
		++index;
	}
	emb_file.close();
}

/**
	* \name print_embedding
	* \brief Print the embedding of each words in a file. Just to debug.
	*
	* \param name : The name of the output file
*/
void Embeddings::print_embedding(char* output_filename)
{
	ComputationGraph cg;
	ofstream output_file(output_filename, ios::out | ios::trunc);
	if(!output_file)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	cerr << "Printing embeddings on " << output_filename << "...\n";
	for(unsigned i=0; i<VOCAB_SIZE; ++i)
		output_file << "c " << const_lookup(cg, p_c, i).value() << endl; //adding 'c' because the constructor read an emb file with each lines begining with a word
	
	output_file.close();
	
}

/**
	* \name get_embedding_expr
	* \brief Give the embedding of the word
	*
	* \param cg : The computation graph
	* \param index : The id of the word
	* 
	* \return The embedding of the word as an Expression
*/
Expression Embeddings::get_embedding_expr(ComputationGraph& cg, unsigned index)
{
	int num = static_cast<int>(index);
	if(num > VOCAB_SIZE)
	{
		cerr << "the vocab size need to be bigger (vocab size = " << VOCAB_SIZE <<", word id = "<< num << endl;
		exit(EXIT_FAILURE);
	}
	/*
	if(index > 28649)
		return const_lookup(cg, p_c, num);*/
	return lookup(cg, p_c, num);
}



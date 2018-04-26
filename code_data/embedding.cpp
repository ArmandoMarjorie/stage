#include "embedding.hpp"
#include <algorithm>

using namespace std;
using namespace dynet;


/**
 * \file embedding.cpp
*/

	/* Constructors */

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
	* \brief Embeddings Constructor (with a file containing pre-trained embedding)
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
	unsigned index;
	vector<float> embedding(dim_embedding);
	if(testing) index=0;
	else index=1;
	while( emb_file >> word )
	{
		for(unsigned i=0; i<dim_embedding; ++i)
			emb_file >> embedding[i];
		p_c.initialize(index, embedding);
		++index;
	}
	emb_file.close();
}

	/* Getter */

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

	/* Printing function */

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

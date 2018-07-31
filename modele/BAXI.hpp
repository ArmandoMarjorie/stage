#ifndef BILSTM_HPP
#define BILSTM_HPP

#include "rnn.hpp"


/**
 * \file BAXI.hpp
*/


/** 
 * \struct Detokenisation
 * \brief Detoken the words ID to their raw word.
*/
struct Detokenisation
{
	std::map<unsigned,std::string> id_to_word; /*!< words ID with their raw word */ 
	
	
	/**
		* \brief Detokenisation Constructor. Initializes a Detokenisation.
		*
		* \param lexique_filename : file containing the SNLI vocabulary 
		* with their ID.
	*/		
	Detokenisation(char* lexique_filename)
	{
		std::ifstream lexique_file(lexique_filename, std::ios::in);
		if(!lexique_file)
		{ 
			std::cerr << "Impossible to open the file " 
				<< lexique_filename << std::endl;
			exit(EXIT_FAILURE);
		}
		std::string word;
		int id;
		while(lexique_file >> word && lexique_file >> id)
			id_to_word[id] = word;
		lexique_file.close();
	}


	/**
		* \name detoken
		* \brief Detokens an expression.
		*
		* \param num_expr : numero of the expression.
		* \param is_premise : true if the expression is in the premise, 
		* else false.
		* \param data_copy : the original instance.
	*/	
	std::string detoken(unsigned num_expr, bool is_premise, 
		Data& data_copy)
	{
		std::string word, tmp="";
		unsigned nb_word;
		
		nb_word = data_copy.get_nb_words(is_premise, num_expr);
		for(unsigned i=0; i<nb_word; ++i)
		{
			tmp = tmp + id_to_word[data_copy.get_word_id(is_premise, num_expr, i)];
			if(nb_word>1 && i<nb_word-1)
				tmp = tmp + "_";
		}		
		
		return tmp;
	}
};


/** 
 * \struct Detokenisation
 * \brief Represents an explanation by the BAXI method.
*/
struct ExplanationsBAXI
{
	unsigned num_expr; /*!< Numero of the expression in the sentence. */ 
	bool is_premise;   /*!< True if the expression is in the premise, else false. */ 
	float importance;  /*!< The expression's importance. */ 
	
	
	/**
		* \brief ExplanationsBAXI Constructor. Initializes an ExplanationsBAXI.
		*
		* \param n : numero of the expression.
		* \param prem : true if the expression is in the premise, else false.
		* \param d : expression's importance.
	*/	
	ExplanationsBAXI(unsigned n, bool prem, float d) :
		num_expr(n), is_premise(prem), importance(d)
	{
		
	}


	/**
		* \brief > operator on the expression's importance. 
		* Sorts an ExplanationsBAXI vector (from best importance to worst).
		*
		* \param eb : ExplanationsBAXI to be compared with.
	*/		
	bool operator > (const ExplanationsBAXI& eb) const
	{
		return importance > eb.importance;
	}
};


void BAXI(RNN& rnn, 
		  dynet::ParameterCollection& model, 
		  DataSet& explication_set, 
		  Embeddings& embedding, 
		  char* parameters_filename, 
		  char* lexique_filename,
		  char* output_filename);


#endif

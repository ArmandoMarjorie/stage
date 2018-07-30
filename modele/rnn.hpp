#ifndef RNN_HPP
#define RNN_HPP

#include "dynet/nodes.h"
#include "dynet/dynet.h"
#include "dynet/training.h"
#include "dynet/timing.h"
#include "dynet/rnn.h"
#include "dynet/gru.h"
#include "dynet/lstm.h"
#include "dynet/dict.h"
#include "dynet/expr.h"
#include "dynet/cfsm-builder.h"
#include "dynet/hsm-builder.h"
#include "dynet/globals.h"
#include "dynet/io.h"
#include <functional>
#include "../code_data/dataset.hpp"

/**
 * \file rnn.hpp
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
		unsigned nb_word, sentence;
		if(is_premise)
			sentence=1;
		else
			sentence=2;
		
		nb_word = data_copy.get_nb_words(sentence, num_expr);
		for(unsigned i=0; i<nb_word; ++i)
		{
			tmp = tmp + id_to_word[data_copy.get_word_id(sentence, num_expr, i)];
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
	float DI; 		   /*!< The expression's importance. */ 
	
	
	/**
		* \brief ExplanationsBAXI Constructor. Initializes an ExplanationsBAXI.
		*
		* \param n : numero of the expression.
		* \param prem : true if the expression is in the premise, else false.
		* \param d : expression's importance.
	*/	
	ExplanationsBAXI(unsigned n, bool prem, float d) :
		num_expr(n), is_premise(prem), DI(d)
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
		return DI > eb.DI;
	}
};


class RNN
{
	protected:
		unsigned nb_layers; 						/*!< number of layers*/ 
		unsigned input_dim; 						/*!< dimention of the input  
													(the embedding dimension)*/
		unsigned hidden_dim; 						/*!< dimention of the hidden states*/ 
		float dropout_rate; 						/*!< dropout rate (between 0 and 1)*/ 
		unsigned systeme = 1; 						/*!< 1 or 2 for LSTM, 3 for BiLSTM*/
		bool apply_dropout = true;					/*!< applying dropout or not*/ 
		dynet::Parameter p_W; 						/*!< weight*/ 
		dynet::Parameter p_bias; 					/*!< bias*/
		dynet::VanillaLSTMBuilder* forward_lstm; 	/*!< forward LSTM*/ 
	
	
	public:
		RNN(unsigned nblayer, 
			unsigned inputdim, 
			unsigned hiddendim, 
			float dropout, 
			unsigned s, 
			dynet::ParameterCollection& model);
			
			
		float get_dropout_rate();
		unsigned get_nb_layers();
		unsigned get_input_dim();
		unsigned get_hidden_dim();
		void enable_dropout();
		void disable_dropout();
		
		virtual std::vector<float> predict(DataSet& set, 
										   Embeddings& embedding, 
										   unsigned num_sentence, 
										   dynet::ComputationGraph& cg, 
										   bool print_proba, 
										   unsigned& argmax) = 0;
										  
										  
		virtual dynet::Expression get_neg_log_softmax(DataSet& set, 
													  Embeddings& embedding, 
													  unsigned num_sentence, 
													  dynet::ComputationGraph& cg) = 0;
													  
													  
		std::vector<float> predict_algo(dynet::Expression& x, 
										dynet::ComputationGraph& cg, 
										bool print_proba, 
										unsigned& argmax);
										
										
		dynet::Expression get_neg_log_softmax_algo(dynet::Expression& score, 
												   unsigned num_sentence, 
												   DataSet& set);
};



void run_train(RNN& rnn, 
			   dynet::ParameterCollection& model, 
			   DataSet& train_set, 
			   DataSet& dev_set, 
			   Embeddings& embedding, 
			   char* output_emb_filename, 
			   unsigned nb_epoch, 
			   unsigned batch_size);
			   
			   
void run_predict(RNN& rnn, 
				 dynet::ParameterCollection& model, 
				 DataSet& test_set, 
				 Embeddings& embedding, 
				 char* parameters_filename);


std::vector<float> run_predict_for_server_lime(RNN& rnn, 
											   DataSet& test_set, 
											   Embeddings& embedding, 
											   bool print_label, 
											   unsigned num_sample);
											   
											   
void change_words_for_mesure(RNN& rnn, 
							 dynet::ParameterCollection& model, 
							 DataSet& explication_set, 
							 Embeddings& embedding, 
							 char* parameters_filename, 
							 char* lexique_filename);


#endif

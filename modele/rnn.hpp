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
 * \class RNN
 * \brief Class representing a RNN. 
 * The LSTM and BILSTM classes herit from this class.
*/	
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


#endif

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
	#include "../code_data/data.hpp"
	//#include "../tokenizer/detoken_explication.hpp"

	/**
	 * \file rnn.hpp
	*/

	class RNN
	{
		protected:
			unsigned nb_layers; /*!< number of layers*/ 
			unsigned input_dim; /*!< dimention of the input xt (dim of the embedding)*/
			unsigned hidden_dim; /*!< dimention of the hidden states ht and  ct*/ 
			float dropout_rate; /*!< dropout rate (between 0 and 1)*/ 
			unsigned systeme = 1; /*!< 1 or 2 for LSTM, 3 or 4 for BiLSTM*/
			bool apply_dropout = true; /*!< apllying dropout or not*/ 
			dynet::Parameter p_W; /*!< weight*/ 
			dynet::Parameter p_bias; /*!< bias*/
			dynet::VanillaLSTMBuilder* forward_lstm; /*!< forward LSTM*/ 
		
		public:
			RNN(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, dynet::ParameterCollection& model);
			float get_dropout_rate();
			unsigned get_nb_layers();
			unsigned get_input_dim();
			unsigned get_hidden_dim();
			void enable_dropout();
			void disable_dropout();
			virtual std::vector<float> predict(DataSet& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax, unsigned* important_couple) = 0;
			virtual dynet::Expression get_neg_log_softmax(DataSet& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg) = 0;
			std::vector<float> predict_algo(dynet::Expression& x, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax);
			dynet::Expression get_neg_log_softmax_algo(dynet::Expression& score, unsigned num_sentence, DataSet& set);

			
	};
	
	//void softmax_vect(std::vector<float>& tmp, std::vector<std::vector<float>>& alpha, unsigned& colonne);
	//void softmax_vect(std::vector<float>& tmp);
	
	void run_train(RNN& rnn, dynet::ParameterCollection& model, DataSet& train_set, DataSet& dev_set, Embeddings& embedding, char* output_emb_filename, unsigned nb_epoch, unsigned batch_size);
	void run_predict(RNN& rnn, dynet::ParameterCollection& model, DataSet& test_set, Embeddings& embedding, char* parameters_filename);
	dynet::Expression get_neg_log_softmax(RNN& rnn, DataSet& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
	void train_score(RNN& rnn, DataSet& train_set, Embeddings& embedding, unsigned nb_batches, 
		unsigned& nb_samples, unsigned batch_size, unsigned completed_epoch, unsigned nb_of_sentences, dynet::Trainer* trainer, std::vector<unsigned>& order, unsigned& numero_sentence);
	unsigned predict_dev_and_test(RNN& rnn, DataSet& dev_set, Embeddings& embedding, unsigned nb_of_sentences_dev, unsigned& best);
	void dev_score(RNN& rnn, dynet::ParameterCollection& model, DataSet& dev_set, Embeddings& embedding, std::string parameter_filename, unsigned nb_of_sentences_dev,
		unsigned& best, char* output_emb_filename);
		
	
	std::vector<float> run_predict_for_server_lime(RNN& rnn, DataSet& test_set, Embeddings& embedding, bool print_label);
	//void run_prediction_expl_for_sys_4(RNN& rnn, dynet::ParameterCollection& model, DataSet& explication_set, Embeddings& embedding, char* parameters_filename, char* lexique_filename);
	//void change_words_for_mesure(RNN& rnn, dynet::ParameterCollection& model, DataSet& explication_set, Embeddings& embedding, char* parameters_filename, char* lexique_filename,
	//	std::vector<Switch_Words*>& sw_vect);
	

#endif
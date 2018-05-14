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
	#include "../tokenizer/detoken_explication.hpp"

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
			virtual std::vector<float> predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax) = 0;
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg) = 0;
			std::vector<float> predict_algo(dynet::Expression& x, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax);
			dynet::Expression get_neg_log_softmax_algo(dynet::Expression& score, unsigned num_sentence, Data& set);

			
	};
	
	void softmax_vect(std::vector<float>& tmp, std::vector<std::vector<float>>& alpha, unsigned& colonne);
	void softmax_vect(std::vector<float>& tmp);
	
	void run_train(RNN& rnn, dynet::ParameterCollection& model, Data& train_set, Data& dev_set, Embeddings& embedding, char* output_emb_filename, unsigned nb_epoch, unsigned batch_size);
	void run_predict(RNN& rnn, dynet::ParameterCollection& model, Data& test_set, Embeddings& embedding, char* parameters_filename);
	dynet::Expression get_neg_log_softmax(RNN& rnn, Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
	void train_score(RNN& rnn, Data& train_set, Embeddings& embedding, unsigned nb_batches, 
		unsigned& nb_samples, unsigned batch_size, unsigned completed_epoch, unsigned nb_of_sentences, dynet::Trainer* trainer, std::vector<unsigned>& order, unsigned& numero_sentence);
	unsigned predict_dev_and_test(RNN& rnn, Data& dev_set, Embeddings& embedding, unsigned nb_of_sentences_dev, unsigned& best);
	void dev_score(RNN& rnn, dynet::ParameterCollection& model, Data& dev_set, Embeddings& embedding, std::string parameter_filename, unsigned nb_of_sentences_dev,
		unsigned& best, char* output_emb_filename);
		
		
	void run_predict_couple(RNN& rnn, dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename);
		
	void save_sentences(Data& explication_set,std::vector<unsigned>& premise,std::vector<unsigned>& hypothesis, unsigned num_sample);
	void write_couple(std::ofstream& output, Data& explication_set, unsigned num_sample, unsigned num_couple);
	void write_sentences(std::ofstream& output, std::vector<unsigned>& premise, std::vector<unsigned>& hypothesis);
	
	void run_predict_removing_couple(RNN& rnn, dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename);
	float calculate_DI(std::vector<float>& probs, std::vector<float>& original_probs, unsigned label_predicted);
	void calculate_DI_label(std::vector<float>& probs, std::vector<float>& original_probs, std::vector<float>& DI);

	
	std::vector<float> run_predict_for_server_lime(RNN& rnn, Data& test_set, Embeddings& embedding, bool print_label);
	
	void saving_branch(std::vector<std::pair<bool,bool>>& stack, std::vector<std::pair<unsigned,unsigned>>& save_premise, 
		std::vector<std::pair<unsigned,unsigned>>& save_hypothesis, Data& explication_set, unsigned num_sample);
	bool premise_empty(std::vector<std::pair<bool,bool>>& stack, unsigned premise_length);
	bool hypothesis_empty(std::vector<std::pair<bool,bool>>& stack, unsigned premise_length);
	void change_val(std::vector<std::pair<bool,bool>>& stack, unsigned& nb_words_removed);
	void generate_all_masks(RNN& rnn, dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename);
	void generate_couple_masks(RNN& rnn, dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename, char* lexique_filename);
	
	
	void print_stack(std::vector<std::pair<bool,bool>>& stack, unsigned premise_length);

#endif

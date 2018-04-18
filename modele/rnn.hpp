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

	/**
	 * \file rnn.hpp
	*/

	class RNN
	{
		protected:
			dynet::Parameter p_W; /*!< weight*/ 
			dynet::Parameter p_bias; /*!< bias*/
			dynet::VanillaLSTMBuilder* forward_lstm; /*!< forward LSTM*/ 
			unsigned nb_layers; /*!< number of layers*/ 
			unsigned input_dim; /*!< dimention of the input xt (dim of the embedding)*/
			unsigned hidden_dim; /*!< dimention of the hidden states ht and  ct*/ 
			float dropout_rate; /*!< dropout rate (between 0 and 1)*/ 
			bool apply_dropout = true; /*!< apllying dropout or not*/ 
			
		public:
			RNN(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, dynet::ParameterCollection& model);
			float get_dropout_rate();
			unsigned get_nb_layers();
			unsigned get_input_dim();
			unsigned get_hidden_dim();
			void enable_dropout();
			void disable_dropout();
			virtual unsigned predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba) = 0;
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg) = 0;
			unsigned predict_algo(dynet::Expression& x, dynet::ComputationGraph& cg, bool print_proba);
			dynet::Expression get_neg_log_softmax_algo(dynet::Expression& score, unsigned num_sentence, Data& set);

			
	};
	
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

	class LSTM : public RNN
	{
		private:
			unsigned systeme = 1;
			dynet::Expression sentence_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, dynet::ComputationGraph& cg);
			dynet::Expression run(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
			dynet::Expression systeme_1(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);
			dynet::Expression systeme_2(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);
			virtual unsigned predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba);
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);

		public:
			LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, dynet::ParameterCollection& model, unsigned systeme);
			
			/*void run_predict_verbose(dynet::ParameterCollection& model, Data& verbose_set, Embeddings& embedding, char* parameters_filename);
			void run_predict_explication(dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename);*/
			

	};

	/** 
	 * \class BiLSTM
	 * \brief Class representing a bidirectional LSTM
	*/
	class BiLSTM : public RNN
	{
		private:
			dynet::VanillaLSTMBuilder* backward_lstm; /*!< backward LSTM*/ 

			void words_representation(Embeddings& embedding, Data& set, unsigned sentence,
					dynet::ComputationGraph& cg, unsigned num_sentence, std::vector<dynet::Expression>& sentence_repr);
			dynet::Expression run_KIM(Data& set, Embeddings& embedding, unsigned num_sentence, 
					dynet::ComputationGraph& cg);
			void create_attention_matrix(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& matrix, 
					std::vector<dynet::Expression>& premise_lstm_repr, std::vector<dynet::Expression>& hypothesis_lstm_repr);
			void compute_beta(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& beta_matrix, 
					std::vector< std::vector<float> >& matrix);
			void compute_alpha(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& alpha_matrix, 
					std::vector< std::vector<float> >& matrix);
			void compute_a_context_vector(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& alpha_matrix,
					std::vector<dynet::Expression>& hypothesis_lstm_repr, unsigned premise_size, std::vector<dynet::Expression>& a_c_vect);
			void compute_b_context_vector(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& beta_matrix,
					std::vector<dynet::Expression>& premise_lstm_repr, unsigned hypothesis_size, std::vector<dynet::Expression>& b_c_vect);
			virtual unsigned predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba);
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);


		public:
			BiLSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, dynet::ParameterCollection& model);
	};

#endif

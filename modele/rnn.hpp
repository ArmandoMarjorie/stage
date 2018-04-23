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

	class LSTM : public RNN
	{
		private:
			dynet::Expression sentence_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, dynet::ComputationGraph& cg);
			dynet::Expression run(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
			dynet::Expression systeme_1(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);
			dynet::Expression systeme_2(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);
			virtual std::vector<float> predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax);
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);

		public:
			LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, dynet::ParameterCollection& model);
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
			dynet::Expression run_sys4(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
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
			virtual std::vector<float> predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax);
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);


		public:
			BiLSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, dynet::ParameterCollection& model);
	};

#endif

#ifndef LSTM_HPP
#define LSTM_HPP

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
	#include <string>
	#include "../code_data/data.hpp"

	/** Class representing a LSTM **/
	class LSTM
	{
		private:
			unsigned nb_layers; //Number of layers
			unsigned input_dim; //Dimention of the input xt (dim of the embedding)
			unsigned hidden_dim; //Dimention of the hidden states ht and  ct
			float dropout_rate;
			unsigned systeme = 1;
			bool apply_dropout = true;
			dynet::Parameter p_W; //weight
			dynet::Parameter p_bias; //bias
			dynet::VanillaLSTMBuilder* builder; 

			dynet::Expression sentence_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, dynet::ComputationGraph& cg);

			dynet::Expression run(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);

			dynet::Expression systeme_1(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);

			dynet::Expression systeme_2(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);

			dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);

			unsigned predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba);

			void enable_dropout();

			void disable_dropout();

			void dev_score(dynet::ParameterCollection& model, Data& dev_set, Embeddings& embedding, std::string parameter_filename, unsigned nb_of_sentences_dev, unsigned& best, char* output_emb_filename);

			void train_score(Data& train_set, Embeddings& embedding, unsigned nb_batches, 
				unsigned& nb_samples, unsigned batch_size, unsigned completed_epoch, unsigned nb_of_sentences, dynet::Trainer* trainer, std::vector<unsigned>& order, unsigned& numero_sentence);
				
			void usage_predict_verbose();

		public:
			LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned systeme, dynet::ParameterCollection& model);

			void run_predict(dynet::ParameterCollection& model, Data& test_set, Embeddings& embedding, char* parameters_filename);
			
			void run_predict_verbose(dynet::ParameterCollection& model, Data& verbose_set, Embeddings& embedding, char* parameters_filename);

			void run_train(dynet::ParameterCollection& model, Data& train_set, Data& dev_set, Embeddings& embedding, char* output_emb_filename, unsigned nb_epoch, unsigned batch_size);

	};

#endif

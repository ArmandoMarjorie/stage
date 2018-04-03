#ifndef DATA_HPP
#define DATA_HPP

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
	
	#define VOCAB_SIZE 36989
	#define NB_CLASSES 3

	/**
	 * \file data.hpp
	*/

	/** 
	 * \class Embeddings
	 * \brief Class representing the different embeddings 
	*/
	class Embeddings
	{
		private:
			dynet::LookupParameter p_c; /*!< table containing each word embedding*/ 
			unsigned dim_embedding = 100; /*!< dimension of the embedding*/ 
			
		public:
			Embeddings(dynet::ParameterCollection& model, unsigned dim);
			Embeddings(char* embedding_filename, dynet::ParameterCollection& model, unsigned dim, bool testing);
			void print_embedding(char* output_filename);
			dynet::Expression get_embedding_expr(dynet::ComputationGraph& cg, unsigned index);
	};
	
	/** 
	 * \class Data
	 * \brief Class representing a dataset (instance 1 object of this class for each dataset type : 1 for training, 1 for dev).
	*/
	class Data
	{
		private:
			std::vector< std::vector<unsigned> > sentence1; /*!< list of premises (sentence2[i] is the hypothesis of sentence1[i])*/ 
			std::vector< std::vector<unsigned> > sentence2; /*!< list of hypothesis (sentence1[i] is the premise of sentence2[i]*/ 
			std::vector<unsigned> label; /*!< list of labels (label[i] = label of the i_th sample)*/ 
			
		public:
			Data(char* data_filename);
			unsigned get_word_id(unsigned sentence, unsigned num_sentence, unsigned word_position);
			unsigned get_label(unsigned num_sentence);
			unsigned get_nb_words(unsigned sentence, unsigned num_sentence);
			unsigned get_nb_sentences();
			void print_sentences(char* name);
	};
	
#endif

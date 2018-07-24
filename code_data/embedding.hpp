#ifndef EMB_HPP
#define EMB_HPP

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
	#define NEUTRAL 0
	#define INFERENCE 1
	#define CONTRADICTION 2
	
	/**
	 * \file embedding.hpp
	*/

	/** 
	 * \class Embeddings
	 * \brief Class representing the different embeddings. 
	*/
	class Embeddings
	{
		private:
			dynet::LookupParameter p_c; /*!< Table containing each word embedding. */ 
			unsigned dim_embedding = 100; /*!< The embedding dimension. */ 

		public:
			Embeddings(dynet::ParameterCollection& model, unsigned dim);
			Embeddings(char* embedding_filename, dynet::ParameterCollection& model, unsigned dim, bool testing);
			void print_embedding(char* output_filename);
			void print_one_embedding(unsigned word_id);
			dynet::Expression get_embedding_expr(dynet::ComputationGraph& cg, unsigned index);
	};
	
#endif

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
	#define NEUTRAL 0
	#define INFERENCE 1
	#define CONTRADICTION 2

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
	
	class Couple
	{
		private:
		
			// unsigned ==> word, int ===> position of the word
			std::vector< std::vector< std::pair<unsigned,int> > > imp_word_premise;
			std::vector< std::vector< std::pair<unsigned,int> > > imp_word_hypothesis;
		
		public:
			Couple(std::ifstream& test_explication);
			void print_couples();
			unsigned get_id(unsigned num_couple, unsigned num_mot, bool premise);
			unsigned get_nb_words(unsigned num_couple, bool premise);
			unsigned get_size();
	};

	/** 
	 * \class Data
	 * \brief Class representing a dataset (instance 1 object of this class for each dataset type : 1 for training, 1 for dev).
	*/
	class Data
	{
		private:
			std::vector< std::vector<unsigned> > premise; /*!< list of premises (hypothesis[i] is the hypothesis of premise[i])*/ 
			std::vector< std::vector<unsigned> > hypothesis; /*!< list of hypothesis (premise[i] is the premise of hypothesis[i]*/ 
			std::vector<unsigned> label; /*!< list of labels (label[i] = label of the i_th sample)*/ 
			
			std::vector< Couple > important_couples;
			//std::vector< std::vector<bool> > marquage_couple_supp;
			
			unsigned nb_contradiction=0;
			unsigned nb_inference=0;
			unsigned nb_neutral=0;
			
			void inline init_rate(unsigned label);

		public:
			Data(char* data_filename);
			Data(unsigned mode);
			Data(unsigned mode, char* lexique_filename);
			Data(char* test_explication_filename, unsigned mode);
			/*Data(Data& original_set);
			Data(Data& original_set, unsigned num_couples_to_remove);*/
			
			unsigned get_couple_id(unsigned num_sample, unsigned num_couple, unsigned num_mot, bool premise);
			unsigned get_couple_nb_words(unsigned num_sample, unsigned num_couple, bool premise);
			unsigned get_nb_couple(unsigned num_sample);
			
			unsigned get_word_id(unsigned sentence, unsigned num_sentence, unsigned word_position);
			unsigned get_label(unsigned num_sentence);
			unsigned get_nb_words(unsigned sentence, unsigned num_sentence);
			unsigned get_nb_sentences();
			void print_sentences(char* name);
			void print_infos(unsigned type);
			unsigned get_nb_contradiction();
			unsigned get_nb_inf();
			unsigned get_nb_neutral();
	};

#endif

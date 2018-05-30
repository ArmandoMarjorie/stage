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
	#include <map>
	#include <vector>
	//#include "couple.hpp"
	#include "embedding.hpp"
	#include "switch_words.hpp"
	//#include "proba_bigram.hpp"
	
	
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
			
			//std::vector< Couple > important_couples;
			std::vector< std::vector<int> > imp_words_premise; /*!< list of the words in the premise explaining the label, for each sample*/ 
			std::vector< std::vector<int> > imp_words_hypothesis; /*!< list of the words in the hypothesis explaining the label, for each sample*/ 
			
			unsigned nb_contradiction=0;
			unsigned nb_inference=0;
			unsigned nb_neutral=0;
			
			void inline init_rate(unsigned label);

		public:
			Data(char* data_filename);
			Data();
			Data(unsigned mode, char* lexique_filename);
			Data(char* test_explication_filename, unsigned mode);
			Data(char* sentence, std::map<std::string,unsigned>& word_to_id, std::vector<unsigned>& length_tab, unsigned num_sample, unsigned sample_label);
			
			/*
			unsigned get_couple_id(unsigned num_sample, unsigned num_couple, unsigned num_mot, bool premise);
			unsigned get_couple_nb_words(unsigned num_sample, unsigned num_couple, bool premise);
			unsigned get_nb_couple(unsigned num_sample);*/
			
			unsigned get_word_id(unsigned sentence, unsigned num_sentence, unsigned word_position);
			unsigned get_label(unsigned num_sentence);
			unsigned get_nb_words(unsigned sentence, unsigned num_sentence);
			unsigned get_nb_sentences();
			void print_sentences(char const* name);
			void print_sentences_of_a_sample(unsigned num_sample);
			void print_sentences_of_a_sample(unsigned num_sample, std::ofstream& output);
			void print_infos(unsigned type);
			unsigned get_nb_contradiction();
			unsigned get_nb_inf();
			unsigned get_nb_neutral();/*
			unsigned get_couple_label(unsigned num_sample, unsigned num_couple);
			
			void remove_couple(std::vector<unsigned>& num_couple, unsigned num_sample);
			void reset_couple(std::vector<unsigned>& num_couple, unsigned num_sample);
			void taking_couple(unsigned num_couple, unsigned num_sample);*/
			void reset_sentences(std::vector<unsigned>& original_premise, std::vector<unsigned>& original_hypothesis, unsigned num_sample, bool is_premise);
			bool is_empty(unsigned num_sample, bool is_premise);
			
			void remove_words_from_stack(std::vector<std::pair<bool,bool>>& stack, unsigned num_sample);
			void reset_words_from_stack(std::vector<std::pair<bool,bool>>& stack, std::vector<unsigned>& original_premise, std::vector<unsigned>& original_hypothesis, unsigned num_sample);
			
			void reset_words_from_vectors(std::vector<unsigned>& sentence, unsigned word_position, unsigned num_sample, bool is_premise);
			void remove_words_from_vectors(unsigned word_position, unsigned num_sample, bool is_premise);
			
			void set_word(bool is_premise, unsigned word_position, unsigned word, unsigned num_sample);
			
			unsigned get_nb_words_total(); //for each sample : nb_word in prem + nb_word in hyp
			
			/* important words' methods */
			unsigned get_important_words(bool is_premise, unsigned num_sample, unsigned num_imp_word);
			unsigned get_important_words_position(bool is_premise, unsigned num_sample, unsigned num_imp_word);
			unsigned get_nb_important_words(bool is_premise, unsigned num_sample);
			unsigned get_nb_words_imp_total(unsigned limit);
			unsigned get_nb_important_words_in_label(unsigned label, unsigned limit);
	};

#endif

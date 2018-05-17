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
	#include "couple.hpp"
	#include "embedding.hpp"
	#include "switch_words.hpp"

/*
	class Triplet
	{
		private:
			unsigned word;
			unsigned contradictory_word;
			unsigned position;
		public:
			Triplet();
			unsigned get_word();
			unsigned get_contradictory_word();
			unsigned get_position();
	};
	
	class Contradictory_words
	{
		private:
			std::vector< Triplet > in_premise; // ex : { (le, la, 1) , (chat, grenouille, 2)} mots contradictoires : le chat / la grenouille
			std::vector< Triplet > in_hypothesis;
			
		
	};
	*/
	
	
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
			//std::vector< std::vector<unsigned> > couple_supp;
			
			unsigned nb_contradiction=0;
			unsigned nb_inference=0;
			unsigned nb_neutral=0;
			
			Switch_Words switch_w; //faire le constructeur special de data
			
			void inline init_rate(unsigned label);

		public:
			Data(char* data_filename);
			Data();
			Data(unsigned mode, char* lexique_filename);
			Data(char* test_explication_filename, unsigned mode);
			Data(char* sentence, std::map<std::string,unsigned>& word_to_id, std::vector<unsigned>& length_tab, unsigned num_sample, unsigned sample_label);
			
			unsigned get_couple_id(unsigned num_sample, unsigned num_couple, unsigned num_mot, bool premise);
			unsigned get_couple_nb_words(unsigned num_sample, unsigned num_couple, bool premise);
			unsigned get_nb_couple(unsigned num_sample);
			
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
			unsigned get_nb_neutral();
			unsigned get_couple_label(unsigned num_sample, unsigned num_couple);
			
			void remove_couple(std::vector<unsigned>& num_couple, unsigned num_sample);
			void reset_couple(std::vector<unsigned>& num_couple, unsigned num_sample);
			void taking_couple(unsigned num_couple, unsigned num_sample);
			void reset_sentences(std::vector<unsigned>& original_premise, std::vector<unsigned>& original_hypothesis, unsigned num_sample, bool is_premise);
			bool is_empty(unsigned num_sample, bool is_premise);
			
			void remove_words_from_stack(std::vector<std::pair<bool,bool>>& stack, unsigned num_sample);
			void reset_words_from_stack(std::vector<std::pair<bool,bool>>& stack, std::vector<unsigned>& original_premise, std::vector<unsigned>& original_hypothesis, unsigned num_sample);
			
			void reset_words_from_vectors(std::vector<unsigned>& sentence, unsigned word_position, unsigned num_sample, bool is_premise);
			void remove_words_from_vectors(unsigned word_position, unsigned num_sample, bool is_premise);
			
			void change_word(bool is_premise, unsigned word_position);
			void set_word(bool is_premise, unsigned word_position, unsigned word, unsigned num_sample);
	};

#endif

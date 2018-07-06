#ifndef DATA_SET_HPP
#define DATA_SET_HPP

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
	#include "embedding.hpp"
	//#include "switch_words.hpp"
	
	#define PREV 0
	#define ACTUAL 1
	#define NEXT 2

	class SW
	{
		private:
			std::vector<unsigned> sw;
			unsigned type; //ACTUAL, NEXT ou PREV
		public:
			SW();
	};
		
	class SwitchWords
	{
		private:
			std::vector<SW*> switch_w;	
		public:
			SwitchWords();
	};
	
	class BagOfWords
	{
		private:
			std::vector<unsigned> words;
			bool important_bag = true;
			std::vector<SwitchWords*> switch_words;
			
		public:
			BagOfWords(std::string& word);
			BagOfWords(const std::vector<unsigned>& wordsID, bool imp);
			BagOfWords(BagOfWords const& copy);
			~BagOfWords();
			
			unsigned get_nb_words();
			unsigned get_word_id(unsigned num_words);
			bool expr_is_important();
			void print_a_sample();
			
			void modif_BoW(unsigned mot_inconnu_ID, bool imp);
			void modif_BoW(BagOfWords& bow);
	};
	
	
	class Data
	{
		private:
			unsigned label;
			std::vector<BagOfWords*> premise;
			std::vector<BagOfWords*> hypothesis;
			
			
		public:
			Data(std::ifstream& database);
			Data(Data const& copy);
			~Data();
			
			Data* get_data_object();
			unsigned get_label();
			
			unsigned get_nb_words(unsigned sentence, unsigned num_expr);
			unsigned get_nb_expr(unsigned sentence);
			unsigned get_word_id(unsigned sentence, unsigned num_expr, unsigned num_words);	
			unsigned get_nb_imp_words(bool is_premise);		
			
			void modif_LIME(bool is_premise, unsigned num_buffer_in, unsigned position);
			unsigned search_position(bool is_premise, unsigned num_buffer_in);
			
			void print_a_sample();
			bool expr_is_important(bool is_premise, unsigned num_expr);
			
			void reset_data(Data const& data_copy);
	};
	
	class DataSet
	{
		private:
			std::vector<Data*> dataset;
			unsigned nb_inf=0;
			unsigned nb_neutral=0;
			unsigned nb_contradiction=0;
		public:
			DataSet(char* filename);
			~DataSet();
			
			Data* get_data_object(unsigned num_sample);
			unsigned get_word_id(unsigned sentence, unsigned num_sample, unsigned num_expr, unsigned num_words);
			
			unsigned get_nb_inf();
			unsigned get_nb_neutral();
			unsigned get_nb_contradiction();
			
			unsigned get_nb_words(unsigned sentence, unsigned num_sample, unsigned num_expr);
			unsigned get_nb_expr(unsigned sentence, unsigned num_sample);
			unsigned get_nb_sentences();
			
			unsigned get_label(unsigned num_sample);
			void print_a_sample(unsigned num_sample);
			
			void modif_LIME(char* buffer_in, unsigned num_sample);
	
			void print_everything();
			bool expr_is_important(unsigned num_sample, bool is_premise, unsigned num_expr);
			
			void reset_data(Data const& data_copy, unsigned num_sample);
	};
	

#endif

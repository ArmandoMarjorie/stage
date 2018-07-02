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
	
	
	
	class BagOfWords
	{
		private:
			std::vector<unsigned> words;
			bool important_bag = true;
		public:
			BagOfWords(std::string& word);
			unsigned get_nb_words();
			unsigned get_word_id(unsigned num_words);
	};
	
	
	class Data
	{
		private:
			unsigned label;
			std::vector<BagOfWords> premise;
			std::vector<BagOfWords> hypothesis;
			
		public:
			Data(std::ifstream& database);
			unsigned get_label();
			
			unsigned get_nb_words(unsigned sentence, unsigned num_expr);
			unsigned get_nb_expr(unsigned sentence);
			unsigned get_word_id(unsigned sentence, unsigned num_expr, unsigned num_words);			
	};
	
	class DataSet
	{
		private:
			std::vector<Data> dataset;
			unsigned nb_inf=0;
			unsigned nb_neutral=0;
			unsigned nb_contradiction=0;
		public:
			DataSet(char* filename);
			unsigned get_word_id(unsigned sentence, unsigned num_sample, unsigned num_expr, unsigned num_words);
			
			unsigned get_nb_inf();
			unsigned get_nb_neutral();
			unsigned get_nb_contradiction();
			
			unsigned get_nb_words(unsigned sentence, unsigned num_sample, unsigned num_expr);
			unsigned get_nb_expr(unsigned sentence, unsigned num_sample);
			unsigned get_nb_sentences();
			
			unsigned get_label(unsigned num_sample);
	};
	

#endif

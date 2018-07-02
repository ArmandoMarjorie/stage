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
	#include "../code_data/embedding.hpp"
	//#include "switch_words.hpp"
	
	
	
	class BagOfWords
	{
		private:
			std::vector<unsigned> words;
			bool important_bag = true;
		public:
			BagOfWords(std::string& word);
	};
	
	
	class Data
	{
		private:
			unsigned label;
			std::vector<BagOfWords> premise;
			std::vector<BagOfWords> hypothesis;
			
		public:
			Data(std::ifstream& database);
	};
	
	class DataSet
	{
		private:
			std::vector<Data> dataset;
		public:
			DataSet(char* filename);
	};
	

#endif

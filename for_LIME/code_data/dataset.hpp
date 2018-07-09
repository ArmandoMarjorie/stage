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
	#include "embedding.hpp"
	#include "data.hpp"
	
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
			void modif_LIME_random(char* buffer_in, unsigned num_sample);
	
			void print_everything();
			bool expr_is_important(unsigned num_sample, bool is_premise, unsigned num_expr);
			
			void reset_data(Data const& data_copy, unsigned num_sample);
	};
	

#endif

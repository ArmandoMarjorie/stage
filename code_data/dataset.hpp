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


/**
 * \file dataset.hpp
*/


/** 
 * \class DataSet
 * \brief Class representing all the instances, either 
 * from the testing_token_id_texts file or the token_id_texts/test file. 
*/
class DataSet
{
	private:
		std::vector<Data*> dataset;  /*!< All the instances. */
		unsigned nb_inf=0;			 /*!< Number of 'entailment' instances. */
		unsigned nb_neutral=0; 		 /*!< Number of 'neutral' instances. */
		unsigned nb_contradiction=0; /*!< Number of 'contradiction' instances. */
		
		inline void init_labels_infos(unsigned lab, unsigned num_sample);
		
	public:
		DataSet(char* filename);
		
		DataSet(char* data_filename, 
				int not_interpret);
				
		~DataSet();
		
		Data* get_data_object(unsigned num_sample);
		
		unsigned get_word_id(bool is_premise, 
							 unsigned num_sample, 
							 unsigned num_expr,
							 unsigned num_words);
							 
		unsigned get_nb_switch_words(bool is_premise, 
									 unsigned num_expr, 
									 unsigned num_sample);
		
		unsigned get_nb_inf();
		
		unsigned get_nb_neutral();
		
		unsigned get_nb_contradiction();
		
		unsigned get_nb_words(bool is_premise, 
							  unsigned num_sample, 
							  unsigned num_expr);
							  
		unsigned get_nb_expr(bool is_premise, 
							 unsigned num_sample);
							 
		unsigned get_nb_instances();
		
		unsigned get_label(unsigned num_sample);
		
		void print_a_sample(unsigned num_sample);
		
		void modif_LIME(char* buffer_in, 
						unsigned num_sample);
		
		void modif_LIME_random(char* buffer_in, 
							   unsigned num_sample);
		
		void modif_word(bool is_premise, 
						unsigned num_expr, 
						unsigned num_sw_words, 
						unsigned num_sample);

		void print_everything();
		
		bool expr_is_important(unsigned num_sample, 
							   bool is_premise, 
							   unsigned num_expr);
		
		void reset_data(Data const& data_copy, 
						unsigned num_sample);
};
	

#endif

#ifndef DATA_HPP
#define DATA_HPP
	
	
	#include "instance_expression.hpp"
	
	/**
	 * \file data.hpp
	*/

	/** 
	 * \class Data
	 * \brief Class representing an instance. 
	*/
	class Data
	{
		private:
			unsigned label; 							/*!< The label : 
															Entailment, 
															Contradiction, 
															Neutral */ 
			std::vector<InstanceExpression*> premise; 	/*!< The premise. 
														 Each case is a word or an expression. */ 
			std::vector<InstanceExpression*> hypothesis;/*!< The hypothesis. 
														 Each case is a word or an expression. */ 
			
		public:
			Data(std::ifstream& database);
			Data(std::ifstream& database, unsigned lab);
			Data(Data const& copy);
			~Data();
			
			Data* get_data_object();
			
			unsigned get_label();
			unsigned get_nb_switch_words(bool is_premise, unsigned num_expr);
			unsigned get_nb_words(bool is_premise, unsigned num_expr);
			unsigned get_nb_expr(bool is_premise);
			unsigned get_word_id(bool is_premise, unsigned num_expr, unsigned num_words);	
			unsigned get_nb_imp_words(bool is_premise);		
			
			void modif_word(bool is_premise, unsigned num_expr, unsigned num_sw_words);
			void modif_LIME(bool is_premise, unsigned position);
			void modif_LIME_random(bool is_premise, unsigned position);
			unsigned search_position(bool is_premise, unsigned num_buffer_in);
			bool expr_is_important(bool is_premise, unsigned num_expr);
			
			void print_a_sample();
			void reset_data(Data const& data_copy);
	};
	
#endif

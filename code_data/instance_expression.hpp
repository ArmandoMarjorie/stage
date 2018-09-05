#ifndef INSTANCE_EXPR_HPP
#define INSTANCE_EXPR_HPP

	#include "switch_words.hpp"

	/**
	 * \file instance_expression.hpp
	*/

	/** 
	 * \class InstanceExpression
	 * \brief Class representing a word/an expression of a sentence 
	 * (the premise or the hypothesis). 
	*/
	class InstanceExpression
	{
		private:
			std::vector<unsigned> words; 			/*!< The expression.
														 Ex : "in front of" : 
														 words[0] = in
														 words[1] = front
														 words[2] = of */ 
														 
			bool important_bag = true; 				/*!< True if we evaluate the expression importance, 
														 False otherwise. (We do not evaluate "the" for example).*/
			
			std::vector<SwitchWords*> switch_words; /*!< All the alternative expressions (up to 3). 
														 Ex : the alternative expressions are "in front of", "outside", and "next to". 
														 Then : switch_words[0] = in front of, switch_words[1] = outside, switch_words[2] = next to */ 
			
		public:
			InstanceExpression(std::string& line);
			
			InstanceExpression(std::string& word, 
							   bool original_lime);
							   
			InstanceExpression(unsigned word);
			
			InstanceExpression(const std::vector<unsigned>& wordsID, 
							   bool imp);
							   
			InstanceExpression(InstanceExpression const& copy);
			
			~InstanceExpression();
			
			unsigned get_nb_words();
			
			unsigned get_word_id(unsigned num_words);
			
			bool expr_is_important();
			
			void print_a_sample();
			
			void modif_InstanceExpression_random(bool imp);
			
			void modif_InstanceExpression(unsigned num_switch_words, 
										  unsigned num_sw, 
										  bool imp);
			
			void modif_InstanceExpression(
				InstanceExpression& instance_expression);
			
			void modif_InstanceExpression(
				InstanceExpression const& current_instance_expression, 
				unsigned num_switch_words, 
				unsigned num_sw, 
				bool imp);
			
			void modif_InstanceExpression(unsigned UNK, 
										  bool imp);
			
			unsigned get_nb_switch_words();
			
			unsigned get_type_sw(unsigned num_switch_word, 
								 unsigned num_sw);
			
			unsigned get_nb_of_sw(unsigned num_switch_word);
			
			void print();
	};

#endif

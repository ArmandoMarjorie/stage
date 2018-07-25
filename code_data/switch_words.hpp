#ifndef SW_HPP
#define SW_HPP
	
	#include <string>
	#include <vector>
	#include <iostream>
	#include <sstream> 
	#include <fstream>
	
	#define PREV 0
	#define ACTUAL 1
	#define NEXT 2
	
	/**
	 * \file switch_words.hpp
	*/

	/** 
	 * \class SW
	 * \brief Class representing a "piece" of alternative expression.
	*/
	
	class SW
	{
		private:
			std::vector<unsigned> sw; 	/*!< A "piece" of an alternative expression. See the attribute "switch_w" from the class "SwitchWords" to understand better.
											 Ex : "in front of" ---> sw[0] = in, sw[1] = front, sw[2] = of */ 
			
			unsigned type; 				/*!< PREV, ACTUAL or NEXT. Defines where to place the expression "sw" in the sentence. 
											 PREV : replaces the expression before the current expression, 
											 ACTUAL : replaces the current expression, 
											 NEXT : replaces the expression after the current expression */ 
			
			inline unsigned init_type(int type, std::string& line);
		public:
			SW(std::stringstream& fluxstring, std::string& word);
			SW(SW const& copy);
			unsigned get_type_sw();
			unsigned get_nb_expr_sw();
			unsigned get_word_id_sw(unsigned num_word_in_sw);
			void print();
			
	};
	
	/** 
	 * \class SwitchWords
	 * \brief Class representing an alternative expressions.
	*/	
		
	class SwitchWords
	{
		private:
			std::vector<SW*> switch_w; /*!< An alternative expression. 
											For example, the sentence is "an animal..." and the alternative expression for "animal" is "big dog". 
											Then we have two SW to make a "real" sentence :  
											"a" PREV
											"big dog" ACTUAL
											
											Then : 
											switch_w[0] = "a", PREV
											switch_w[1] = "big dog", ACTUAL */ 	
		public:
			SwitchWords(std::stringstream& fluxstring);
			SwitchWords(SwitchWords const& copy);
			~SwitchWords();
			
			unsigned get_type_sw(unsigned num_sw);
			unsigned get_nb_of_sw();
			unsigned get_nb_expr_sw(unsigned num_sw);
			unsigned get_word_id_sw(unsigned num_sw, unsigned num_word_in_sw);
			void print();
	};
	
#endif

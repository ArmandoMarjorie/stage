#ifndef BOW_HPP
#define BOW_HPP

	#include "switch_words.hpp"

	/**
	 * \file bow.hpp
	*/

	/** 
	 * \class BagOfWords
	 * \brief Class representing a word/an expression of a sentence (the premisse or the hypothesis). 
	*/
	class BagOfWords
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
			BagOfWords(std::string& line);
			BagOfWords(const std::vector<unsigned>& wordsID, bool imp);
			BagOfWords(BagOfWords const& copy);
			~BagOfWords();
			
			unsigned get_nb_words();
			unsigned get_word_id(unsigned num_words);
			bool expr_is_important();
			void print_a_sample();
			
			void modif_BoW_random(bool imp);
			void modif_BoW(unsigned num_switch_words, unsigned num_sw, bool imp);
			void modif_BoW(BagOfWords& bow);
			void modif_BoW(BagOfWords const& current_bow, unsigned num_switch_words, unsigned num_sw, bool imp);
			
			unsigned get_nb_switch_words();
			
			unsigned get_type_sw(unsigned num_switch_word, unsigned num_sw);
			unsigned get_nb_of_sw(unsigned num_switch_word);
			
			void print();
	};

#endif

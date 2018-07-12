#ifndef BOW_HPP
#define BOW_HPP

	#include "switch_words.hpp"

	class BagOfWords
	{
		private:
			std::vector<unsigned> words;
			bool important_bag = true;
			std::vector<SwitchWords*> switch_words;
			
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

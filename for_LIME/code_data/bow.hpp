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
			
			void modif_BoW(unsigned mot_inconnu_ID, bool imp);
			void modif_BoW(BagOfWords& bow);
	};

#endif

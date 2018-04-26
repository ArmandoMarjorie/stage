#ifndef COUPLE_HPP
#define COUPLE_HPP

	#include <string>
	#include <vector>
	#include <utility>
	#include <iostream>
	#include <fstream>

	class Couple
	{
		private:
			// unsigned ==> word, int ==> position of the word
			std::vector< std::vector< std::pair<unsigned,int> > > imp_word_premise;
			std::vector< std::vector< std::pair<unsigned,int> > > imp_word_hypothesis;
			
			std::vector<unsigned> labels; /*!< label of each couple*/
		
		public:
			Couple(std::ifstream& test_explication);
			void print_couples(unsigned num_couple);
			unsigned get_id(unsigned num_couple, unsigned num_mot, bool premise);
			int get_position(unsigned num_couple, unsigned num_mot, bool premise);
			unsigned get_nb_words(unsigned num_couple, bool premise);
			unsigned get_size();
			unsigned get_label(unsigned num_couple);
	};

#endif

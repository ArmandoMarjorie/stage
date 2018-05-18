#ifndef PB_HPP
#define PB_HPP

	#include <string>
	#include <map>
	#include <vector>
	#include <utility>
	#include <iostream>
	#include <fstream>
	
	
	/** 
	 * \class Switch_Words
	 * \brief Class representing the database containing, for each word in the explication_test file, words that can replace them
	 * ex : dog cat animal man person human
	 * dog is the real word in the dataset, the rest can replace it
	*/
	class Proba_Bigram
	{
		private:
			unsigned v;
			double alpha;
			std::map< std::pair<unsigned, unsigned>, double > proba;
			std::map<  unsigned, unsigned > unigram;
		
		public:
			Proba_Bigram(char* filename, char* uni_filename);
			double get_proba_log(unsigned word1, unsigned word2);
			bool is_equal(char* true_filename, char* generating_filename);
			void print_prob(char* fi, char* fi2);
	};

#endif


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
			std::vector< unsigned > unigram;
		
		public:
			Proba_Bigram(char* filename);
	};

#endif


#ifndef Switch_Words_HPP
#define Switch_Words_HPP

	#include <string>
	#include <vector>
	#include <utility>
	#include <map>
	#include <iostream>
	#include <fstream>
	
	
	/** 
	 * \class Switch_Words
	 * \brief Class representing the database containing, for each word in the explication_test file, words that can replace them
	 * ex : dog cat animal man person human
	 * dog is the real word in the dataset, the rest can replace it
	*/
	class Switch_Words
	{
		private:
			std::map<unsigned, unsigned> correspondance;
			std::vector<std::vector<unsigned>> sw; /*!< */
		
		public:
			Switch_Words(char* filename);
			unsigned get_corresponding(unsigned word_id);
			unsigned get_nb_switch_words(unsigned word_id);
			unsigned get_switch_word(unsigned word_id, unsigned num_switch_word);
	};

#endif

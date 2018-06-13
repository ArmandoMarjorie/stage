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
	class RW
	{
		private:
			unsigned word;
			unsigned position; 
			bool insert;
		
		public:
			RW(unsigned w, unsigned p, int in);
			bool is_insert();
			unsigned get_word();
			unsigned get_position();
	};	
	
	class SW
	{
		private:
			std::vector<std::pair<unsigned,unsigned>> real_words; 
			std::vector<std::vector<RW>> remplacing_words; 
		
		public:
			SW(std::ifstream& database);
			unsigned get_word(unsigned num_remplace, unsigned num_w);
			unsigned get_nb_replace();
			unsigned get_nb_replace_word(unsigned num_remplace);
			unsigned get_position(unsigned num_remplace, unsigned num_w);
			unsigned is_insert(unsigned num_remplace, unsigned num_w);
	};
	

	
	class Switch_Words
	{
		private:
			std::vector<SW> prem;  //toutes les prémises du jeu de données
			std::vector<SW> hyp;
		public:
			Switch_Words(char* filename);
			unsigned get_word(unsigned num_remplace, unsigned num_w, bool is_premise, unsigned num_sample);
			unsigned get_nb_replace_word(unsigned num_remplace, bool is_premise, unsigned num_sample);
			unsigned get_nb_replace(bool is_premise, unsigned num_sample);
			unsigned get_position(unsigned num_remplace, unsigned num_w, bool is_premise, unsigned num_sample);
			unsigned is_insert(unsigned num_remplace, unsigned num_w, bool is_premise, unsigned num_sample);
			
	};
	
#endif

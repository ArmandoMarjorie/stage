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

	class SW
	{
		private:
			std::vector<unsigned> sw;
			unsigned type; //ACTUAL, NEXT ou PREV
		public:
			SW(std::stringstream& fluxstring, std::string& word);
			SW(SW const& copy);
			unsigned get_type_sw();
			unsigned get_nb_expr_sw();
			unsigned get_word_id_sw(unsigned num_word_in_sw);
			void print();
	};
		
	class SwitchWords
	{
		private:
			std::vector<SW*> switch_w;	
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

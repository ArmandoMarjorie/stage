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
	};
		
	class SwitchWords
	{
		private:
			std::vector<SW*> switch_w;	
		public:
			SwitchWords(std::stringstream& fluxstring);
			SwitchWords(SwitchWords const& copy);
			~SwitchWords();
	};
	
#endif

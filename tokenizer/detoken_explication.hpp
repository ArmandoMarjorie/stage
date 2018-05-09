#ifndef DETOKEN_HPP
#define DETOKEN_HPP

	#include <iostream>
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>
	#include <cstdlib>
	#include <map>
	#include <algorithm>
	#include <locale>

	std::string detoken_label(int label);

	void reading_lexique(char* lexique_filename, std::map<unsigned, std::string>& id_to_word, bool DI);

	void detokenizer_with_couple(char* lexique_filename, char* explication_filename, char* output_filename, bool DI);
	
	void detoken_expl(char* lexique_filename, char* explication_filename, char* output_filename);

#endif

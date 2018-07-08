#ifndef DATA_HPP
#define DATA_HPP
	
	
	#include "bow.hpp"
	
	
	class Data
	{
		private:
			unsigned label;
			std::vector<BagOfWords*> premise;
			std::vector<BagOfWords*> hypothesis;
			
			
		public:
			Data(std::ifstream& database);
			Data(Data const& copy);
			~Data();
			
			Data* get_data_object();
			unsigned get_nb_switch_words(bool is_premise, unsigned num_expr);
			unsigned get_label();
			
			unsigned get_nb_words(unsigned sentence, unsigned num_expr);
			unsigned get_nb_expr(unsigned sentence);
			unsigned get_word_id(unsigned sentence, unsigned num_expr, unsigned num_words);	
			unsigned get_nb_imp_words(bool is_premise);		
			
			void modif_LIME(bool is_premise, unsigned position);
			unsigned search_position(bool is_premise, unsigned num_buffer_in);
			
			void print_a_sample();
			bool expr_is_important(bool is_premise, unsigned num_expr);
			
			void reset_data(Data const& data_copy);
	};
	
#endif

#ifndef BILSTM_HPP
#define BILSTM_HPP

#include "../code_data/dataset.hpp"
#include "rnn.hpp"

/**
 * \file BILSTM.hpp
*/

/** 
 * \class BiLSTM
 * \brief Class representing a bidirectional LSTM. Herits from the RNN class.
*/
class BiLSTM : public RNN
{
	private:
		dynet::VanillaLSTMBuilder* backward_lstm; 	/*!< backward LSTM*/ 

		void words_representation(Embeddings& embedding, 
								  DataSet& set,
								  bool is_premise,
								  dynet::ComputationGraph& cg, 
								  unsigned num_sentence, 
								  std::vector<dynet::Expression>& sentence_repr);
				
				
		dynet::Expression run_KIM(DataSet& set, 
								  Embeddings& embedding, 
								  unsigned num_sentence, 
								  dynet::ComputationGraph& cg);
				
				
		void create_attention_matrix(std::vector< std::vector<float> >& matrix, 
									 std::vector<dynet::Expression>& premise_lstm_repr, 
									 std::vector<dynet::Expression>& hypothesis_lstm_repr);
				
				
		void compute_beta(std::vector< std::vector<float> >& beta_matrix, 
						  std::vector< std::vector<float> >& matrix);
				
				
		void compute_alpha(std::vector< std::vector<float> >& alpha_matrix, 
						   std::vector< std::vector<float> >& matrix);
		
		
		void compute_context_vector(std::vector< std::vector<float> >& matrix, 
									std::vector<dynet::Expression>& sentence_repr, 
									unsigned sentence_size, 
									unsigned other_sentence_size, 
									std::vector<dynet::Expression>& context_vect);
		
		
		virtual std::vector<float> predict(DataSet& set, 
										   Embeddings& embedding, 
										   unsigned num_sentence, 
										   dynet::ComputationGraph& cg, 
										   bool print_proba, 
										   unsigned& argmax);
										   
										   
		virtual dynet::Expression get_neg_log_softmax(DataSet& set, 
													  Embeddings& embedding, 
													  unsigned num_sentence, 
													  dynet::ComputationGraph& cg);


	public:
		BiLSTM(unsigned nblayer, 
			   unsigned inputdim, 
			   unsigned hiddendim, 
			   float dropout, 
			   unsigned s, 
			   dynet::ParameterCollection& model);
};

#endif

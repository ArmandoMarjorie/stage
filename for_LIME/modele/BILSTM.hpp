#ifndef BILSTM_HPP
#define BILSTM_HPP

	#include "../code_data/dataset.hpp"
	#include "rnn.hpp"

	/**
	 * \file BILSTM.hpp
	*/
	
	/** 
	 * \class BiLSTM
	 * \brief Class representing a bidirectional LSTM
	*/
	class BiLSTM : public RNN
	{
		private:
			dynet::VanillaLSTMBuilder* backward_lstm; /*!< backward LSTM*/ 
			dynet::Parameter p_W_attention; /*!< attention matrix weight*/

			void words_representation(Embeddings& embedding, DataSet& set, unsigned sentence,
					dynet::ComputationGraph& cg, unsigned num_sentence, std::vector<dynet::Expression>& sentence_repr);
			dynet::Expression run_KIM(DataSet& set, Embeddings& embedding, unsigned num_sentence, 
					dynet::ComputationGraph& cg);
			//dynet::Expression run_sys4(DataSet& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, unsigned* important_couple);
			void create_attention_matrix(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& matrix, 
					std::vector<dynet::Expression>& premise_lstm_repr, std::vector<dynet::Expression>& hypothesis_lstm_repr);
			void compute_beta(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& beta_matrix, 
					std::vector< std::vector<float> >& matrix);
			void compute_alpha(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& alpha_matrix, 
					std::vector< std::vector<float> >& matrix);
			void compute_a_context_vector(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& alpha_matrix,
					std::vector<dynet::Expression>& hypothesis_lstm_repr, unsigned premise_size, std::vector<dynet::Expression>& a_c_vect);
			void compute_b_context_vector(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& beta_matrix,
					std::vector<dynet::Expression>& premise_lstm_repr, unsigned hypothesis_size, std::vector<dynet::Expression>& b_c_vect);
			virtual std::vector<float> predict(DataSet& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax, unsigned* important_couple);
			virtual dynet::Expression get_neg_log_softmax(DataSet& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);


		public:
			BiLSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, dynet::ParameterCollection& model);
	};

#endif

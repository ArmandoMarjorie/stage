#ifndef LSTM_HPP
#define LSTM_HPP

	#include "../code_data/data.hpp"
	#include "rnn.hpp"

	/**
	 * \file LSTM.hpp
	*/
	
	/** 
	 * \class LSTM
	 * \brief Class representing a LSTM
	*/	
	class LSTM : public RNN
	{
		private:
			dynet::Expression sentence_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, dynet::ComputationGraph& cg);
			void words_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, dynet::ComputationGraph& cg, std::vector<dynet::Expression>& sentence_repr);
			dynet::Expression run(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
			dynet::Expression run_sys5(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);
			dynet::Expression systeme_1(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);
			dynet::Expression systeme_2(std::vector<dynet::Expression>& h, dynet::Expression bias, dynet::Expression W);
			virtual std::vector<float> predict(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg, bool print_proba, unsigned& argmax, unsigned* important_couple);
			virtual dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, dynet::ComputationGraph& cg);

		public:
			LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, dynet::ParameterCollection& model);
	};

#endif

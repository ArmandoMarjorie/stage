#include "LSTM.hpp"
	
using namespace std;
using namespace dynet;


/**
 * \file LSTM.cpp
*/


	/* Constructor */
	
	
/**
	* \brief LSTM Constructor. Initializes a LSTM.
	*
	* \param nblayer : number of layer.
	* \param inputdim : inputs dimension (embeddings dimension).
	* \param hiddendim : hidden states dimension.
	* \param dropout : dropout.
	* \param s : numero of the system used (1 or 2).
	* \param model : the model.
*/	
LSTM::LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, 
	float dropout, unsigned s, ParameterCollection& model, 
	bool original_lime) :
	RNN(nblayer, inputdim, hiddendim, dropout, s, model, original_lime)
{
	if(systeme == 1)
		p_W = model.add_parameters({NB_CLASSES, 2*hidden_dim});
	else if(systeme == 2)
		p_W = model.add_parameters({NB_CLASSES, hidden_dim*hidden_dim});
}


	/* Predictions algorithms */


/**
	* \name predict
	* \brief Predicts the most probable label and gives the probability 
	* of each label for an instance. 
	* 
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param num_sentence : numero of the instance.
	* \param cg : computation graph.
	* \param print_proba : true if we want the probability of each 
	* 	labels printed on screen, else false.
	* \param argmax : the predicted label.
	* 
	* \return The probability of each label for the "num_sentence"th instance.
*/
vector<float> LSTM::predict(DataSet& set, Embeddings& embedding, 
	unsigned num_sentence, ComputationGraph& cg, bool print_proba, 
	unsigned& argmax)
{
	Expression x = run(set, embedding, num_sentence, cg);
	vector<float> probs = predict_algo(x, cg, print_proba, argmax); //in rnn.cpp
	return probs;
}


	/* Negative log softmax algorithms */


/**
	* \name get_neg_log_softmax
	* \brief Loss function (negative log likelihood).
	* 
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param num_sentence : numero of the instance.
	* \param cg : computation graph.
	* 
	* \return The negative log likelihood of the "num_sentence"th 
	* instance's label after taking the softmax. 
*/
Expression LSTM::get_neg_log_softmax(DataSet& set, Embeddings& embedding, 
	unsigned num_sentence, ComputationGraph& cg)
{
	Expression score = run(set, embedding, num_sentence, cg);
	Expression loss_expr = get_neg_log_softmax_algo(score, num_sentence, set); //in rnn.cpp
	return loss_expr;
}


	/* LSTM systems (1 and 2) */


/**
	* \name sentence_representation
	* \brief Gives a hidden representation of a sentence (the premise or 
	* the hypothesis) from an instance.
	* 
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param is_premise : true if the premise is processed, else false.
	* \param num_sentence : numero of the instance.
	* \param cg : computation graph.
	* 
	* \return A hidden representation of a sentence from the 
	* "num_sentence"th instance. 
*/
Expression LSTM::sentence_representation(DataSet& set, 
	Embeddings& embedding, bool is_premise, unsigned num_sentence, 
	ComputationGraph& cg)
{
	const unsigned nb_expr = set.get_nb_expr(is_premise, num_sentence);
	unsigned nb_words; //nb de mots dans l'expression courante

	if (apply_dropout) 
		forward_lstm->set_dropout(dropout_rate);
	else 
		forward_lstm->disable_dropout();

	forward_lstm->new_graph(cg);  // reset LSTM builder for new graph
	forward_lstm->start_new_sequence(); //to do before add_input() and after new_graph()
	Expression repr;
	unsigned wordID;

	for(unsigned i=0; i<nb_expr; ++i)
	{
		nb_words = set.get_nb_words(is_premise, num_sentence, i);
		for(unsigned j=0; j < nb_words; ++j)
		{
			if(!original_LIME && (wordID = set.get_word_id(is_premise, num_sentence, i, j) ) == 0) // 0 means "this is not a word, there is no word here !"
				continue;
			repr =  forward_lstm->add_input( embedding.get_embedding_expr(cg, wordID) );
		}
	}

	return repr;
}


/**
	* \name run
	* \brief Gives a vector of score for each label.
	* 
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param num_sentence : numero of the instance.
	* \param cg : computation graph.
	* 
	* \return A vector of score for each label for the "num_sentences"th 
	* instance. 
*/
Expression LSTM::run(DataSet& set, Embeddings& embedding, 
	unsigned num_sentence, ComputationGraph& cg) 
{
	vector<Expression> h(2);
	h[0] = sentence_representation(set, embedding, true, num_sentence, cg);
	h[1] = sentence_representation(set, embedding, false, num_sentence, cg);

	Expression W = parameter(cg, p_W); 
	Expression bias = parameter(cg, p_bias); 
	Expression score;

	if(systeme == 1)
		score = systeme_1(h, bias, W);

	else if(systeme == 2)
		score = systeme_2(h, bias, W);

	return score;
}


/**
	* \name systeme_1
	* \brief Gives a vector of score for each label, using the first system.
	* 
	* \param h : vector containing the hidden representation of the 
	* 	premise and the hypothesis. 
	* \param bias : the bias.
	* \param W : the weight matrix.
	* 
	* \return A vector of score for each label using the first system.
*/
Expression LSTM::systeme_1(vector<Expression>& h, Expression bias, Expression W) 
{
	Expression x = concatenate(h);
	Expression score = affine_transform({bias, W, x});

	return score;
}


/**
	* \name systeme_2
	* \brief Gives a vector of score for each label, using the second system.
	* 
	* \param h : vector containing the hidden representation of the 
	* 	premise and the hypothesis. 
	* \param bias : the bias.
	* \param W : the weight matrix.
	* 
	* \return A vector of score for each label using the second system.
*/
Expression LSTM::systeme_2(vector<Expression>& h, Expression bias, Expression W) 
{
	Expression x = h[0] * transpose(h[1]);
	//reshape x as a (H * H, 1) vector 
	Expression x_reshaped = reshape(x, Dim({hidden_dim*hidden_dim}, 1));
	Expression score = affine_transform({bias, W, x_reshaped});

	return score;
}

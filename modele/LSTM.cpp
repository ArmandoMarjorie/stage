#include "LSTM.hpp"
	
using namespace std;
using namespace dynet;


	/* Constructors */
	
LSTM::LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, ParameterCollection& model) :
	RNN(nblayer, inputdim, hiddendim, dropout, s, model)
{
	if(systeme == 1)
		p_W = model.add_parameters({NB_CLASSES, 2*hidden_dim});
	else if(systeme == 2)
		p_W = model.add_parameters({NB_CLASSES, hidden_dim*hidden_dim});
}

	/* Predictions algorithms */

vector<float> LSTM::predict(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg, bool print_proba, unsigned& argmax, unsigned* important_couple)
{
	//cerr << "LSTM prediction : system " << systeme << endl;
	Expression x = run(set, embedding, num_sentence, cg);
	vector<float> probs = predict_algo(x, cg, print_proba, argmax); //in rnn.cpp
	return probs;
}

	/* Negative log softmax algorithms */

Expression LSTM::get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	Expression score = run(set, embedding, num_sentence, cg);
	Expression loss_expr = get_neg_log_softmax_algo(score, num_sentence, set);
	return loss_expr;
}

	/* LSTM system */

Expression LSTM::sentence_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, 
        ComputationGraph& cg)
{
	const unsigned nb_words = set.get_nb_words(sentence, num_sentence);

	if (apply_dropout) 
		forward_lstm->set_dropout(dropout_rate);
	else 
		forward_lstm->disable_dropout();

	forward_lstm->new_graph(cg);  // reset LSTM builder for new graph
	forward_lstm->start_new_sequence(); //to do before add_input() and after new_graph()
	Expression repr;

	for(unsigned i=0; i<nb_words; ++i)
	{
		//if(set.get_word_id(sentence, num_sentence, i) == 0) // 0 means "this is not a word, there is no word here !"
		//	continue;
		repr =  forward_lstm->add_input( embedding.get_embedding_expr(cg, set.get_word_id(sentence, num_sentence, i)) );
	}

	//Expression repr = sum(vect_sentence);
	//cerr << "dim repr = "<< repr.dim() << endl;
	return repr;//vect_sentence[nb_words-1];
}

Expression LSTM::run(Data& set, Embeddings& embedding, unsigned num_sentence, 
        ComputationGraph& cg) 
{
	vector<Expression> h(2);
	h[0] = sentence_representation(set, embedding, 1, num_sentence, cg);
	h[1] = sentence_representation(set, embedding, 2, num_sentence, cg);

	Expression W = parameter(cg, p_W); 
	Expression bias = parameter(cg, p_bias); 
	Expression score;

	if(systeme == 1)
		score = systeme_1(h, bias, W);

	else if(systeme == 2)
		score = systeme_2(h, bias, W);

	return score;
}

Expression LSTM::systeme_1(vector<Expression>& h, Expression bias, Expression W) 
{
	Expression x = concatenate(h);
	Expression score = affine_transform({bias, W, x});

	return score;
}

Expression LSTM::systeme_2(vector<Expression>& h, Expression bias, Expression W) 
{
	Expression x = h[0] * transpose(h[1]);
	//reshape x as a (H * H, 1) vector 
	Expression x_reshaped = reshape(x, Dim({hidden_dim*hidden_dim}, 1));
	Expression score = affine_transform({bias, W, x_reshaped});

	return score;
}

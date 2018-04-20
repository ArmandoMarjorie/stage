#include <sys/types.h>
#include <unistd.h>
#include "rnn.hpp"

using namespace std;
using namespace dynet;

	/* Constructors */

RNN::RNN(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, ParameterCollection& model) :
	nb_layers(nblayer), input_dim(inputdim), hidden_dim(hiddendim), dropout_rate(dropout), systeme(s)
{
	forward_lstm = new VanillaLSTMBuilder(nb_layers, input_dim, hidden_dim, model); 
	apply_dropout = (dropout != 0);
	p_bias = model.add_parameters({NB_CLASSES});
}

LSTM::LSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, ParameterCollection& model) :
	RNN(nblayer, inputdim, hiddendim, dropout, s, model)
{
	if(systeme == 1)
		p_W = model.add_parameters({NB_CLASSES, 2*hidden_dim});
	else if(systeme == 2)
		p_W = model.add_parameters({NB_CLASSES, hidden_dim*hidden_dim});
}

BiLSTM::BiLSTM(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, ParameterCollection& model) :
	RNN(nblayer, inputdim, hiddendim, dropout, s, model)
{
	backward_lstm = new VanillaLSTMBuilder(nb_layers, input_dim, hidden_dim, model);
	if(systeme==3)
		p_W = model.add_parameters({NB_CLASSES, 4*hidden_dim});
	else if(systeme==4)
		p_W = model.add_parameters({NB_CLASSES, (2*hidden_dim)*(2*hidden_dim)});
}

	/* Getters and setters */

void RNN::disable_dropout()
{
	apply_dropout = false;
}

void RNN::enable_dropout()
{
	apply_dropout = true;
}


float RNN::get_dropout_rate(){ return dropout_rate; }
unsigned RNN::get_nb_layers(){ return nb_layers; }
unsigned RNN::get_input_dim(){ return input_dim; }
unsigned RNN::get_hidden_dim(){ return hidden_dim; }

	/* Predictions algorithms */

unsigned LSTM::predict(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg, bool print_proba)
{
	//cerr << "LSTM prediction : system " << systeme << endl;
	Expression x = run(set, embedding, num_sentence, cg);
	unsigned argmax = predict_algo(x, cg, print_proba);
	return argmax;
}

unsigned BiLSTM::predict(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg, bool print_proba)
{
	//cerr << "BiLSTM prediction \n";
	Expression x;
	if(systeme==3)
		x = run_KIM(set, embedding, num_sentence, cg);
	else if(systeme==4)
		x = run_sys4(set, embedding, num_sentence, cg);
		
	unsigned argmax = predict_algo(x, cg, print_proba);
	return argmax;
}

unsigned RNN::predict_algo(Expression& x, ComputationGraph& cg, bool print_proba)
{
	vector<float> probs = as_vector(cg.forward(x));
	unsigned argmax=0;

	for (unsigned k = 0; k < probs.size(); ++k) 
	{
		if(print_proba)
			cerr << "proba[" << k << "] = " << probs[k] << endl;
		if (probs[k] > probs[argmax])
			argmax = k;
	}
	return argmax;
}

unsigned predict_dev_and_test(RNN& rnn, Data& dev_set, Embeddings& embedding, unsigned nb_of_sentences_dev, unsigned& best)
{
	unsigned positive = 0;
	unsigned positive_inf = 0;
	unsigned positive_neutral = 0;
	unsigned positive_contradiction = 0;
	unsigned label_predicted;
	
	const double nb_of_inf = static_cast<double>(dev_set.get_nb_inf());
	const double nb_of_contradiction = static_cast<double>(dev_set.get_nb_contradiction());
	const double nb_of_neutral = static_cast<double>(dev_set.get_nb_neutral());

	for (unsigned i=0; i<nb_of_sentences_dev; ++i)
	{
		ComputationGraph cg;
		label_predicted = rnn.predict(dev_set, embedding, i, cg, false);
		if (label_predicted == dev_set.get_label(i))
		{
			positive++;
			if(dev_set.get_label(i) == NEUTRAL)
				++positive_neutral;
			else if(dev_set.get_label(i) == INFERENCE)
				++positive_inf;
			else
				++positive_contradiction;
		}
	}

	cerr << "Accuracy in general = " << positive / (double) nb_of_sentences_dev << endl;
	
	cerr << "\tContradiction Accuracy = " << positive_contradiction / nb_of_contradiction << endl;
	cerr << "\tInference Accuracy = " << positive_inf / nb_of_inf << endl;
	cerr << "\tNeutral Accuracy = " << positive_neutral / nb_of_neutral << endl;	
	
	return positive;
}

		/* Predictions algorithms (handlers) */

void run_predict(RNN& rnn, ParameterCollection& model, Data& test_set, Embeddings& embedding, char* parameters_filename)
{
	// Load preexisting weights
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	unsigned nb_of_sentences = test_set.get_nb_sentences();
	unsigned best = 0;
	rnn.disable_dropout();
	predict_dev_and_test(rnn, test_set, embedding, nb_of_sentences, best);
}

/* EN CONSTRUCTION ..................... */
/*
void LSTM::run_predict_explication(dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	unsigned label_predicted;
	const unsigned nb_of_sentences = explication_set.get_nb_sentences();
	disable_dropout();
	vector<unsigned> num_couple(1);
	for(unsigned i=0; i<nb_of_sentences; ++i) //pour un sample ...
	{
		ComputationGraph cg;
		label_predicted = predict(explication_set, embedding, i, cg, true);
		cerr << "True label = " << explication_set.get_label(i) << ", label predicted = " << label_predicted << endl;
		
		// changement ici 
		
		
		// parcours de tous les couples (supprimer 1 par 1, voir lesquels sont les meilleurs)
		for(unsigned j=0; j < explication_set.get_nb_couple(i); ++j) // parcours de tous les couples
		{
			num_couple[0] = j;
			explication_set.remove_couple(num_couple, i);
			// prédiction avec le nouveau explication_set
			// il faut récupérer les probas, et regarder les 2 meilleurs couples 
			
			// Notion de "bon" couple 
			// * une mesure de bon couple (DBC = degré de bon couple)
			// * si un couple a un bon DBC, alors le modèle s'est beaucoup servi de ce couple pour sa prédiction 
			// * Est ce que c'est plus important qu'un couple supprimé fasse augmenter le label de base et beaucoup augmenter les autres,
			// * ou qu'un couple supprimé fasse baisser le label de base et augmenter les autres ? 
			 
			explication_set.reset_couple(num_couple, i);
			
		}
		// Enlever les 2 meilleurs couples, prédire, conclure 
		
		// -------------- 
	}
	
}*/
/* .......................................... */

void run_predict_couple(RNN& rnn, dynet::ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	unsigned label_predicted;
	const unsigned nb_of_sentences = explication_set.get_nb_sentences();
	rnn.disable_dropout();
	char const* name = "Files/expl_token";
	ofstream output(name, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << name << endl;
		exit(EXIT_FAILURE);
	}	
	
	vector<unsigned> premise;
	vector<unsigned> hypothesis;
	
	
	for(unsigned i=0; i<nb_of_sentences; ++i) //pour un sample ...
	{
		ComputationGraph cg;
		label_predicted = rnn.predict(explication_set, embedding, i, cg, false);
		output << explication_set.get_label(i) << endl << label_predicted << endl;
		save_sentences(explication_set, premise, hypothesis, i);
		write_sentences(output, premise, hypothesis);
		for(unsigned j=0; j < explication_set.get_nb_couple(i); ++j) // parcours de tous les couples
		{
			explication_set.taking_couple(j,i);
			label_predicted = rnn.predict(explication_set, embedding, i, cg, false);
			write_couple(output, explication_set, i, j);
			output << "-1 " << label_predicted << " " << explication_set.get_couple_label(i, j) << endl;
			explication_set.reset_sentences(premise, hypothesis, i, true);
			explication_set.reset_sentences(premise, hypothesis, i, false);
		}
		premise.clear();
		hypothesis.clear();
		output << " -3\n";
	}
	
	output.close();
}

void write_couple(ofstream& output, Data& explication_set, unsigned num_sample, unsigned num_couple)
{	
	for(unsigned i=0; i<explication_set.get_couple_nb_words(num_sample,num_couple, true); ++i)
	{
		output << explication_set.get_couple_id(num_sample, num_couple, i, true) << " ";
	}
	output << "-2 ";
	for(unsigned i=0; i<explication_set.get_couple_nb_words(num_sample,num_couple, false); ++i)
	{
		output << explication_set.get_couple_id(num_sample, num_couple, i, false) << " ";
	}	
}

void write_sentences(ofstream& output, vector<unsigned>& premise, vector<unsigned>& hypothesis)
{
	unsigned i;
	for(i=0; i<premise.size(); ++i)
		output << premise[i] << " ";
	output << "-1\n";
	for(i=0; i<hypothesis.size(); ++i)
		output << hypothesis[i] << " ";	
	output << "-1\n";
}

void save_sentences(Data& explication_set,vector<unsigned>& premise,vector<unsigned>& hypothesis, unsigned num_sample)
{
	for(unsigned sentence=1; sentence <= 2; ++sentence)
	{
		for(unsigned i =0; i<explication_set.get_nb_words(sentence, num_sample); ++i)
		{
			if(sentence==1)
				premise.push_back(explication_set.get_word_id(sentence, num_sample, i));
			else
				hypothesis.push_back(explication_set.get_word_id(sentence, num_sample, i));
		}	
	}
}


/*
void LSTM::usage_predict_verbose()
{
	cerr << "Enter id label\n";
	cerr << "Enter id word to form a sentence and -1 to end your sentence\n";
	cerr << "The first sentence is the premise, the second is the hypothesis\n";
	cerr << "Enter -2 to end the program\n";
	
}


void LSTM::run_predict_verbose(ParameterCollection& model, Data& verbose_set, Embeddings& embedding, char* parameters_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n\n";

	cerr << "\t** Testing : Verbose Mode **\n";
//	usage_predict_verbose();
	unsigned label_predicted;
	
	disable_dropout();
	ComputationGraph cg;
	label_predicted = predict(verbose_set, embedding, 0, cg, true);
	
	cerr << "True label = " << verbose_set.get_label(0) << ", label predicted = " << label_predicted << endl;

}*/


	/* Negative log softmax algorithms */

Expression get_neg_log_softmax(RNN& rnn, Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	Expression loss_expr = rnn.get_neg_log_softmax(set, embedding, num_sentence, cg);
	return loss_expr;
}

Expression LSTM::get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	Expression score = run(set, embedding, num_sentence, cg);
	Expression loss_expr = get_neg_log_softmax_algo(score, num_sentence, set);
	return loss_expr;
}

Expression BiLSTM::get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	Expression score;
	if(systeme==3)
		score = run_KIM(set, embedding, num_sentence, cg);
	else if(systeme==4)
		score = run_sys4(set, embedding, num_sentence, cg);
	Expression loss_expr = get_neg_log_softmax_algo(score, num_sentence, set);
	return loss_expr;
}

Expression RNN::get_neg_log_softmax_algo(Expression& score, unsigned num_sentence, Data& set)
{
	const unsigned label = set.get_label(num_sentence);
	Expression loss_expr = pickneglogsoftmax(score, label);

	return loss_expr;
}

	/* Training algorithm */

void run_train(RNN& rnn, ParameterCollection& model, Data& train_set, Data& dev_set, Embeddings& embedding, char* output_emb_filename, unsigned nb_epoch, unsigned batch_size)
{
	Trainer* trainer = new AdamTrainer(model);

	// Model output file
	ostringstream os;
	os << "rnn"
	   << "_D-" << rnn.get_dropout_rate()
	   << "_L-" << rnn.get_nb_layers()
	   << "_I-" << rnn.get_input_dim()
	   << "_H-" << rnn.get_hidden_dim()
	   << "_pid-" << getpid() << ".params";
	string parameter_filename = os.str();
	cerr << "Parameters will be written to: " << parameter_filename << endl;

	unsigned best = 0; 
	unsigned si;
	unsigned nb_samples;
	unsigned nb_of_sentences = train_set.get_nb_sentences();
	unsigned nb_of_sentences_dev = dev_set.get_nb_sentences();

	// Number of batches in training set
	unsigned nb_batches = nb_of_sentences / batch_size; 
	cerr << "nb of examples = "<<nb_of_sentences<<endl;
	cerr << "batches size = "<<batch_size<<endl;
	cerr << "nb of batches = "<<nb_batches<<endl;

	vector<unsigned> order(nb_of_sentences);
	for (unsigned i = 0; i < nb_of_sentences; ++i) 
		order[i] = i;
	unsigned numero_sentence;
	
	for(unsigned completed_epoch=0; completed_epoch < nb_epoch; ++completed_epoch) 
	{
		// Reshuffle the dataset
		cerr << "\n**SHUFFLE\n";
		random_shuffle(order.begin(), order.end());
		Timer iteration("completed in");

		nb_samples=0;
		numero_sentence=0;
		rnn.enable_dropout();
		train_score(rnn, train_set, embedding, nb_batches, nb_samples, batch_size, completed_epoch, nb_of_sentences, trainer, order, numero_sentence);
		rnn.disable_dropout();
		dev_score(rnn, model, dev_set, embedding, parameter_filename, nb_of_sentences_dev, best, output_emb_filename);
	}
}


void train_score(RNN& rnn, Data& train_set, Embeddings& embedding, unsigned nb_batches, 
        unsigned& nb_samples, unsigned batch_size, unsigned completed_epoch, unsigned nb_of_sentences, Trainer* trainer, vector<unsigned>& order, unsigned& numero_sentence)
{
	double loss = 0;

	for(unsigned batch = 0; batch < nb_batches; ++batch)
	{
		ComputationGraph cg; // we create a new computation graph for the epoch, not each item.
		vector<Expression> losses;

		// we will treat all those sentences as a single batch
		for (unsigned si = 0; si < batch_size; ++si) 
		{
			Expression loss_expr = get_neg_log_softmax(rnn, train_set, embedding, order[numero_sentence], cg);
			losses.push_back(loss_expr);
			++numero_sentence;
		}
		// Increment number of samples processed
		nb_samples += batch_size;

		/*  we accumulated the losses from all the batch.
			Now we sum them, and do forward-backward as usual.
			Things will run with efficient batch operations.  */
		Expression sum_losses = (sum(losses)) / (double)batch_size; //averaging the losses
		loss += as_scalar(cg.forward(sum_losses));
		cg.backward(sum_losses); //backpropagation gradient
		trainer->update(); //update parameters

		if(/*(batch + 1) % (nb_batches / 160) == 0 || */batch==nb_batches-1)
		{
			trainer->status();
			cerr << " Epoch " << completed_epoch+1 << " | E = " << (loss / static_cast<double>(nb_of_sentences))
				 << " | samples processed = "<<nb_samples<<endl;
		}
	}
}

void dev_score(RNN& rnn, ParameterCollection& model, Data& dev_set, Embeddings& embedding, string parameter_filename, unsigned nb_of_sentences_dev, unsigned& best, char* output_emb_filename)
{
	unsigned positive = predict_dev_and_test(rnn, dev_set, embedding, nb_of_sentences_dev, best);
	if (positive > best) //save the model if it's better than before
	{
		cerr << "it's better !\n";
		best = positive;
		TextFileSaver saver(parameter_filename);
		saver.save(model);
		if(output_emb_filename != NULL)
			embedding.print_embedding(output_emb_filename);
	}

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
		if(set.get_word_id(sentence, num_sentence, i) == 0) // 0 means "this is not a word, there is no word here !"
			continue;
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

	/* Bi-LSTM system */

/* Call this fonction twice (the first time for the premise, the second time for the hypothesis) 
 * sentence_repr[i] = representation of the word nb i by the KIM method
 */
/**
        * \brief Give a representation for each word of the given sentence
        * \details Use a Bi-LSTM to run a forward and backward LSTM on the sentence. 
        * The hidden states generated by the LSTMs at each time step are concatenated.
        * 
        * \param embedding : the words embedding
        * \param set : the dataset 
        * \param sentence : The sentence you want to represent (1 if you want the premise, 2 if you want the hypothesis)
        * \param cg : the computation graph
        * \param num_sentence : the number of the sample processed
        * \param sentence_repr : matrix of size (number of words in the sentence, hidden dimension). 
        * sentence_repr[i] = representation of the i_th word
*/
void BiLSTM::words_representation(Embeddings& embedding, Data& set, unsigned sentence,
	ComputationGraph& cg, unsigned num_sentence, vector<Expression>& sentence_repr)
{
	const unsigned nb_words = set.get_nb_words(sentence, num_sentence);
	if (apply_dropout)
	{ 
		forward_lstm->set_dropout(dropout_rate);
		backward_lstm->set_dropout(dropout_rate);
	}
	else 
	{
		forward_lstm->disable_dropout();
		backward_lstm->disable_dropout();
	}
	forward_lstm->new_graph(cg);  // reset LSTM builder for new graph
	forward_lstm->start_new_sequence(); //to do before add_input() and after new_graph()
	backward_lstm->new_graph(cg);  // reset LSTM builder for new graph
	backward_lstm->start_new_sequence(); //to do before add_input() and after new_graph()

	vector<Expression> tmp;
	unsigned i;
	int j;

	/* Run forward LSTM */
	for(i=0; i<nb_words; ++i)
	{
		if(set.get_word_id(sentence, num_sentence, i) == 0)
			continue;
		sentence_repr.push_back(forward_lstm->add_input( embedding.get_embedding_expr(cg, set.get_word_id(sentence, num_sentence, i)) ) );
	}
	/* Run backward LSTM */
	for(j=nb_words-1; j>=0; --j)
	{
		if(set.get_word_id(sentence, num_sentence, static_cast<unsigned>(j)) == 0)
			continue;
		tmp.push_back(backward_lstm->add_input( 
				embedding.get_embedding_expr(cg, set.get_word_id(sentence, num_sentence, static_cast<unsigned>(j))) ) );
	}
	/* Concat */
	for(i=0; i<sentence_repr.size(); ++i)
	{
		vector<Expression> input_expr(2);
		input_expr[0] = sentence_repr[i];
		input_expr[1] = tmp[i];
		sentence_repr[i] = concatenate(input_expr);
	}

}

void BiLSTM::compute_alpha(ComputationGraph& cg, vector< vector<float> >& alpha_matrix, vector< vector<float> >& matrix)
{
	const unsigned premise_size = matrix.size();
	const unsigned hypothesis_size = matrix[0].size();
	float result=0;
	for(unsigned i=0; i<premise_size; ++i)
	{
		for(unsigned j=0; j<hypothesis_size; ++j)
		{
			result = 0;
			for(unsigned k=0; k<hypothesis_size; ++k)
					result += exp( matrix[i][k] );
			alpha_matrix[i][j] = exp( matrix[i][j] ) / result;
		}
	}
	/*Expression e = input(cg, Dim({premise_size, hypothesis_size}), alpha_matrix);
	return e;*/
}

void BiLSTM::compute_beta(ComputationGraph& cg, vector< vector<float> >& beta_matrix, vector< vector<float> >& matrix)
{
	const unsigned premise_size = matrix.size();
	const unsigned hypothesis_size = matrix[0].size();
	float result=0;
	for(unsigned i=0; i<premise_size; ++i)
	{
		for(unsigned j=0; j<hypothesis_size; ++j)
		{
			result = 0;
			for(unsigned k=0; k<premise_size; ++k)
				result += exp( matrix[k][j] );
			beta_matrix[i][j] = exp( matrix[i][j] ) / result;

			//cout << beta_matrix[i][j] << ' ';
		}
		//cout << endl;
	}
	/*Expression e = input(cg, {premise_size, hypothesis_size}, beta_matrix);
	return e;*/
}

void BiLSTM::create_attention_matrix(ComputationGraph& cg, vector< vector<float> >& matrix, 
	vector<Expression>& premise_lstm_repr, vector<Expression>& hypothesis_lstm_repr)
{
	const unsigned premise_size = premise_lstm_repr.size();
	const unsigned hypothesis_size = hypothesis_lstm_repr.size();
	for(unsigned i=0; i<premise_size; ++i)
	{
		for(unsigned j=0; j<hypothesis_size; ++j)
		{
			Expression e = transpose(premise_lstm_repr[i]) * hypothesis_lstm_repr[j];
			matrix[i][j] = as_scalar( e.value() );

			//cout << matrix[i][j] << ' ';
		}
		//cout << endl;
	}
}

void BiLSTM::compute_a_context_vector(ComputationGraph& cg, vector< vector<float> >& alpha_matrix, 
	vector<Expression>& hypothesis_lstm_repr, unsigned premise_size, vector<Expression>& a_c_vect)
{
	const unsigned hypothesis_size = hypothesis_lstm_repr.size();

	for(unsigned i=0; i<premise_size; ++i)
	{
		vector<Expression> vect;
		for(unsigned j=0; j<hypothesis_size; ++j)
		{
			Expression e = hypothesis_lstm_repr[j] * alpha_matrix[i][j];
			vect.push_back(e); 
		}
		a_c_vect[i] = sum(vect);
	}
}

void BiLSTM::compute_b_context_vector(ComputationGraph& cg, vector< vector<float> >& beta_matrix, vector<Expression>& premise_lstm_repr, 
	unsigned hypothesis_size, vector<Expression>& b_c_vect)
{
	const unsigned premise_size = premise_lstm_repr.size();

	for(unsigned j=0; j<hypothesis_size; ++j)
	{
		vector<Expression> vect;
		for(unsigned i=0; i<premise_size; ++i)
		{
			Expression e = premise_lstm_repr[i] * beta_matrix[i][j];
			vect.push_back(e); 
		}
		b_c_vect[j] = sum(vect);
	}
}

Expression BiLSTM::run_sys4(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	/* Representation of each word (of the premise and of the hypothesis)
	 * by the BiLSTM
	 */
	vector<Expression> premise_lstm_repr;
	vector<Expression> hypothesis_lstm_repr;
	words_representation(embedding, set, 1, cg, num_sentence, premise_lstm_repr);
	words_representation(embedding, set, 2, cg, num_sentence, hypothesis_lstm_repr);
	
	// Computing score 
	Expression W = parameter(cg, p_W); 
	Expression bias = parameter(cg, p_bias); 
	vector<Expression> scores;
	Expression input;
	
	for(unsigned i=0; i<premise_lstm_repr.size(); ++i)
		for(unsigned j=0; j<hypothesis_lstm_repr.size(); ++j)
		{
			input = premise_lstm_repr[i] * transpose(hypothesis_lstm_repr[j]);
			input = reshape(input, Dim({(2*hidden_dim)*(2*hidden_dim)}, 1));
			//cerr << "dimension = "<<input.dim() <<endl;
			scores.push_back(affine_transform({bias, W, input}));
		}
		
	return sum(scores);
}

Expression BiLSTM::run_KIM(Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg) 
{
	/* Representation of each word (of the premise and of the hypothesis)
	 * by the BiLSTM explained in the step 1 of KIM.
	 */
	vector<Expression> premise_lstm_repr;
	vector<Expression> hypothesis_lstm_repr;
	words_representation(embedding, set, 1, cg, num_sentence, premise_lstm_repr);
	words_representation(embedding, set, 2, cg, num_sentence, hypothesis_lstm_repr);

	/* Creating attention matrix */
	vector< vector<float> > attention_matrix(premise_lstm_repr.size(), vector<float>(hypothesis_lstm_repr.size()) );
	create_attention_matrix(cg, attention_matrix, premise_lstm_repr, hypothesis_lstm_repr); 
	//cerr<< "Dim of attention matrix = ("<<attention_matrix.size()<<", "<< attention_matrix[0].size()<<")"<<endl;

	// Computing alpha and beta 
	vector< vector<float> > alpha_matrix(premise_lstm_repr.size(), vector<float>(hypothesis_lstm_repr.size()) );
	vector< vector<float> > beta_matrix(premise_lstm_repr.size(), vector<float>(hypothesis_lstm_repr.size()) );
	compute_alpha(cg, alpha_matrix, attention_matrix);
	compute_beta(cg, beta_matrix, attention_matrix); //softmax(e_ij);
	/*cerr<< "Dim of alpha matrix = ("<<alpha_matrix.size()<<", "<< alpha_matrix[0].size()<<")"<<endl;
	cerr<< "Dim of beta matrix = ("<<beta_matrix.size()<<", "<< beta_matrix[0].size()<<")"<<endl;*/

	// Computing context-vector 
	vector<Expression> a_c_vect(premise_lstm_repr.size());
	vector<Expression> b_c_vect(hypothesis_lstm_repr.size());
	compute_a_context_vector(cg, alpha_matrix, hypothesis_lstm_repr, premise_lstm_repr.size(), a_c_vect);
	compute_b_context_vector(cg, beta_matrix, premise_lstm_repr, hypothesis_lstm_repr.size(), b_c_vect);
	/*cerr<< "Dim of a context vector = "<<a_c_vect.size()<<endl;
	cerr<< "Dim of b context vector = "<<b_c_vect.size()<<endl;*/

	// Pooling 
	vector<Expression> pool(2);
	pool[0] = sum(a_c_vect) / static_cast<double>(premise_lstm_repr.size());
	pool[1] = sum(b_c_vect) / static_cast<double>(hypothesis_lstm_repr.size());

	// Concat pooling 
	Expression concat = concatenate(pool);
	//cerr<< "Dim of the final vector = "<<concat.dim()<<endl;

	// Computing score 
	Expression W = parameter(cg, p_W); 
	Expression bias = parameter(cg, p_bias); 
	Expression score = affine_transform({bias, W, concat});

	return score;
}

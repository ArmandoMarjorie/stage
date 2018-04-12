#include <sys/types.h>
#include <unistd.h>
#include "lstm.hpp"

using namespace std;
using namespace dynet;

/**
 * \file lstm.cpp
*/



void LSTM::disable_dropout()
{
	apply_dropout = false;
}

void LSTM::enable_dropout()
{
	apply_dropout = true;
}

/** Code avec les systèmes s1 et s2 */

LSTM::LSTM(unsigned nblayer, unsigned inputdim,
	unsigned hiddendim, float dropout, unsigned s, ParameterCollection& model) :
	nb_layers(nblayer), input_dim(inputdim), hidden_dim(hiddendim),
	dropout_rate(dropout), systeme(s)
{
	builder = new VanillaLSTMBuilder(nb_layers, input_dim, hidden_dim, model); 

	if(systeme == 1)
		p_W = model.add_parameters({NB_CLASSES, 2*hidden_dim});
	else if(systeme == 2)
		p_W = model.add_parameters({NB_CLASSES, hidden_dim*hidden_dim});
	p_bias = model.add_parameters({NB_CLASSES});

}

/*voir autre représentation */
Expression LSTM::sentence_representation(Data& set, Embeddings& embedding, unsigned sentence, unsigned num_sentence, 
        ComputationGraph& cg)
{
	const unsigned nb_words = set.get_nb_words(sentence, num_sentence);

	if (apply_dropout) 
		builder->set_dropout(dropout_rate);
	else 
		builder->disable_dropout();

	builder->new_graph(cg);  // reset LSTM builder for new graph
	builder->start_new_sequence(); //to do before add_input() and after new_graph()
	Expression repr;

	for(unsigned i=0; i<nb_words; ++i)
	{
		repr =  builder->add_input( embedding.get_embedding_expr(cg, set.get_word_id(sentence, num_sentence, i)) );
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

Expression LSTM::get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, 
        ComputationGraph& cg)
{
	const unsigned label = set.get_label(num_sentence);
	Expression score = run(set, embedding, num_sentence, cg);
	Expression loss_expr = pickneglogsoftmax(score, label);

	return loss_expr;
}

unsigned LSTM::predict(Data& set, Embeddings& embedding, unsigned num_sentence, 
        ComputationGraph& cg, bool print_proba)
{
	Expression x = run(set, embedding, num_sentence, cg);
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

void LSTM::run_predict(ParameterCollection& model, Data& test_set, Embeddings& embedding, char* parameters_filename)
{
	// Output file containing the dev file's predictions ---------------------------------------------
	/*fstream predicted_file("predictions.txt", ios::in | ios::out | ios::trunc);
	if(!predicted_file)
	{
		cerr << "Problem with the prediction file \n";
		exit(EXIT_FAILURE);
	}*/

	// Load preexisting weights
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	
	unsigned positive = 0;
	unsigned positive_inf = 0;
	unsigned positive_neutral = 0;
	unsigned positive_contradiction = 0;
	unsigned label_predicted;
	const double nb_of_inf = static_cast<double>(test_set.get_nb_inf());
	const double nb_of_contradiction = static_cast<double>(test_set.get_nb_contradiction());
	const double nb_of_neutral = static_cast<double>(test_set.get_nb_neutral());
	unsigned nb_of_sentences = test_set.get_nb_sentences();
	disable_dropout();
	for (unsigned i=0; i<nb_of_sentences; ++i)
	{
		ComputationGraph cg;
		label_predicted = predict(test_set, embedding, i, cg, false);
		if (label_predicted == test_set.get_label(i))
		{
			positive++;
			if(test_set.get_label(i) == NEUTRAL)
				++positive_neutral;
			else if(test_set.get_label(i) == INFERENCE)
				++positive_inf;
			else
				++positive_contradiction;
		}
	}

	// Print informations
	cerr << "Accuracy in general = " << positive / (double) nb_of_sentences << endl;
	
	cerr << "\tContradiction Accuracy = " << positive_contradiction / nb_of_contradiction << endl;
	cerr << "\tInference Accuracy = " << positive_inf / nb_of_inf << endl;
	cerr << "\tNeutral Accuracy = " << positive_neutral / nb_of_neutral << endl;
	//predicted_file.close();
}

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

}
/**
void LSTM::run_predict_interpretation(ParameterCollection& model, Data& test_interprete_set, Embeddings& embedding, char* parameters_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n\n";

	cerr << "\t** Testing : Interpretation Mode **\n";
//	usage_predict_verbose();
	unsigned label_predicted;
	
	disable_dropout();
	ComputationGraph cg;
	label_predicted = predict(verbose_set, embedding, 0, cg, true);
	
	cerr << "True label = " << verbose_set.get_label(0) << ", label predicted = " << label_predicted << endl;

}
*/
void LSTM::run_train(ParameterCollection& model, Data& train_set, Data& dev_set, Embeddings& embedding, char* output_emb_filename, unsigned nb_epoch, unsigned batch_size)
{
	Trainer* trainer = new AdamTrainer(model);

	// Model output file
	ostringstream os;
	os << "rnn"
	   << "_D-" << dropout_rate
	   << "_L-" << nb_layers
	   << "_I-" << input_dim
	   << "_H-" << hidden_dim
	   << "_pid-" << getpid() << ".params";
	string parameter_filename = os.str();
	cerr << "Parameters will be written to: " << parameter_filename << endl;

	//double best = 9e+99; //avec dev loss
	unsigned best = 0; //avec dpos
	unsigned si;
	unsigned nb_samples;
	unsigned nb_of_sentences = train_set.get_nb_sentences();
	unsigned nb_of_sentences_dev = dev_set.get_nb_sentences();

	// Number of batches in training set
	unsigned int nb_batches = nb_of_sentences / batch_size; 
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
		enable_dropout();
		train_score(train_set, embedding, nb_batches, nb_samples, batch_size, completed_epoch, nb_of_sentences, trainer, order, numero_sentence);
		disable_dropout();
		dev_score(model, dev_set, embedding, parameter_filename, nb_of_sentences_dev, best, output_emb_filename);

	}

}

void LSTM::train_score(Data& train_set, Embeddings& embedding, unsigned nb_batches, unsigned& nb_samples, unsigned batch_size, unsigned completed_epoch,
    unsigned nb_of_sentences, Trainer* trainer, vector<unsigned>& order, unsigned& numero_sentence)
{
	double loss = 0;

	for(unsigned batch = 0; batch < nb_batches; ++batch)
	{
		ComputationGraph cg; // we create a new computation graph for the epoch, not each item.
		vector<Expression> losses;

		// we will treat all those sentences as a single batch
		for (unsigned si = 0; si < batch_size; ++si) 
		{
			Expression loss_expr = get_neg_log_softmax(train_set, embedding, order[numero_sentence], cg);
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

		if(/*(batch + 1) % (nb_batches / 160) == 0 ||*/ batch==nb_batches-1)
		{
			trainer->status();
			cerr << " Epoch " << completed_epoch+1 << " | E = " << (loss / static_cast<double>(nb_of_sentences))
				 << " | samples processed = "<<nb_samples<<endl;
		}
	}
}

void LSTM::dev_score(ParameterCollection& model, Data& dev_set, Embeddings& embedding, string parameter_filename, unsigned nb_of_sentences_dev, unsigned& best, char* output_emb_filename)
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
		label_predicted = predict(dev_set, embedding, i, cg, false);
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
	if (positive > best) //save the model if it's better than before
	{
		cerr << "it's better !\n";
		best = positive;
		TextFileSaver saver(parameter_filename);
		saver.save(model);
		if(output_emb_filename != NULL)
			embedding.print_embedding(output_emb_filename);
	}
	cerr << "Accuracy in general = " << positive / (double) nb_of_sentences_dev << endl;
	
	cerr << "\tContradiction Accuracy = " << positive_contradiction / nb_of_contradiction << endl;
	cerr << "\tInference Accuracy = " << positive_inf / nb_of_inf << endl;
	cerr << "\tNeutral Accuracy = " << positive_neutral / nb_of_neutral << endl;

}

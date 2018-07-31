#include <sys/types.h>
#include <unistd.h>
#include "rnn.hpp"

using namespace std;
using namespace dynet;


/**
 * \file BILSTM.cpp
*/


	/* Constructor */
	
	
/**
	* \brief RNN Constructor. Initializes a RNN.
	*
	* \param nblayer : number of layer.
	* \param inputdim : inputs dimension (embeddings dimension).
	* \param hiddendim : hidden states dimension.
	* \param dropout : dropout.
	* \param s : numero of the system used (1, 2 or 3).
	* \param model : the model.
*/	
RNN::RNN(unsigned nblayer, unsigned inputdim, unsigned hiddendim, float dropout, unsigned s, ParameterCollection& model) :
	nb_layers(nblayer), input_dim(inputdim), hidden_dim(hiddendim), dropout_rate(dropout), systeme(s)
{
	forward_lstm = new VanillaLSTMBuilder(nb_layers, input_dim, hidden_dim, model); 
	apply_dropout = (dropout != 0);
	p_bias = model.add_parameters({NB_CLASSES});
}


	/* Getters and others functions */


/**
	* \name disable_dropout
	* \brief Disables the dropout.
*/	
void RNN::disable_dropout()
{
	apply_dropout = false;
}


/**
	* \name enable_dropout
	* \brief Enables the dropout.
*/
void RNN::enable_dropout()
{
	apply_dropout = true;
}


/**
	* \name get_dropout_rate
	* \brief Gives the dropout rate.
	* 
	* \return The dropout rate.
*/
float RNN::get_dropout_rate(){ return dropout_rate; }


/**
	* \name get_nb_layers
	* \brief Gives the number of layer.
	* 
	* \return The number of layer.
*/
unsigned RNN::get_nb_layers(){ return nb_layers; }


/**
	* \name get_input_dim
	* \brief Gives the input dimension.
	* 
	* \return The input dimension.
*/
unsigned RNN::get_input_dim(){ return input_dim; }


/**
	* \name get_hidden_dim
	* \brief Gives the hidden states dimension.
	* 
	* \return The hidden states dimension.
*/
unsigned RNN::get_hidden_dim(){ return hidden_dim; }


/**
	* \name softmax_vect
	* \brief Gives the hidden states dimension.
	* 
	* \param tmp : 
	* \param alpha : 
	* \param colonne : 
*/ /* PAS UTILISEE NORMALEMENT
void softmax_vect(vector<float>& tmp, vector<vector<float>>& alpha, unsigned& colonne)
{
	float x,y;
	unsigned j,k;
	vector<float> copy(tmp.size());
	for(j=0; j<tmp.size(); ++j)
	{
		x = exp(tmp[j]);
		y = 0;
		for(k=0; k<tmp.size(); ++k)
			y += exp(tmp[k]);
		copy[j] = x / y; 
	}
	
	for(j=0; j<copy.size() ; ++j)
		alpha[j][colonne] = copy[j];
	++colonne;

}
*/


/**
	* \name softmax_vect
	* \brief Does softmax on the given vector.
	* 
	* \param tmp : vector where to do softmax.
*/
void softmax_vect(vector<float>& tmp)
{
	float x,y;
	unsigned j,k;
	vector<float> copy(tmp.size());
	for(j=0; j<tmp.size(); ++j)
	{
		x = exp(tmp[j]);
		y = 0;
		for(k=0; k<tmp.size(); ++k)
			y += exp(tmp[k]);
		copy[j] = x / y; 
	}
	
	for(j=0; j<copy.size(); ++j)
		tmp[j] = copy[j];
	
}


/**
	* \name populate_from_file
	* \brief Load model parameters.
	* 
	* \param parameters_filename : file containing the parameters to load.
	* \param model : the model.
*/
void populate_from_file(char* parameters_filename, 
	ParameterCollection& model)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";
}


	/* Predictions algorithms */


/**
	* \name predict_algo
	* \brief Gives the probability of each label for the "num_sentence"th instance, 
	* and the predicted label.
	* 
	* \param x : vector of score.
	* \param cg : computation graph.
	* \param print_proba : true to print the probability of each label. 
	* on the screen, else false.
	* \param argmax : the predicted label (reference).
	* 
	* \return The probability of each label for the "num_sentence"th instance.
*/
vector<float> RNN::predict_algo(Expression& x, ComputationGraph& cg, 
	bool print_proba, unsigned& argmax)
{
	vector<float> probs = as_vector(cg.forward(x));
	
	softmax_vect(probs);
	argmax=0;

	for (unsigned k = 0; k < probs.size(); ++k) 
	{
		if(print_proba)
			cerr << "proba[" << k << "] = " << probs[k] << endl;
		if(probs[k] > probs[argmax])
			argmax = k;
	}
	
	return probs;
}


/**
	* \name predict_dev_and_test
	* \brief Predicts labels for every instances from the dev or test corpus.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param nb_of_instances : number of instances.
	* \param best : the best score of number of correct predicted labels. 
	* Useful for the dev corpus (for the epochs).
	* 
	* \return The number of correct predicted labels.
*/
unsigned predict_dev_and_test(RNN& rnn, DataSet& set, 
	Embeddings& embedding, unsigned nb_of_instances, unsigned& best)
{
	unsigned positive = 0;
	unsigned positive_inf = 0;
	unsigned positive_neutral = 0;
	unsigned positive_contradiction = 0;
	unsigned label_predicted;
	
	const double nb_of_inf = static_cast<double>(set.get_nb_inf());
	const double nb_of_contradiction = static_cast<double>(set.get_nb_contradiction());
	const double nb_of_neutral = static_cast<double>(set.get_nb_neutral());

	for (unsigned i=0; i<nb_of_instances; ++i)
	{
		ComputationGraph cg;
		rnn.predict(set, embedding, i, cg, false, label_predicted);
		cout << label_predicted << endl;
		if (label_predicted == set.get_label(i))
		{
			positive++;
			if(set.get_label(i) == NEUTRAL)
				++positive_neutral;
			else if(set.get_label(i) == INFERENCE)
				++positive_inf;
			else
				++positive_contradiction;
		}
	}

	cerr << "Accuracy in general = " << positive / (double) nb_of_instances << endl;
	
	cerr << "\tNeutral Accuracy = " << positive_neutral <<"/"<< nb_of_neutral << endl;	
	cerr << "\tEntailment Accuracy = " << positive_inf <<"/"<< nb_of_inf << endl;
	cerr << "\tContradiction Accuracy = " << positive_contradiction <<"/"<< nb_of_contradiction << endl;
	
	
	return positive;
}


		/* Predictions algorithms (handlers) */


/**
	* \name run_predict
	* \brief Main function to predict labels for every instances 
	* from the test corpus.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param model : the model. 
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param parameters_filename : preexisting weights.
*/
void run_predict(RNN& rnn, ParameterCollection& model, DataSet& set, 
	Embeddings& embedding, char* parameters_filename)
{
	// Load preexisting weights
	populate_from_file(parameters_filename, model);

	cerr << "Testing ...\n";
	unsigned nb_of_instances = set.get_nb_instances();
	unsigned best = 0;
	rnn.disable_dropout();
	predict_dev_and_test(rnn, set, embedding, nb_of_instances, best);
}


/**
	* \name run_predict_for_server_lime
	* \brief Main function to predict labels for every instances 
	* from the interp-SNLI corpus, using LIME.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param num_sample : numero of the instance.
	* 
	* \return The probability of each label for the "num_sample"th instance.
*/
vector<float> run_predict_for_server_lime(RNN& rnn, DataSet& set, 
	Embeddings& embedding, unsigned num_sample)
{
	unsigned label_predicted;
	rnn.disable_dropout();
	ComputationGraph cg;
	vector<float> probas = rnn.predict(set, embedding, num_sample, cg, false, label_predicted);
	return probas;
}


	/* Negative log softmax algorithms */


/**
	* \name get_neg_log_softmax_algo
	* \brief Loss function (negative log likelihood). Called in the LSTM 
	* or the BILSTM's get_neg_log_softmax function.
	* 
	* \param score : score of each label, using a LSTM or a BILSTM 
	* according to the system chosed.
	* \param num_sentence : numero of the instance.
	* \param set : the data set. 
	* 
	* \return The negative log likelihood of the "num_sentence"th 
	* instance's label after taking the softmax. 
*/
Expression RNN::get_neg_log_softmax_algo(Expression& score, 
	unsigned num_sentence, DataSet& set)
{
	const unsigned label = set.get_label(num_sentence);
	Expression loss_expr = pickneglogsoftmax(score, label);

	return loss_expr;
}


/**
	* \name get_neg_log_softmax
	* \brief Loss function (negative log likelihood).
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param num_sentence : numero of the instance.
	* \param cg : computation graph.
	* 
	* \return The negative log likelihood of the "num_sentence"th 
	* instance's label after taking the softmax. 
*/
Expression get_neg_log_softmax(RNN& rnn, DataSet& set, 
	Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	Expression loss_expr = rnn.get_neg_log_softmax(set, embedding, num_sentence, cg);
	return loss_expr;
}


	/* Training algorithms */
	
	
/**
	* \name train_score
	* \brief Trains the model (updates parameters).
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param set : the data set. 
	* \param embedding : word embeddings.
	* \param nb_batches : number of batches.
	* \param nb_samples : number of instances processed.
	* \param batch_size : batch size.
	* \param completed_epoch : number of completed epochs.
	* \param nb_of_instances : number of instances.
	* \param trainer : trainer object to update parameters.
	* \param order : vector containing numeros of instances.
	* \param index_order : numero of the indice for the order vector.
*/
void train_score(RNN& rnn, DataSet& set, Embeddings& embedding, 
	unsigned nb_batches, unsigned& nb_samples, unsigned batch_size, 
	unsigned completed_epoch, unsigned nb_of_instances, Trainer* trainer, 
	vector<unsigned>& order, unsigned& index_order)
{
	double loss = 0;

	for(unsigned batch = 0; batch < nb_batches; ++batch)
	{
		ComputationGraph cg; // we create a new computation graph for the epoch, not each item.
		vector<Expression> losses;

		// we will treat all those sentences as a single batch
		for (unsigned si = 0; si < batch_size; ++si) 
		{
			Expression loss_expr = get_neg_log_softmax(rnn, set, embedding, order[index_order], cg);
			losses.push_back(loss_expr);
			++index_order;
		}
		// Increment number of samples processed
		nb_samples += batch_size;

		// From a DyNet tutorial :
		// We accumulated the losses from all the batch.
		// Now we sum them, and do forward-backward as usual.
		// Things will run with efficient batch operations.
		Expression sum_losses = (sum(losses)) / (double)batch_size; //averaging the losses
		loss += as_scalar(cg.forward(sum_losses));
		cg.backward(sum_losses); //backpropagation gradient
		trainer->update(); //update parameters

		if(batch==nb_batches-1)
		{
			trainer->status();
			cerr << " Epoch " << completed_epoch+1 << " | E = " << (loss / static_cast<double>(nb_of_instances))
				 << " | samples processed = "<<nb_samples<<endl;
		}
	}
}


/**
	* \name dev_score
	* \brief Evaluates if the model is better than before during epoch. 
	* Saves it if it's the case.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param model : the model (to save the best model).
	* \param dev_set : the dev data set. 
	* \param embedding : word embeddings.
	* \param parameter_filename : file to save the parameters.
	* \param nb_of_instances_dev : number of instances in 
	* the SNLI dev corpus.
	* \param best : the best score of number of correct predicted labels.
	* \param output_emb_filename : file to save the embeddings.
*/
void dev_score(RNN& rnn, ParameterCollection& model, DataSet& dev_set, 
	Embeddings& embedding, string parameter_filename, 
	unsigned nb_of_instances_dev, unsigned& best, char* output_emb_filename)
{
	unsigned positive = predict_dev_and_test(rnn, dev_set, embedding, nb_of_instances_dev, best);
	
	//save the model if it's better than before
	if (positive > best) 
	{
		cerr << "it's better !\n";
		best = positive;
		TextFileSaver saver(parameter_filename);
		saver.save(model);
		if(output_emb_filename != NULL)
			embedding.print_embedding(output_emb_filename);
	}

}


	/* Training algorithm (handler) */


/**
	* \name run_train
	* \brief Main function to trains the model.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param model : the model (to save the best model).
	* \param set : the training data set. 
	* \param dev_set : the dev data set. 
	* \param embedding : word embeddings.
	* \param output_emb_filename : file to save the embeddings.
	* \param nb_epoch : number of epoch fixed.
	* \param batch_size : batch size.
*/
void run_train(RNN& rnn, ParameterCollection& model, DataSet& set, 
	DataSet& dev_set, Embeddings& embedding, char* output_emb_filename, 
	unsigned nb_epoch, unsigned batch_size)
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
	unsigned nb_of_instances = set.get_nb_instances();
	unsigned nb_of_instances_dev = dev_set.get_nb_instances();

	// Number of batches in training set
	unsigned nb_batches = nb_of_instances / batch_size; 
	cerr << "nb of instances = " << nb_of_instances << endl;
	cerr << "batches size = " << batch_size << endl;
	cerr << "nb of batches = " << nb_batches << endl;

	vector<unsigned> order(nb_of_instances);
	for (unsigned i = 0; i < nb_of_instances; ++i) 
		order[i] = i;
	unsigned index_order;
	
	for(unsigned completed_epoch=0; completed_epoch < nb_epoch; ++completed_epoch) 
	{
		// Reshuffle the data set
		cerr << "\n**SHUFFLE\n";
		random_shuffle(order.begin(), order.end());
		Timer iteration("completed in");

		nb_samples=0;
		index_order=0;
		
		rnn.enable_dropout();
		
		train_score(rnn, set, embedding, nb_batches, nb_samples, 
			batch_size, completed_epoch, nb_of_instances, trainer, order, index_order);
			
		rnn.disable_dropout();
		
		dev_score(rnn, model, dev_set, embedding, parameter_filename, 
			nb_of_instances_dev, best, output_emb_filename);
	}
}

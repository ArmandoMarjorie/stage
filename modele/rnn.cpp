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

vector<float> RNN::predict_algo(Expression& x, ComputationGraph& cg, bool print_proba, unsigned& argmax)
{
	vector<float> probs = as_vector(cg.forward(x));
	argmax=0;

	for (unsigned k = 0; k < probs.size(); ++k) 
	{
		if(print_proba)
			cerr << "proba[" << k << "] = " << probs[k] << endl;
		if (probs[k] > probs[argmax])
			argmax = k;
	}
	
	return probs;
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
		rnn.predict(dev_set, embedding, i, cg, false, label_predicted);
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


vector<float> run_predict_for_server_lime(RNN& rnn, Data& test_set, Embeddings& embedding, bool print_label)
{
	//cerr << "Testing ...\n";
	unsigned nb_of_sentences = test_set.get_nb_sentences();
	unsigned label_predicted;
	rnn.disable_dropout();
	ComputationGraph cg;
	vector<float> probas = rnn.predict(test_set, embedding, 0, cg, false, label_predicted);
	if(print_label)
		cerr << "True label = " << test_set.get_label(0) << ", label predicted = " << label_predicted << endl;
	return probas;
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

/**
	* \name run_predict_couple
	* \brief For each sample in the dataset, it predicts the label for each couple in the file "expl_token"
	* Example : P = "the dog is sleeping" ; H = "the cat is sleeping"
	* Couple 1 = dog/cat
	* Couple 2 = sleeping/sleeping
	* It will make a prediction for P = "dog" ; H = "cat" and for P = "sleeping" ; H = "sleeping"
	*
	* \param rnn : The rnn (LSTM or BiLSTM)
	* \param model : The model
	* \param explication_set : The dataset 
	* \param embedding : The word embedding table 
	* \param parameters_filename : File containing the model's weights to populate
*/
void run_predict_couple(RNN& rnn, ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename)
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
		rnn.predict(explication_set, embedding, i, cg, false, label_predicted);
		output << explication_set.get_label(i) << endl << label_predicted << endl;
		save_sentences(explication_set, premise, hypothesis, i);
		write_sentences(output, premise, hypothesis);
		for(unsigned j=0; j < explication_set.get_nb_couple(i); ++j) // parcours de tous les couples
		{
			explication_set.taking_couple(j,i);
			rnn.predict(explication_set, embedding, i, cg, false, label_predicted);
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

void run_predict_removing_couple(RNN& rnn, ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	unsigned label_predicted;
	unsigned label_predicted_true_sample;
	const unsigned nb_of_sentences = explication_set.get_nb_sentences();
	rnn.disable_dropout();
	char const* name = "Files/expl_removing_couple_token";
	ofstream output(name, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << name << endl;
		exit(EXIT_FAILURE);
	}	
	
	/* Algo :
	 * faire la prédiction normale
	 * ecrire dans le fichier le label de base et la prediciton, la premisse, l'hypothese
	 * pour chaque couple :
	 * 	le grisouiller 
	 * 	faire une prédiction
	 * 	peut etre ecrire le label obtenu
	 * 	calculer son DI
	 * 	ecrire le couple et son DI
	 * 
	 */ 	
	
	//float DI=0;
	
	vector<float> DI(NB_CLASSES,0.0);
	vector<unsigned> num_couple(1);
	
	for(unsigned i=0; i<nb_of_sentences; ++i) //pour un sample ...
	{
		ComputationGraph cg;
		vector<float> original_probs = rnn.predict(explication_set, embedding, i, cg, false, label_predicted_true_sample);
		output << explication_set.get_label(i) << endl << label_predicted_true_sample << endl;
		explication_set.print_sentences_of_a_sample(i, output);
		for(unsigned j=0; j < explication_set.get_nb_couple(i); ++j) // parcours de tous les couples
		{
			cg.clear();
			num_couple[0] = j;
			explication_set.remove_couple(num_couple ,i);
			vector<float> probs = rnn.predict(explication_set, embedding, i, cg, false, label_predicted);
			write_couple(output, explication_set, i, j);
			//DI = calculate_DI(probs, original_probs, label_predicted_true_sample);
			calculate_DI_label(probs, original_probs, DI);
			//output << "-1 " << DI << endl;
			output << "-1 ";
			for(unsigned k=0; k<NB_CLASSES; ++k)
				output << DI[k] << " ";
			output << endl;
			std::fill(DI.begin(), DI.end(), 0.0);
			explication_set.reset_couple(num_couple, i);
		}
		output << " -3\n";
	}
	
	output.close();	
}

/* Sans coeff pour l'instant */
float calculate_DI(vector<float>& probs, vector<float>& original_probs, unsigned label_predicted)
{
	float distance;
	float DI = 0;
	for(unsigned label = 0; label < NB_CLASSES; ++label)
	{
		distance = probs[label] - original_probs[label];
		if(label == label_predicted)
		{
			distance = -distance;
		}
		DI += distance;
	}
	return DI;
}

/* Calcul de DI pour chaque label (comme Lime)*/
void calculate_DI_label(vector<float>& probs, vector<float>& original_probs, vector<float>& DI)
{
	float distance;
	for(unsigned label_ref = 0; label_ref < NB_CLASSES; ++label_ref)
	{
		for(unsigned label = 0; label < NB_CLASSES; ++label)
		{
			distance = probs[label] - original_probs[label];
			if(label == label_ref)
			{
				distance = -distance;
			}
			DI[label_ref] += distance;
		}
	}
}


void generate_all_masks(RNN& rnn, ParameterCollection& model, Data& explication_set, Embeddings& embedding, char* parameters_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	unsigned label_predicted;
	unsigned label_predicted_true_sample;
	const unsigned nb_of_sentences = 1;//explication_set.get_nb_sentences();
	rnn.disable_dropout();
	char const* name = "Files/expl_removing_couple_token";
	ofstream output(name, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << name << endl;
		exit(EXIT_FAILURE);
	}	

	vector<unsigned> premise;
	vector<unsigned> hypothesis;
	unsigned branch=0;
	unsigned nb_words_removed=0;
	unsigned emp;
	bool end;
	float min=9e+99;
	float score;
	vector<pair<unsigned,unsigned>> save_premise; //the word id (first) and its position (second)
	vector<pair<unsigned,unsigned>> save_hypothesis;
	for(unsigned i=0; i < nb_of_sentences; ++i)
	{
		unsigned stack_size = explication_set.get_nb_words(1,i) + explication_set.get_nb_words(2,i);
		vector<pair<bool,bool>> stack(stack_size, make_pair(true,false)); //first -> false if we want to remove the word else true, second -> false if we didn't change the val else true
		save_sentences(explication_set, premise, hypothesis, i);
		end = false;
		while(!end)
		{
			//cerr << "on a passé " << branch << " tour de boucle\n";
			ComputationGraph cg;
			if(branch!=0)
				explication_set.remove_words_from_stack(stack,i);
			cerr << "boucle "<<branch<<endl;
			explication_set.print_sentences_of_a_sample(i);
			//cerr << "pile = \n";
			print_stack(stack, explication_set.get_nb_words(1,i));
			sleep(5);
			if(stack.empty())
				cerr << "it is empty (:\n";
			
			vector<float> probs = rnn.predict(explication_set, embedding, i, cg, false, label_predicted);
			if(branch!=0)
			{
				score = nb_words_removed + probs[label_predicted];
				if(score < min)
				{
					min = score;
					saving_branch(stack, save_premise, save_hypothesis, explication_set, i);
				}
			}
			if(branch!=0)
				explication_set.reset_words_from_stack(stack, premise, hypothesis, i);
			++branch;
			if(!(stack[stack.size()-1].second))
			{
				change_val(stack, nb_words_removed);
				//cerr << "\t[CHANGEMENT VALEUR]\n";
				//print_stack(stack, explication_set.get_nb_words(1,i));
			}
			else
			{
				do
				{
					//dépiler, changer de valeur et empiler
					while(!stack.empty() && stack[stack.size()-1].second)
					{
						stack.pop_back();
						//cerr << "\t[DEPILE]\n";
						//print_stack(stack, explication_set.get_nb_words(1,i));
						--nb_words_removed;
					}
					if(stack.empty() || premise_empty(stack, explication_set.get_nb_words(1,i))) //fini quand premise vide
					{
						end = true;
						continue;
					}
					change_val(stack, nb_words_removed);
					//cerr << "\t[CHANGEMENT VALEUR]\n";
					//print_stack(stack, explication_set.get_nb_words(1,i));
					for(emp = stack.size()-1; emp<stack_size-1; ++emp)
						stack.push_back(make_pair(true,false));
					//cerr << "\t[EMPILAGE 1]\n";
					//print_stack(stack, explication_set.get_nb_words(1,i));
				}while(!end && hypothesis_empty(stack, explication_set.get_nb_words(1,i))); //gère l'hypothèse vide
			}
		}
		/* write save in file */
		for(unsigned k=0; k<save_premise.size(); ++k)
			cerr << save_premise[k].first << " ";
		cerr << endl;
		premise.clear();
		hypothesis.clear();
	}
} 

void print_stack(vector<pair<bool,bool>>& stack, unsigned premise_length)
{
	for(unsigned i=0; i<stack.size(); ++i)
	{
		if(i==premise_length)
			cerr << endl;
		cerr << stack[i].first << " ";
	}
	cerr << endl;
}

bool premise_empty(vector<pair<bool,bool>>& stack, unsigned premise_length)
{
	for(unsigned i=0; i<premise_length; ++i)
		if(stack[i].first)
			return false;
	return true;
}

bool hypothesis_empty(vector<pair<bool,bool>>& stack, unsigned premise_length)
{
	for(unsigned i=premise_length; i<stack.size(); ++i)
		if(stack[i].first)
			return false;
	return true;
	
}

void change_val(vector<pair<bool,bool>>& stack, unsigned& nb_words_removed)
{
	stack[stack.size()-1].first = false;
	stack[stack.size()-1].second = true;	
	++nb_words_removed;
}

void saving_branch(vector<pair<bool,bool>>& stack, vector<pair<unsigned,unsigned>>& save_premise, 
	vector<pair<unsigned,unsigned>>& save_hypothesis, Data& explication_set, unsigned num_sample)
{
	unsigned premise_length = explication_set.get_nb_words(1,num_sample);
	for(unsigned i=0; i<stack.size(); ++i)
		if(stack[i].first)
		{
			if(i<premise_length)
				save_premise.push_back(make_pair(explication_set.get_word_id(1,num_sample,i),i));
			else
				save_hypothesis.push_back(make_pair(explication_set.get_word_id(2,num_sample,i),i));
		}
}



	/* Negative log softmax algorithms */

Expression get_neg_log_softmax(RNN& rnn, Data& set, Embeddings& embedding, unsigned num_sentence, ComputationGraph& cg)
{
	Expression loss_expr = rnn.get_neg_log_softmax(set, embedding, num_sentence, cg);
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



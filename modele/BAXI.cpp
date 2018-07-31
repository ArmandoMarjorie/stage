#include <sys/types.h>
#include <unistd.h>
#include "BAXI.hpp"

using namespace std;
using namespace dynet;


/**
 * \file BAXI.cpp
*/


/**
	* \name evaluate_importance
	* \brief Evaluate the importance of each expression of an instance 
	* with the BAXI method.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param cg : computation graph.
	* \param explication_set : the Interp-SNLI data set. 
	* \param embedding : word embeddings.
	* \param is_premise : true if we process the premise, else false.
	* \param original_probs : probability of each label of the 
	* original instance.
	* \param expr_importance : vector containing each 
	* expression (from the premise and the hypothesis)'s importance.
	* \param num_sample : numero of the instance.
	* \param copy : the original instance.
*/
void evaluate_importance(RNN& rnn, ComputationGraph& cg, 
	DataSet& explication_set, Embeddings& embedding,
	bool is_premise, vector<float>& original_probs, 
	vector<ExplanationsBAXI>& expr_importance, unsigned num_sample, 
	Data* copy)
{
	float max_importance = -9999; 
	float importance = -9999; 
	
	unsigned label_predicted;
	unsigned label_explained = explication_set.get_label(num_sample);
	unsigned nb_alternative_words;
	unsigned nb_expr = explication_set.get_nb_expr(is_premise, num_sample);
	
	// For every expression in the given sentence
	for(unsigned num_expr=0; num_expr < nb_expr; ++num_expr)
	{
		//We do not evaluate some expression (like "a", "the" for example)
		if(! explication_set.expr_is_important(num_sample, 
			is_premise, num_expr))
			continue;
			
		nb_alternative_words = 
			explication_set.get_nb_switch_words(is_premise, num_expr, num_sample);
			
		//Search the alternative expression that maximises the importance
		for(unsigned nb=0; nb<nb_alternative_words; ++nb)
		{
			cg.clear();
			explication_set.modif_word(is_premise, 
				num_expr, nb, num_sample);
			
			vector<float> probs = rnn.predict(explication_set, embedding, 
				num_sample, cg, false, label_predicted);
			
			
			importance =  
				original_probs[label_explained] - probs[label_explained];
			if(importance > max_importance)
				max_importance = importance;
			
			explication_set.reset_data(*copy, num_sample);
		}
		expr_importance.push_back(
			ExplanationsBAXI(num_expr, is_premise, importance)); 
	}
}


/**
	* \name write_explanations
	* \brief Writes, for each instance, expressions sorted 
	* by their importance.
	* 
	* \param output : write the explanations in this file.
	* \param expr_importance : vector containing each 
	* expression (from the premise and the hypothesis)'s importance.
	* \param detok : to detoken the words ID.
	* \param copy : the original instance.
*/
void write_explanations(ofstream& output, 
	vector<ExplanationsBAXI>& expr_importance, Detokenisation& detok, 
	Data* copy)
{
	sort(expr_importance.begin(), expr_importance.end(), 
		greater<ExplanationsBAXI>()); 
		
	for(unsigned i=0; i < expr_importance.size(); ++i)
	{
		output << "('" << detok.detoken(expr_importance[i].num_expr, 
			expr_importance[i].is_premise, *copy) << "'), " 
			<<  expr_importance[i].importance << endl;
	}
	
	output << "-3\n\n\n";
}


/**
	* \name print_labels_accuracy
	* \brief Prints accuracy on predicted labels
	* (global and per labels).
	* 
	* \param total : number of labels correctly predicted.
	* \param correct_label : vector containing, for each label, the 
	* number of labels correctly predicted.
*/
void print_labels_accuracy(DataSet& explication_set, unsigned total, 
	vector<unsigned>& correct_label)
{
	double nb_of_instances = 
		static_cast<double>(explication_set.get_nb_instances());
	double nb_neutral = 
		static_cast<double>(explication_set.get_nb_neutral());
	double nb_entailment = 
		static_cast<double>(explication_set.get_nb_inf());
	double nb_contradiction = 
		static_cast<double>(explication_set.get_nb_contradiction());
	
	cout << "Success Rate = " 
		 << 100 * (total / nb_of_instances) << endl;
	
	cout << "\tSuccess Rate neutral = " 
		 << 100 * (correct_label[0] / nb_neutral ) 
		 << endl << "\t" << correct_label[0] << " / " 
		 << nb_neutral << endl;
		
	cout << "\tSuccess Rate entailment = " 
		 << 100 * (correct_label[1] / nb_entailment) 
		 << endl << "\t" << correct_label[1] << " / " 
		 << nb_entailment << endl;
		
	cout << "\tSuccess Rate contradiction = " 
		 << 100 * (correct_label[2] / nb_contradiction) 
		 << endl << "\t" << correct_label[2] << " / "
		 << nb_contradiction << endl;	
}


/**
	* \name original_prediction
	* \brief Predicts the original instance's label. 
	* 
	* \param explication_set : the data set. 
	* \param embedding : word embeddings.
	* \param num_sample : numero of the instance.
	* \param cg : computation graph.
	* \param total : number of labels correctly predicted.
	* \param correct_label : vector containing, for each label, the 
	* number of labels correctly predicted.
	* \param output : output file.
	* 
	* \return Vector containing each label's probabilities.
*/
vector<float> original_prediction(RNN& rnn, DataSet& explication_set, 
	Embeddings& embedding, unsigned num_sample, ComputationGraph& cg, 
	unsigned& total, vector<unsigned>& correct_label, ofstream& output)
{
	unsigned label_predicted;
	unsigned true_label = explication_set.get_label(num_sample);
	
	vector<float> original_probs = rnn.predict(explication_set, 
		embedding, num_sample, cg, false, label_predicted);
		
	if(label_predicted == true_label)
	{
		++total;
		++correct_label[label_predicted];
	}
		
	output << true_label << endl << label_predicted << endl;
	output << "neutral : " << original_probs[0] 
		   << ", entailment : " << original_probs[1] 
		   << ", contradiction : " << original_probs[2] << endl;
		   
	return original_probs;
}


/**
	* \name BAXI
	* \brief BAXI method. Sorts the most important expressions in an 
	* instance and write them in a file.
	* 
	* \param rnn : a LSTM or a BiLSTM.
	* \param model : the model.
	* \param explication_set : the data set. 
	* \param embedding : word embeddings.
	* \param parameters_filename : preexisting weights.
	* \param lexique_filename : file containing the SNLI vocabulary 
	* with their ID.
	* \param output_filename : output file's name.
*/
void BAXI(RNN& rnn, ParameterCollection& model, 
	DataSet& explication_set, Embeddings& embedding, 
	char* parameters_filename, char* lexique_filename, 
	char* output_filename)
{
	populate_from_file(parameters_filename, model);

	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " 
			 << output_filename << endl;
		exit(EXIT_FAILURE);
	}		

	const unsigned nb_of_instances = explication_set.get_nb_instances();
	unsigned total = 0;  
	vector<unsigned> correct_label(NB_CLASSES,0); 
	Detokenisation detok(lexique_filename);
	Data* copy = NULL;
	
	rnn.disable_dropout();
	for(unsigned i=0; i<nb_of_instances; ++i) // for every instances
	{
		copy = 
			new Data( *(explication_set.get_data_object(i)) );
		vector<ExplanationsBAXI> expr_importance;
		ComputationGraph cg;
		
		// Probabilities of labels on the original instance
		vector<float> original_probs = 
			original_prediction(rnn, explication_set, embedding, i, cg, total, 
			correct_label, output);
		
		// Evaluation of the premise's expressions' importance 
		evaluate_importance(rnn, cg, explication_set, embedding,  
			true, original_probs, expr_importance, i, copy);
		
		// Evaluation of the hypothesis' expressions' importance 
		evaluate_importance(rnn, cg, explication_set, embedding,  
			false, original_probs, expr_importance, i, copy);
		
		write_explanations(output, expr_importance, detok, copy);
		
	}	
	output.close();
	print_labels_accuracy(explication_set, total, correct_label);

}



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
	* \param num_expr : numero of the expression in the premise or 
	* in the hypothesis.
	* \param is_premise : true if we process the premise, else false.
	* \param original_probs : probability of each label of the 
	* original instance.
	* \param expr_importance : vector containing each 
	* expression (from the premise and the hypothesis)'s importance.
	* \param num_sample : numero of the instance.
	* \param copy : the original instance.
*/
void evaluate_importance(RNN& rnn, ComputationGraph& cg, 
	DataSet& explication_set, Embeddings& embedding, unsigned num_expr,
	bool is_premise, vector<float>& original_probs, 
	vector<ExplanationsBAXI>& expr_importance, unsigned num_sample, 
	Data* copy)
{
	float max_importance = -9999; 
	float importance;
	
	unsigned label_predicted;
	unsigned label_explained = explication_set.get_label(num_sample);
	unsigned nb_alternative_words = 
		explication_set.get_nb_switch_words(is_premise, num_expr, num_sample);
	
	// Search the alternative expression that maximises the importance
	for(unsigned nb=0; nb<nb_alternative_words; ++nb)
	{
		cg.clear();
		explication_set.modif_word(is_premise, num_expr, nb, num_sample);
		
		vector<float> probs = rnn.predict(explication_set, embedding, 
			num_sample, cg, false, label_predicted);
		
		
		importance =  original_probs[label_explained] - probs[label_explained];
		if(importance > max_importance)
			max_importance = importance;
		
		explication_set.reset_data(*copy, num_sample);
	}
		
	expr_importance.push_back(ExplanationsBAXI(num_expr, is_premise, importance)); 
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
	sort(expr_importance.begin(), expr_importance.end(), greater<ExplanationsBAXI>()); 
	for(unsigned i=0; i < expr_importance.size(); ++i)
	{
		output << "('" << detok.detoken(expr_importance[i].num_expr, 
			expr_importance[i].is_premise, *copy) << "'), " 
			<<  expr_importance[i].importance << endl;
	}
	
	output << "-3\n\n\n";
}


void BAXI(RNN& rnn, ParameterCollection& model, DataSet& explication_set, 
	Embeddings& embedding, char* parameters_filename, char* lexique_filename)
{
	cerr << "Loading parameters ...\n";
	TextFileLoader loader(parameters_filename);
	loader.populate(model);
	cerr << "Parameters loaded !\n";

	cerr << "Testing ...\n";
	unsigned label_predicted;
	unsigned label_predicted_true_sample;
	const unsigned nb_of_instances = explication_set.get_nb_intances();
	rnn.disable_dropout();
	char* name = "Files/expl_token_changing_word";
	ofstream output(name, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << name << endl;
		exit(EXIT_FAILURE);
	}		
	
	
	unsigned prem_size, hyp_size, position;
	float importance;
	unsigned nb_imp_words_prem;
	unsigned nb_imp_words_hyp;
	unsigned true_label;
	
	
	vector<unsigned> positive(NB_CLASSES,0); 
	unsigned pos = 0;  
	Data* copy=NULL;
	Detokenisation detok(lexique_filename);
	
	for(unsigned i=0; i<nb_of_instances; ++i) // pour chaque instance...
	{
		vector<ExplanationsBAXI> expr_importance;
		
		cout << "SAMPLE " << i << "\n";
		ComputationGraph cg;
		copy = new Data( *(explication_set.get_data_object(i)) ); // COPY DATA
		
		// original prediction
		true_label = explication_set.get_label(i);
		vector<float> original_probs = rnn.predict(explication_set, embedding, i, cg, false, label_predicted_true_sample);
		if(label_predicted_true_sample == true_label)
		{
			++pos;
			++positive[label_predicted_true_sample];
		}
			
		output << true_label << endl << label_predicted_true_sample << endl;
		output << "neutral : " << original_probs[0] << ", entailment : " << original_probs[1] << ", contradiction : " << original_probs[2] << endl;
		
		// In the premise
		for( position=0; position < explication_set.get_nb_expr(true,i); ++position)
			if(explication_set.expr_is_important(i, true, position))
				evaluate_importance(rnn, cg, explication_set, embedding, position, true, original_probs, expr_importance, i, copy);

		// In the hypothesis
		for(position=0; position < explication_set.get_nb_expr(false,i); ++position)
			if(explication_set.expr_is_important(i, false, position))
				evaluate_importance(rnn, cg, explication_set, embedding, position, false, original_probs, expr_importance, i, copy);
		
		
		write_explanations(output, expr_importance, detok, copy);
		
		
	}	
	output.close();
	cout << "Success Rate = " << 100 * (pos / (double)nb_of_instances) << endl;
	cout << "\tSuccess Rate neutral = " << 100 * (positive[0] / (double)explication_set.get_nb_neutral()) << endl << "\t" << positive[0] << " / "<< explication_set.get_nb_neutral() << endl;
	cout << "\tSuccess Rate entailment = " << 100 * (positive[1] / (double)explication_set.get_nb_inf()) << endl << "\t" << positive[1] << " / "<< explication_set.get_nb_inf() << endl;
	cout << "\tSuccess Rate contradiction = " << 100 * (positive[2] / (double)explication_set.get_nb_contradiction()) << endl << "\t" << positive[2] << " / "<< explication_set.get_nb_contradiction() << endl;

}

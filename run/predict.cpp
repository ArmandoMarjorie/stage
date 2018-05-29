#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <gtkmm/main.h>
#include "../modele/LSTM.hpp"
#include "../modele/BILSTM.hpp"

using namespace std;
using namespace dynet;

void usage(char* exe_name) 
{
	cerr << "\n**USAGE**\n\t" << exe_name << " test_file embedding_file nb_layers input_dim hidden_dim parameters_file system\n\n"
/*1*/		 << "test_file (with mode 0) OR lexique_file (with mode 2) <string> : test file containing exemples of 2 sentences and their labels\n"
/*2*/		 << "embedding_file <string> : file containing word embeddings used in the training step\n"
/*3*/		 << "nb_layers <int> : number of layers\n"
/*4*/		 << "input_dim <int> : dimension of the word embedding\n"
/*5*/		 << "hidden_dim <int> : dimension  of the hidden states ht and ct\n"
/*6*/		 << "parameters_file <string> : file containing the parameters (weight and bias) updated in the training step\n"
/*7*/		 << "system <int> : which system you want to use (1, 2 or 3=KIM)\n"
/*8*/		 << "lexique_file <string> : lexique containing the IDs of each word\n"
/*9*/		 << "sw neutral <string> : Files/files_test/remplac_token.neutral\n"
/*10*/		 << "sw inf <string> : Files/files_test/remplac_token.neutral\n"
/*11*/		 << "sw contradiction <string> : Files/files_test/remplac_token.neutral\n";



/*9*/		 //<< "bigram_prob_file <string> : Files/bigram_proba_log\n"
/*10*/		 //<< "unigram file <string> : Files/unigram\n"
/*11*/		 //<< "switch words file <string> : Files/files_test/mots_remplacants_interpretation_test\n"
			// << "uni_prob_file <string> : Files/uni_proba_log\n";
		 //<< "mode <int> : 0 = with file, 1 = verbose with word id as input, 2 = verbose with word as input\n";
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{ 
	if(argc > 0 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc != 12 )
	{
		cerr << "Usage pour tester :\n " 
		<< argv[0] << " test_file " << " embedding_file " 
		<<" nb_layers " << " input_dim " << " hidden_dim " << " model_file " << " systeme \n";
		exit(EXIT_FAILURE);
	}

	Gtk::Main app(argc, argv);
	// Fetch dynet params 
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);

	// Build model 
	ParameterCollection model;
							 
	// Load Dataset 
	Embeddings embedding(argv[2], model, static_cast<unsigned>(atoi(argv[4])), true);
	Switch_Words* sw_neutral = new Switch_Words(argv[9]);
	Switch_Words* sw_inf = new Switch_Words(argv[10]);
	Switch_Words* sw_con = new Switch_Words(argv[11]);
	vector<Switch_Words*> sw_vect;
	sw_vect.push_back(sw_neutral);
	sw_vect.push_back(sw_inf);
	sw_vect.push_back(sw_con);


	//Proba_Bigram pb(argv[9], argv[10], argv[12]);
	
	
	//unsigned mode=static_cast<unsigned>(atoi(argv[8]));

	int systeme = atoi(argv[7]);
	
	// Create model
	if(systeme < 3 || systeme == 5)
	{
		LSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), 
		static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);
		Data set(argv[1], 1);
		cout << "DATA SET OK \n";
		change_words_for_mesure(rnn, model, set, embedding, argv[6], argv[8], sw_vect);
		

		
		/**
		Data explication_set(argv[1], 3); 
		run_predict_removing_couple(rnn, model, explication_set, embedding, argv[6]); //method with DI*/
		/**
		Data explication_set(argv[1]);
		generate_couple_masks(rnn, model, explication_set, embedding, argv[6], argv[8]); //methode O(n*m) retire chaque couple */
		
		/* For 'normal' test prediction
		Data set(argv[1]);
		run_predict(rnn, model, set, embedding, argv[6]);
		*/ 
	}
	else
	{
		Data test_set(argv[1]);
		//test_set.print_infos(3);
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);
		//run_prediction_expl_for_sys_4(rnn, model, test_set, embedding, argv[6], argv[8]);
		//change_words_for_mesure(rnn, model, set, embedding, argv[6], argv[8], sw_vect);
	}
	
	for (unsigned i =0; i< sw_vect.size();i++)
	{
		delete (sw_vect[i]);
	} 
	sw_vect.clear();

	return 0;
}

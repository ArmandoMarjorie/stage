#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "../modele/rnn.hpp"

using namespace std;
using namespace dynet;

void usage(char* exe_name) 
{
	cerr << "\n**USAGE**\n\t" << exe_name << " test_file embedding_file nb_layers input_dim hidden_dim parameters_file system\n\n"
		 << "test_file (with mode 0) OR lexique_file (with mode 2) <string> : test file containing exemples of 2 sentences and their labels\n"
		 << "embedding_file <string> : file containing word embeddings used in the training step\n"
		 << "nb_layers <int> : number of layers\n"
		 << "input_dim <int> : dimension of the word embedding\n"
		 << "hidden_dim <int> : dimension  of the hidden states ht and ct\n"
		 << "parameters_file <string> : file containing the parameters (weight and bias) updated in the training step\n"
		 << "system <int> : which system you want to use (1, 2 or 3=KIM)\n"
		 << "mode <int> : 0 = with file, 1 = verbose with word id as input, 2 = verbose with word as input\n";
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{ 
	if(argc > 0 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc != 8 )
	{
		cerr << "Usage pour tester :\n " 
		<< argv[0] << " test_file " << " embedding_file " 
		<<" nb_layers " << " input_dim " << " hidden_dim " << " model_file " << " systeme \n";
		exit(EXIT_FAILURE);
	}

	// Fetch dynet params 
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);

	// Build model 
	ParameterCollection model;
							 
	// Load Dataset 
	Embeddings embedding(argv[2], model, static_cast<unsigned>(atoi(argv[4])), true);
	
	//unsigned mode=static_cast<unsigned>(atoi(argv[8]));

	int systeme = atoi(argv[7]);
	
	// Create model
	if(systeme < 3)
	{
		LSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, model, static_cast<unsigned>(systeme));
		/*if(mode==0)
		{
			Data test_set(argv[1]);
			test_set.print_infos(3); //3 = test set
			run_predict(rnn, model, test_set, embedding, argv[6]);
		}*/
		/* TESTING IF REMOVE_COUPLE() & RESET_COUPLE() IS CORRECT */
		/*
		Data test_explication_set(argv[1],0);
		vector<unsigned> num_couple(2);
		num_couple[0] = 1;
		num_couple[1] = 0;
		
		test_explication_set.remove_couple(num_couple,0);
		char const* name = "Files/output_sans_couple_0";
		test_explication_set.print_sentences(name);
		test_explication_set.reset_couple(num_couple,0);
		char const* name_2 = "Files/output_reset_couple";
		test_explication_set.print_sentences(name_2);*/
	/*	else if(mode==1)
		{
			Data verbose_set(1);
			run_predict_verbose(rnn, model, verbose_set, embedding, argv[6]);
		}
		else if(mode==2)
		{
			Data verbose_set(2, argv[1]);
			run_predict_verbose(rnn, model, verbose_set, embedding, argv[6]);
		}*/
		
		
		Data explication_set(argv[1], 3); 
		run_predict_couple(rnn, model, explication_set, embedding, argv[6]);
	}
	else
	{
		Data test_set(argv[1]);
		test_set.print_infos(3);
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, model);
		run_predict(rnn, model, test_set, embedding, argv[6]);
	}

	return 0;
}

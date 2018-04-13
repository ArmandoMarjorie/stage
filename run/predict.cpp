#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "../modele/lstm.hpp"
#include "../modele/bilstm.hpp"

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
	if( argc != 9 )
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
	
	unsigned mode=static_cast<unsigned>(atoi(argv[8]));

	int systeme = atoi(argv[7]);
	
	// Create model
	if(systeme < 3)
	{
		LSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);
		if(mode==0)
		{
			Data test_set(argv[1]);
			test_set.print_infos(3); //3 = test set
			rnn.run_predict(model, test_set, embedding, argv[6]);
		}
		else if(mode==1)
		{
			Data verbose_set(1);
			rnn.run_predict_verbose(model, verbose_set, embedding, argv[6]);
		}
		else if(mode==2)
		{
			Data verbose_set(2, argv[1]);
			rnn.run_predict_verbose(model, verbose_set, embedding, argv[6]);
		}
		else if(mode==3)
		{
			Data explication_set(argv[1], 3); 
			Data new_set(explication_set, 1);
			rnn.run_predict_explication(model, explication_set, embedding, argv[6]);
		}
	}
	else
	{
		Data test_set(argv[1]);
		test_set.print_infos(3);
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, model);
		rnn.run_predict(model, test_set, embedding, argv[6]);
	}

	return 0;
}

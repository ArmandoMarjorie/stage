#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "../modele/LSTM.hpp"
#include "../modele/BILSTM.hpp"

using namespace std;
using namespace dynet;

void usage(char* exe_name) 
{
	cerr << "\n**USAGE**\n\t" << exe_name << " test_file embedding_file \
			nb_layers input_dim hidden_dim parameters_file system\n\n"
		 << "test_file <string> : test file containing exemples of 2 \
			sentences and their labels\n"
		 << "embedding_file <string> : file containing word embeddings \
			output by the training step\n"
		 << "nb_layers <int> : number of layers\n"
		 << "input_dim <int> :  word embedding dimension\n"
		 << "hidden_dim <int> : hidden states dimension\n"
		 << "parameters_file <string> : file containing the parameters \
			(weight and bias) updated in the training step\n"
		 << "system <int> : which system you want to use (1, 2 or 3)\n";
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{ 
	if(argc > 0 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc != 8 )
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// Fetch dynet params 
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);

	// Build model 
	ParameterCollection model;
							 
	// Load word embeddings
	Embeddings embedding(argv[2], model, 
		static_cast<unsigned>(atoi(argv[4])), true);
	unsigned systeme = static_cast<unsigned>(atoi(argv[7]));
	
	if(systeme < 3)
	{
		LSTM rnn(static_cast<unsigned>(atoi(argv[3])), 
			static_cast<unsigned>(atoi(argv[4])), 
			static_cast<unsigned>(atoi(argv[5])), 0, 
			systeme, model);
			
		DataSet set(argv[1],1);
		run_predict(rnn, model, set, embedding, argv[6]);
		
	}
	else
	{
		//test_set.print_infos(3);
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[3])), 
			static_cast<unsigned>(atoi(argv[4])), 
			static_cast<unsigned>(atoi(argv[5])), 0, 
			systeme, model);
			
		DataSet set(argv[1],1);
		run_predict(rnn, model, set, embedding, argv[6]);
	}
	

	return 0;
}

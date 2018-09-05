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
		 << "test_file <string> : test file \
			(Files/files_test/testing_token_id_texts)\n"
		 << "embedding_file <string> : file containing word embeddings \
			output by the training step\n"
		 << "nb_layers <int> : number of layers\n"
		 << "input_dim <int> :  word embedding dimension\n"
		 << "hidden_dim <int> : hidden states dimension\n"
		 << "parameters_file <string> : file containing the parameters \
			(weight and bias) updated in the training step\n"
		 << "system <int> : which system you want to use (1, 2 or 3)\n"
		 << "lexique_file <string> : lexique file (Files/snli.lexique)\n"
		 << "output_name <string> : explanations output file name\n";
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{ 
	if(argc > 1 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc != 10 )
	{
		cerr << "See \"" << argv[0] << " -h\" for the right command\n"; 
		exit(EXIT_FAILURE);
	}

	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);

	ParameterCollection model;
							 
	Embeddings embedding(argv[2], model, 
		static_cast<unsigned>(atoi(argv[4])), true);
	unsigned systeme = static_cast<unsigned>(atoi(argv[7]));
	cerr << "** SYSTEM " << systeme << " **\n";
	
	RNN* rnn;
	
	if(systeme < 3)
		rnn = new LSTM(static_cast<unsigned>(atoi(argv[3])), 
			static_cast<unsigned>(atoi(argv[4])), 
			static_cast<unsigned>(atoi(argv[5])), 0, 
			systeme, model, false);
	else
		rnn = new BiLSTM(static_cast<unsigned>(atoi(argv[3])), 
			static_cast<unsigned>(atoi(argv[4])), 
			static_cast<unsigned>(atoi(argv[5])), 0, 
			systeme, model, false);
			
	DataSet set(argv[1], false);
	BAXI(*rnn, model, set, embedding, argv[6], argv[8], argv[9]);
		
	return 0;
}

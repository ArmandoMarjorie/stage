#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <sys/types.h>
#include <unistd.h>
#include "../modele/LSTM.hpp"
#include "../modele/BILSTM.hpp"

using namespace std;
using namespace dynet;

void usage(char* exe_name)
{
	cerr << "\n**USAGE**\n\t" << exe_name << " train_file dev_file \
			nb_layers input_dim hidden_dim dropout nb_epoch batch_size \
			system embedding_file output_embedding_filename \n\n"
		 << "train_file <string> : train file containing exemples of \
			2 sentences and their labels\n"
		 << "dev_file <string> : dev file containing exemples of \
			2 sentences and their labels\n"    
		 << "nb_layers <int> : number of layers\n"                                                   
		 << "input_dim <int> : word embeddings dimension\n"                                   		 
		 << "hidden_dim <int> : hidden states dimension\n"                         
		 << "dropout <float> : dropout rate (between 0 and 1)\n"                                    
		 << "nb_epoch <int> : number of times the program will perform \
			the training phase\n"              
		 << "batch_size <int> : batches size\n"                                                   
		 << "system <int> : which system you want to use (1, 2, 3)\n"     
		 << "embedding_file <string> : file containing word embeddings\n"                            
		 << "output_embedding_filename <string> : name of the file \
			that will containing the word embeddings trained during the \
			training phase. This file has to be used in the testing \
			phase\n\n";

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{
	if(argc > 1 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc != 12 )
	{
		cerr << "See \"" << argv[0] << " -h\" for the right command\n"; 
		exit(EXIT_FAILURE);
	}

	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);

	ParameterCollection model;

	DataSet train_set(argv[1],1);
	DataSet dev_set(argv[2],1);
	Embeddings embedding(argv[10], model, 
		static_cast<unsigned>(atoi(argv[4])), false);

	unsigned systeme = static_cast<unsigned>(atoi(argv[9]));
	cerr << "** SYSTEM " << systeme << " **\n";
	
	RNN* rnn;
	
	if(systeme < 3)
		rnn = new LSTM(static_cast<unsigned>(atoi(argv[3])), 
			static_cast<unsigned>(atoi(argv[4])), 
			static_cast<unsigned>(atoi(argv[5])), strtof(argv[6], NULL), 
			systeme, model);
	else
		rnn = new BiLSTM(static_cast<unsigned>(atoi(argv[3])), 
			static_cast<unsigned>(atoi(argv[4])), 
			static_cast<unsigned>(atoi(argv[5])), strtof(argv[6], NULL), 
			systeme, model);
			
	run_train(*rnn, model, train_set, dev_set, embedding, argv[11], 
		static_cast<unsigned>(atoi(argv[7])), 
		static_cast<unsigned>(atoi(argv[8])));

	return 0;
}

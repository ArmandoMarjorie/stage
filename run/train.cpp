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
	cerr << "\n**USAGE**\n\t" << exe_name << " train_file dev_file nb_layers input_dim hidden_dim dropout nb_epoch batch_size"
		 << " systeme output_embedding_filename\n\n"
		 << "train_file <string> : train file containing exemples of 2 sentences and their labels\n" //1
		 << "dev_file <string> : dev file containing exemples of 2 sentences and their labels\n"     //2
		 << "nb_layers <int> : number of layers\n"                                                   //3
		 << "input_dim <int> : dimension of the word embedding\n"                                    //4
		 << "hidden_dim <int> : dimension  of the hidden states ht and ct\n"                         //5
		 << "dropout <float> : dropout rate (between 0 and 1)\n"                                     //6
		 << "nb_epoch <int> : number of times the program will do the training phase\n"              //7
		 << "batch_size <int> : size of batches\n"                                                   //8
		 << "systeme <int> : which system you want to use (1, 2, 3=KIM, or 4)\n"                     //9
		 << "embedding_file <string> : file containing word embeddings\n"                            //10
		 << "output_embedding_filename <string> : [optionnal] name of the file containing the word "  //11
		 << "embedding trained during the training phase. This file have to be used in the testing phase\n\n";

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{
	if(!strcmp(argv[1], "-h"))
		usage(argv[0]);
	if(argc < 11 || argc > 12)
	{
		cerr << "Usage pour entrainer :\n " 
		<< argv[0] << " train_file " << " dev_file "
		<< " nb_layers " << " input_dim " << " hidden_dim " 
		<< " dropout " << " nb_epochs " << " batch_size " << " systeme " << " embedding_file " << " output_embedding_filename\n";
		exit(EXIT_FAILURE);
	}

	// Fetch dynet params ----------------------------------------------------------------------------
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);

	// Build model -----------------------------------------------------------------------------------
	ParameterCollection model;
/*
	// Load dataset ----------------------------------------------------------------------------------
	Data train_set(argv[1]);
	Data dev_set(argv[2]);
	Embeddings embedding(argv[10], model, static_cast<unsigned>(atoi(argv[4])), false);
	train_set.print_infos(0);
	dev_set.print_infos(1);

	int systeme = atoi(argv[9]);
	
	// Build LSTM -------------------------------------------------------------------------------------
	if(systeme < 3 || systeme == 5)
	{
		cerr << "** SYSTEM " << systeme << " **\n";
		LSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), strtof(argv[6], NULL), static_cast<unsigned>(systeme), model);
		run_train(rnn, model, train_set, dev_set, embedding, argv[11], static_cast<unsigned>(atoi(argv[7])), static_cast<unsigned>(atoi(argv[8])));
	}

	else
	{
		cerr << "** SYSTEM KIM **\n";
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), strtof(argv[6], NULL), static_cast<unsigned>(systeme), model);
		run_train(rnn, model, train_set, dev_set, embedding, argv[11], static_cast<unsigned>(atoi(argv[7])), static_cast<unsigned>(atoi(argv[8])) );
	}
*/
	return 0;
}

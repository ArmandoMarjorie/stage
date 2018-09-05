#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include "../modele/LSTM.hpp"
#include "../modele/BILSTM.hpp"

#define SERVER_PORT htons(50007)

using namespace std;
using namespace dynet;

void usage(char* exe_name) 
{
	cerr << "\n**USAGE**\n\t" << exe_name << " test_file embedding_file \
			nb_layers input_dim hidden_dim parameters_file \
			system random_words\n\n"
		 << "test_file <string> : test file \
			(Files/files_test/testing_token_id_texts)\n" 
		 << "embedding_file <string> : file containing word embeddings \
			output by the training step\n"
		 << "nb_layers <int> : number of layers\n"                                                   
		 << "input_dim <int> : word embeddings dimension\n"                                   		 
		 << "hidden_dim <int> : hidden states dimension\n"
		 << "parameters_file <string> : file containing the parameters \
			(weight and bias) updated in the training step\n"
		 << "system <int> : which system you want to use (1, 2 or 3)\n"
		 << "explanation mode <int> : 1 for the original LIME, else 0.\n"
		 << "[optionnal] random_words <int> : words replaced by random\
			words (1) or not (0). Words are not replaced with LIME so \
			ignore this argument in this case.";
	exit(EXIT_SUCCESS);
}

void error(const string& message)
{
	cerr << message;
	exit(EXIT_FAILURE);
}

bool mode_rand_words(char* arg_rand_words)
{
	bool random_words = false;
	if(arg_rand_words != NULL)
	{
		cout << "MODIFIED LIME MODE\n";
		random_words = (atoi(arg_rand_words)==1);
		if(random_words)
			cout << "RANDOM WORDS MODE\n";
		else
			cout << "ANNOTATED WORDS MODE\n";
	}
	else
		cout << "ORIGINAL LIME MODE\n";
	return random_words;
}

void init_local_adrr(sockaddr_in& server_adrr)
{
	server_adrr.sin_family = AF_INET;
	server_adrr.sin_port = SERVER_PORT;
	server_adrr.sin_addr.s_addr = INADDR_ANY;		
}

void handling_instance(Data* data_copy, DataSet& set, bool new_instance, 
	bool random_words, unsigned num_sample, char* buffer_in, original_LIME)
{
	// If this is a new instance : save it 
	if(new_instance)
	{
		cout << "copy data...\n";
		data_copy = new Data( *(set.get_data_object(num_sample)) );
		cout << "data copied.\n";
	}
	// Else : modify it (LIME removes/replaces words)
	else
	{
		if(random_words)
			set.modif_LIME_random(buffer_in, num_sample);
		else
			set.modif_LIME(buffer_in, num_sample, original_LIME);
	}
}

string probas_to_string(vector<float>& probas)
{
	string buffer_out;
	for(unsigned i=0; i<probas.size(); ++i)
	{
		buffer_out += to_string(probas[i]);
		if(i != probas.size()-1)
			buffer_out += " ";
	}
	cout << "We're sending : \"" << buffer_out <<"\"\n\n"<<endl;
	return buffer_out;
}

void run_server(RNN& rnn, DataSet& set, Embeddings& embedding, 
	bool random_words, int client_socket, int server_socket, bool original_LIME)
{
	char buffer_in[5000] = {0}; // what is received
	int n;	
	unsigned num_sample=0;
	bool print_sample=true, running = true;
	Data* data_copy = NULL;
	while(running)
	{
		if(print_sample)
			cerr << "\n*** SAMPLE NUMERO " << num_sample << endl;
		bzero(buffer_in, 5000);
		
		// Receiving an instance
		n = recv(client_socket, buffer_in, 4999, 0);
		if( n == -1 )
			error("Error receiving message from the client : " 
				+ std::to_string(errno) + "\n");	
		cout << "We received in buffer_in: \n\t\"" 
			 << buffer_in << "\"" << endl;
		
		// If the current instance has been entirely processed 
		if( !strcmp(buffer_in, "-1") )
		{
			cout << "We received in buffer_in: \n\t" 
				 << buffer_in << endl;
			n = write(client_socket, "-1", 3);
			if( n == -1)
				error("Error sending message to the client : " + 
					std::to_string(errno) + "\n");	
			++num_sample;
			print_sample = true;
			data_copy->~Data();
			data_copy = NULL;
			continue;
		}
		// If all the instances has been entirely processed
		else if( !strcmp(buffer_in, "quit") )
		{
			close(server_socket);
			set.~DataSet();
			running = false;
			continue;
		}	

		// Modifies the instance according to what LIME wants to remove
		// and makes a prediction
		handling_instance(data_copy, set, print_sample, 
			random_words, num_sample, buffer_in, original_LIME);		
		vector<float> probas = run_predict_for_server_lime
			(rnn, set, embedding, num_sample); 
		
		// if it's not the original instance's prediction we reset it 
		// to modify it later according to what LIME wants to remove
		if(!print_sample)
		{
			cout << "Reset original instance...\n";
			set.reset_data(*data_copy, num_sample);
			cout << "The original instance has been reset...\n";
		}
		
		// converts the probas vector in the "buffer_out" to send it
		string buffer_out = probas_to_string(probas);
		n = write(client_socket, buffer_out.c_str(), buffer_out.size());
		if( n == -1)
			error("Error sending message to the client : " 
				+ std::to_string(errno) + "\n");
									
		print_sample = false; //it's not a new instance anymore
	}
}

int main(int argc, char** argv) 
{	
	if(argc > 1 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc < 8 || argc > 9 )
		error("See \"" + string(argv[0]) + " -h\" for the right command\n");
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);
	ParameterCollection model;		
	srand(time(NULL));
	
	// Socket
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
		error("Error creating socket : " + std::to_string(errno) + "\n");	
	sockaddr_in server_adrr;
	init_local_adrr(server_adrr);
	int opt = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		error("Error setsockopt : " + std::to_string(errno) + "\n");	
	if( bind(server_socket, (struct sockaddr*)&server_adrr, sizeof(struct sockaddr)) == -1 )
		error("Error assigning address : " + std::to_string(errno) + "\n");
	int client_socket;
	int len = sizeof(server_adrr);
	cout << "Waiting a client ...\n";
	if ( listen(server_socket, 1) != 0)
		error("Error listen : " + std::to_string(errno) + "\n");
	client_socket = accept(server_socket, (struct sockaddr*)&server_adrr, (socklen_t*)&len);
	if(client_socket == -1)
		error("Error accepting the client : " + std::to_string(errno) + "\n");
	cout << "Connection OK\n";
	
	
	// Init RNN, dataset, ...
	bool random_words = mode_rand_words(argv[9]);
	bool original_LIME = (atoi(argv[8])==1);
	Embeddings embedding(argv[2], model, static_cast<unsigned>(atoi(argv[4])), true);
	unsigned systeme = static_cast<unsigned>(atoi(argv[7]));
	RNN* rnn;
	if(systeme < 3)
		rnn = new LSTM(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), 
		static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model, original_LIME);
	else
		rnn = new BiLSTM(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), 
		static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model, original_LIME);		
	DataSet set(argv[1], original_LIME);
	populate_from_file(argv[6], model);
 
	// Does the LIME method
	run_server(*rnn, set, embedding, random_words, client_socket, server_socket, original_LIME);

	return 0;
}


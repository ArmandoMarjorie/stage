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
#include "modele/LSTM.hpp"
#include "modele/BILSTM.hpp"


// FAIRE UNE DIFFERENCE ENTRE LIME ORIGINALE (0 est un mot)
// ET LE RESTE (0 veut dire pas de mot)

#define SERVER_PORT htons(50007)

using namespace std;
using namespace dynet;

void usage(char* exe_name) 
{
	cerr << "\n**USAGE**\n\t" << exe_name << " test_file embedding_file \
			nb_layers input_dim hidden_dim parameters_file \
			system random_words\n\n"
			
		 << "test_file <string> : file containing the number of words for each sentences from each test sample\n"
		 
		 << "embedding_file <string> : file containing word embeddings \
			output by the training step\n"
		 << "nb_layers <int> : number of layers\n"                                                   
		 << "input_dim <int> : word embeddings dimension\n"                                   		 
		 << "hidden_dim <int> : hidden states dimension\n"
		 << "parameters_file <string> : file containing the parameters \
			(weight and bias) updated in the training step\n"
		 << "system <int> : which system you want to use (1, 2 or 3)\n"
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

int create_socket()
{
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
	{
		//err = "Error socket : " + std::to_string(errno) + "\n";
		error("Error creating socket : " + std::to_string(errno) + "\n");
	}	
	return server_socket;
}

void init_local_adrr(sockaddr_in& server_adrr)
{
	server_adrr.sin_family = AF_INET;
	server_adrr.sin_port = SERVER_PORT;
	server_adrr.sin_addr.s_addr = INADDR_ANY;		
}

void connecting(int& server_socket, int& client_socket, sockaddr_in& server_adrr)
{
	int len = sizeof(server_adrr);
	cout << "Waiting a client ...\n";
	if ( listen(server_socket, 1) != 0)
	{
		//err = "Error listen : " + std::to_string(errno) + "\n";
		error("Error listen : " + std::to_string(errno) + "\n");		
	}
	client_socket = accept(server_socket, (struct sockaddr*)&server_adrr, (socklen_t*)&len);
	if(client_socket == -1)
	{
		//err = "Error accepting the client : " + std::to_string(errno) + "\n";
		error("Error accepting the client : " + std::to_string(errno) + "\n");
	}
	cout << "Connection OK\n";
}


void run_server(RNN& rnn, DataSet& set, Embeddings& embedding, 
	int& client_socket, int& server_socket)
{
	char buffer_in[5000] = {0}; // what is received
	char buffer_out[5000] = {0}; // what is sent
	string tmp;
	int n;	
	unsigned num_sample=0;
	bool print_sample=true, running = true;
	Data* data_copy = NULL;
	while(running)
	{
		if(print_sample)
			cerr << "\n*** SAMPLE NUMERO " << num_sample << endl;
		bzero(buffer_in, 5000);
		bzero(buffer_out, 5000);
		
		// Receiving an instance
		n = recv(client_socket, buffer_in, 4999, 0);
		if( n == -1 )
			error("Error receiving message from the client : " + std::to_string(errno) + "\n");	
		cout << "We received in buffer_in: \n\t\"" << buffer_in << "\"" << endl;
		
		// If the current instance has been entirely processed 
		if( !strcmp(buffer_in, "-1") )
		{
			cout << "We received in buffer_in: \n\t" << buffer_in << endl;
			n = write(client_socket, "-1", 3);
			if( n == -1)
				error("Error sending message to the client : " + std::to_string(errno) + "\n");	
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

		// If this is a new instance : save it 
		if(print_sample)
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
				set.modif_LIME(buffer_in, num_sample);
		}
		
		vector<float> probas = run_predict_for_server_lime
			(rnn, set, embedding, true, num_sample); 
		
		// if it's not the original instance's prediction
		if(!print_sample)
		{
			cout << "Reset original instance...\n";
			set.reset_data(*data_copy, num_sample);
			//set.print_a_sample(num_sample);
			cout << "The original instance has been reset...\n";
		}
		

		bzero(buffer_in, 5000);
		bzero(buffer_out, 5000);	
		
		// converts the probas vector in the char* "buffer_out"
		tmp = to_string(probas[0]);
		strcpy(buffer_out,tmp.c_str());
		for(unsigned i=1; i<probas.size(); ++i)
		{
			tmp = to_string(probas[i]);
			strcat(buffer_out," ");
			strcat(buffer_out,tmp.c_str());
		}
		cout << "We're sending : \"" <<buffer_out<<"\"\n\n"<<endl;
		
		n = write(client_socket, buffer_out, strlen(buffer_out));
		if( n == -1)
			error("Error sending message to the client : " + std::to_string(errno) + "\n");					
		print_sample = false;
	}
}


int main(int argc, char** argv) 
{	
	if(argc > 1 && !strcmp(argv[1], "-h"))
		usage(argv[0]);
	if( argc < 7 || argc > 8 )
	{
		cerr << "See \"" << argv[0] << " -h\" for the right command\n"; 
		exit(EXIT_FAILURE);
	}
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);
	ParameterCollection model;		
	srand(time(NULL));
	
	// Socket
	int server_socket = create_socket();	
	sockaddr_in server_adrr;
	init_local_adrr(server_adrr);
	int opt = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		error("Error setsockopt : " + std::to_string(errno) + "\n");	
	if( bind(server_socket, (struct sockaddr*)&server_adrr, sizeof(struct sockaddr)) == -1 )
		error("Error assigning address : " + std::to_string(errno) + "\n");
	int client_socket;
	connecting(server_socket, client_socket, server_adrr);
	
	
	// Init RNN, dataset, ...
	bool random_words = mode_rand_words(argv[8]);
	Embeddings embedding(argv[2], model, static_cast<unsigned>(atoi(argv[4])), true);
	unsigned systeme = static_cast<unsigned>(atoi(argv[7]));
	RNN* rnn;
	if(systeme < 3)
		rnn = new LSTM(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), 
		static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);
	else
		rnn = new BiLSTM(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), 
		static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);		
	DataSet set(argv[1]);
	populate_from_file(argv[6], model);
 
	// Does the LIME method
	run_server(*rnn, set, embedding, client_socket, server_socket);

	return 0;
}


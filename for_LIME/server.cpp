#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <string>
//#include "../modele/rnn.hpp"

using namespace std;
//using namespace dynet;

#define SERVER_PORT htons(50007)

void error(string message)
{
	cerr << message;
	exit(EXIT_FAILURE);
}

int main() 
{	
	/*// Fetch dynet params 
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);
	// Build model 
	ParameterCollection model;			 
	// Load Dataset 
	Embeddings embedding(argv[2], model, static_cast<unsigned>(atoi(argv[4])), true);
	int systeme = atoi(argv[7]);*/
	string err;
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
	{
		err = "Error socket : " + std::to_string(errno) + "\n";
		error(err);
	}

	sockaddr_in server_adrr;
	int len = sizeof(server_adrr);
	int opt = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		err = "Error setsockopt : " + std::to_string(errno) + "\n";
		error(err);		
	}
	
	server_adrr.sin_family = AF_INET;
	server_adrr.sin_port = SERVER_PORT;
	server_adrr.sin_addr.s_addr = INADDR_ANY;

	/* bind (this socket, local address, address length)
	   bind server socket (serverSock) to server address (serverAddr).  
	   Necessary so that server can use a specific port */ 
	if( bind(server_socket, (struct sockaddr*)&server_adrr, sizeof(struct sockaddr)) == -1 )
	{
		err = "Error assigning address : " + std::to_string(errno) + "\n";
		error(err);
	}

	cout << "Waiting a client ...\n";
	
	// wait for a client
	/* listen (this socket, request queue length) */
	if ( listen(server_socket, 1) != 0)
	{
		err = "Error listen : " + std::to_string(errno) + "\n";
		error(err);		
	}
	int client_socket = accept(server_socket, (struct sockaddr*)&server_adrr, (socklen_t*)&len);
	if(client_socket == -1)
	{
		err = "Error accepting the client : " + std::to_string(errno) + "\n";
		error(err);
	}
	cout << "Connection OK\n";
	
	char buffer_in[1000] = {0}; // what is received
	char buffer_out[1000]; // what is sent
	int n = recv(client_socket, buffer_in, 199, 0);
	cout << "Server received :\n" << buffer_in << endl;
	
	/*
	strcpy(buffer_out, "je t'envoie un message !");
	int n = write(client_socket, buffer_out, strlen(buffer_out));*/
	close(server_socket);
	
	
	
	/*
	if(systeme < 3)
	{
		LSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);
		// The Code Here !! asking predict here !! 
		while(!strcmp(buffer_in, "quit"))
		{
			bzero(buffer_in, 8000);
			
			//receive a message from a client
			n = read(client_socket, buffer_in, 8000);
			cout << "Server received :\n" << buffer_in << endl;
			//tokeniser buffer_in, en faire 2 phrases comme ceci:
			// mots premisse tokénisés -1 longueur prem (idem pour l'hypothese)
			// faire un objet data avec ça (osef du label)
			// faire la prediction
			// renvoyer les probas de chaque label

			strcpy(buffer_out, "test");
			n = write(client_socket, buffer_out, strlen(buffer_out));
		}
						//Data explication_set(argv[1], 3); 
						//run_predict_removing_couple(rnn, model, explication_set, embedding, argv[6]);
	}*/
	/*else
	{
		Data test_set(argv[1]);
		test_set.print_infos(3);
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[3])), static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 0, static_cast<unsigned>(systeme), model);
		run_predict(rnn, model, test_set, embedding, argv[6]);
	}*/


	return 0;
}

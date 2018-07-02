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


#define SERVER_PORT htons(50007)

using namespace std;
using namespace dynet;


void usage(char* exe_name) 
{
	cerr << "\n**USAGE**\n\t" << exe_name << " test_file embedding_file nb_layers input_dim hidden_dim parameters_file system\n\n"
		 << "length_file <string> : file containing the number of words for each sentences from each test sample\n"
		 << "lexique_file : file containing the IDs of each word of the vocabulary\n"
		 << "embedding_file <string> : file containing word embeddings used in the training step\n"
		 << "nb_layers <int> : number of layers\n"
		 << "input_dim <int> : dimension of the word embedding\n"
		 << "hidden_dim <int> : dimension  of the hidden states ht and ct\n"
		 << "parameters_file <string> : file containing the parameters (weight and bias) updated in the training step\n"
		 << "system <int> : which system you want to use (1 (LSTM), 2(LSTM), 3=KIM(BILSTM), or 4(BILSTM))\n";
	exit(EXIT_SUCCESS);
}

void error(string message)
{
	cerr << message;
	exit(EXIT_FAILURE);
}

void init_map(char* lexique_filename, map<string, unsigned>& word_to_id)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}	
	cerr << "Reading " << lexique_filename << " ...\n";
	unsigned id;
	string word;
	while(lexique_file >> word && lexique_file >> id)
		word_to_id[word] = id;
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
}

void init_lenght_tab(char* length_filename, vector<unsigned>& length_tab)
{
	ifstream file(length_filename, ios::in);
	if(!file) 
	{ 
		cerr << "Impossible to open the file " << length_filename << endl;
		exit(EXIT_FAILURE);
	}		
	unsigned val;
	while(file >> val)
	{
		length_tab.push_back(val);
	}
}


int main(int argc, char** argv) 
{	
	
	/*if(argc > 0 && !strcmp(argv[1], "-h"))
		usage(argv[0]);*/
	/*if( argc != 9 )
	{
		cerr << "Usage :\n " 
		<< argv[0] << " length_file " << " lexique_file " << " embedding_file " 
		<<" nb_layers " << " input_dim " << " hidden_dim " << " model_file " << " systeme \n"
		<< argv[0] << " -h for more info\n";
		exit(EXIT_FAILURE);
	}*/
	
	
	// Fetch dynet params 
	auto dyparams = dynet::extract_dynet_params(argc, argv);
	dynet::initialize(dyparams);
	// Build model 
	ParameterCollection model;		
	
	DataSet dataset(argv[1]);
	exit(EXIT_SUCCESS);
	
	
	
	
		 
	// Load Dataset 
	Embeddings embedding(argv[3], model, static_cast<unsigned>(atoi(argv[5])), true);
	int systeme = atoi(argv[8]);
	
	
	map<string, unsigned> word_to_id;
	init_map(argv[2], word_to_id);
	vector<unsigned> length_tab;
	init_lenght_tab(argv[1], length_tab);

	
	// Socket
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
	
	/*
	char buffer_in[10000] = {0}; // what is received
	char buffer_out[1000] = {0}; // what is sent
	int n = recv(client_socket, buffer_in, 9999, 0);
	cout << "Server received :\n" << buffer_in << endl;
	*/
	/*
	strcpy(buffer_out, "je t'envoie un message !");
	int n = write(client_socket, buffer_out, strlen(buffer_out));*/
	//close(server_socket);
	
	string tmp;
	bool print_label=true;
	if(systeme < 3)
	{
		LSTM rnn(static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), 
			static_cast<unsigned>(atoi(argv[6])), 0, static_cast<unsigned>(systeme), model);
		
		// Load preexisting weights
		cerr << "Loading parameters ...\n";
		TextFileLoader loader(argv[7]);
		loader.populate(model);
		cerr << "Parameters loaded !\n";
	
		char buffer_in[5000] = {0}; // what is received (sentence)
		char buffer_out[5000] = {0}; // what is sent	
		char buffer_in_bis[5000] = {0}; // what is received (sentence)	
		unsigned num_sample=0, rcv_finish=0;
		int n;
		bool is_premise, recevoir_prem_or_not=true;
		// The Code Here !! asking predict here !! 
		while(strcmp(buffer_in, "quit"))
		{
			if(print_label)
				cerr << "*** SAMPLE NUMERO " << num_sample << endl;
			bzero(buffer_in, 5000);
			bzero(buffer_out, 5000);
			bzero(buffer_in_bis, 5000);
			
			//recevoir si c'est prem ou hyp
			
			if(recevoir_prem_or_not)
			{
				n = recv(client_socket, buffer_in, 4999, 0);
				if( n == -1 )
				{
					err = "Error receiving message from the client : " + std::to_string(errno) + "\n";
					error(err);		
				}		
				//cout << "prem ou hyp ? : " << buffer_in << endl;
				if(!strcmp(buffer_in,"prem"))
					is_premise = true;
				else
					is_premise = false;
				n = write(client_socket, "ok", 3); //envoie "bien recu"
				if(n == -1)
				{
					err = "Error sending message to the client : " + std::to_string(errno) + "\n";
					error(err);					
				}
				//cout << "on envoit OK\n";			
				bzero(buffer_in, 5000);
				bzero(buffer_out, 5000);
				recevoir_prem_or_not = false;
			}
			
			/*if(!strcmp(tmp,"0"))
				continue;*/
			
			//receive a premise from a client
			n = recv(client_socket, buffer_in, 4999, 0); //recoit prem
			if( n == -1 )
			{
				err = "Error receiving message from the client : " + std::to_string(errno) + "\n";
				error(err);		
			}
			//cout << "on a recu dans buffer_in: \n\t" << buffer_in << endl;
			if( !strcmp(buffer_in, "-1") )
			{
				recevoir_prem_or_not=true;

				n = write(client_socket, "-1", 3);
				if( n == -1)
				{
					err = "Error sending message to the client : " + std::to_string(errno) + "\n";
					error(err);					
				}
				//doit recevoir les mots ici et doit les traiter ...
				/*if(is_premise)
					imp_words_max = length_tab[num_sample*2];
				else
					imp_words_max = length_tab[num_sample*2+1];
					
				for(unsigned nb_imp_words=0; nb_imp_words < imp_words_max; ++nb_imp_words)
				{
					bzero(buffer_in, 5000);
					cout << "mot important = " << buffer_in << endl;
					traitement_mots(data, buffer_in); //il faut les mots importants dans data !
					
					n = write(client_socket, "ok", 3);
					if( n == -1)
					{
						err = "Error sending message to the client : " + std::to_string(errno) + "\n";
						error(err);					
					}	
				}		*/
				// .....	
				++rcv_finish; 
				if(rcv_finish==2)
				{
					rcv_finish=0;
					++num_sample;
				}
				continue;
			}
			else if( !strcmp(buffer_in, "quit") )
			{
				close(server_socket);
				exit(EXIT_SUCCESS);
			}
			
			
			n = write(client_socket, "ok", 3); //envoie "bien recu"
			if(n == -1)
			{
				err = "Error sending message to the client : " + std::to_string(errno) + "\n";
				error(err);					
			}
			//cout << "on envoit OK\n";
			
			n = recv(client_socket, buffer_in_bis, 4999, 0); //recoit hyp
			if( n == -1 )
			{
				err = "Error receiving message from the client : " + std::to_string(errno) + "\n";
				error(err);		
			}
			//cout << "on a recu dans buffer_in_bis : \n\t" << buffer_in_bis << endl;
			n = write(client_socket, "ok", 3); //envoie "bien recu"
			if(n == -1)
			{
				err = "Error sending message to the client : " + std::to_string(errno) + "\n";
				error(err);					
			}
			//cout << "on envoit OK\n";			
			

			Data data(buffer_in, word_to_id, buffer_in_bis, num_sample, is_premise); //nouveau data
			bzero(buffer_in, 5000);
			bzero(buffer_out, 5000);
			bzero(buffer_in_bis, 5000);	
			//cout << "data OK\n";		
			data.print_sentences_of_a_sample(0);
			
			//data.print_sentences_of_a_sample(0);
			vector<float> probas = run_predict_for_server_lime(rnn, data, embedding, print_label);
			//cout << "prediction \n";
			tmp = to_string(probas[0]);
			strcpy(buffer_out,tmp.c_str());
			for(unsigned i=1; i<probas.size(); ++i)
			{
				tmp = to_string(probas[i]);
				strcat(buffer_out," ");
				strcat(buffer_out,tmp.c_str());
			}
			//cout << "on va envoyer = \"" <<buffer_out<<"\""<<endl;
			n = write(client_socket, buffer_out, strlen(buffer_out));
			if( n == -1)
			{
				err = "Error sending message to the client : " + std::to_string(errno) + "\n";
				error(err);					
			}
			//cout << "envoyer !" << endl;
		}
		
	}
	else
	{
		BiLSTM rnn(static_cast<unsigned>(atoi(argv[4])), static_cast<unsigned>(atoi(argv[5])), static_cast<unsigned>(atoi(argv[6])), 0, static_cast<unsigned>(systeme), model);
		// unsigned s, ParameterCollection& model
		
		// Load preexisting weights
		cerr << "Loading parameters ...\n";
		TextFileLoader loader(argv[7]);
		loader.populate(model);
		cerr << "Parameters loaded !\n";
	
		char buffer_in[5000] = {0}; // what is received (sentence)
		char buffer_out[5000] = {0}; // what is sent	
		char buffer_in_bis[5000] = {0}; // what is received (sentence)	
		unsigned num_sample=0, rcv_finish=0;
		int n;
		bool is_premise, recevoir_prem_or_not=true;
		// The Code Here !! asking predict here !! 
		while(strcmp(buffer_in, "quit"))
		{
			if(print_label)
				cerr << "*** SAMPLE NUMERO " << num_sample << endl;
			bzero(buffer_in, 5000);
			bzero(buffer_out, 5000);
			bzero(buffer_in_bis, 5000);
			
			//recevoir si c'est prem ou hyp
			
			if(recevoir_prem_or_not)
			{
				n = recv(client_socket, buffer_in, 4999, 0);
				if( n == -1 )
				{
					err = "Error receiving message from the client : " + std::to_string(errno) + "\n";
					error(err);		
				}		
				//cout << "prem ou hyp ? : " << buffer_in << endl;
				if(!strcmp(buffer_in,"prem"))
					is_premise = true;
				else
					is_premise = false;
				n = write(client_socket, "ok", 3); //envoie "bien recu"
				if(n == -1)
				{
					err = "Error sending message to the client : " + std::to_string(errno) + "\n";
					error(err);					
				}
				//cout << "on envoit OK\n";			
				bzero(buffer_in, 5000);
				bzero(buffer_out, 5000);
				recevoir_prem_or_not = false;
			}
			
			/*if(!strcmp(tmp,"0"))
				continue;*/
			
			//receive a premise from a client
			n = recv(client_socket, buffer_in, 4999, 0); //recoit prem
			if( n == -1 )
			{
				err = "Error receiving message from the client : " + std::to_string(errno) + "\n";
				error(err);		
			}
			//cout << "on a recu dans buffer_in: \n\t" << buffer_in << endl;
			if( !strcmp(buffer_in, "-1") )
			{
				recevoir_prem_or_not=true;

				n = write(client_socket, "-1", 3);
				if( n == -1)
				{
					err = "Error sending message to the client : " + std::to_string(errno) + "\n";
					error(err);					
				}
				//doit recevoir les mots ici et doit les traiter ...
				/*if(is_premise)
					imp_words_max = length_tab[num_sample*2];
				else
					imp_words_max = length_tab[num_sample*2+1];
					
				for(unsigned nb_imp_words=0; nb_imp_words < imp_words_max; ++nb_imp_words)
				{
					bzero(buffer_in, 5000);
					cout << "mot important = " << buffer_in << endl;
					traitement_mots(data, buffer_in); //il faut les mots importants dans data !
					
					n = write(client_socket, "ok", 3);
					if( n == -1)
					{
						err = "Error sending message to the client : " + std::to_string(errno) + "\n";
						error(err);					
					}	
				}		*/
				// .....	
				++rcv_finish; 
				if(rcv_finish==2)
				{
					rcv_finish=0;
					++num_sample;
				}
				continue;
			}
			else if( !strcmp(buffer_in, "quit") )
			{
				close(server_socket);
				exit(EXIT_SUCCESS);
			}
			
			
			n = write(client_socket, "ok", 3); //envoie "bien recu"
			if(n == -1)
			{
				err = "Error sending message to the client : " + std::to_string(errno) + "\n";
				error(err);					
			}
			//cout << "on envoit OK\n";
			
			n = recv(client_socket, buffer_in_bis, 4999, 0); //recoit hyp
			if( n == -1 )
			{
				err = "Error receiving message from the client : " + std::to_string(errno) + "\n";
				error(err);		
			}
			//cout << "on a recu dans buffer_in_bis : \n\t" << buffer_in_bis << endl;
			n = write(client_socket, "ok", 3); //envoie "bien recu"
			if(n == -1)
			{
				err = "Error sending message to the client : " + std::to_string(errno) + "\n";
				error(err);					
			}
			//cout << "on envoit OK\n";			
			

			Data data(buffer_in, word_to_id, buffer_in_bis, num_sample, is_premise); //nouveau data
			bzero(buffer_in, 5000);
			bzero(buffer_out, 5000);
			bzero(buffer_in_bis, 5000);	
			//cout << "data OK\n";		
			data.print_sentences_of_a_sample(0);
			
			//data.print_sentences_of_a_sample(0);
			vector<float> probas = run_predict_for_server_lime(rnn, data, embedding, print_label);
			//cout << "prediction \n";
			tmp = to_string(probas[0]);
			strcpy(buffer_out,tmp.c_str());
			for(unsigned i=1; i<probas.size(); ++i)
			{
				tmp = to_string(probas[i]);
				strcat(buffer_out," ");
				strcat(buffer_out,tmp.c_str());
			}
			//cout << "on va envoyer = \"" <<buffer_out<<"\""<<endl;
			n = write(client_socket, buffer_out, strlen(buffer_out));
			if( n == -1)
			{
				err = "Error sending message to the client : " + std::to_string(errno) + "\n";
				error(err);					
			}
			//cout << "envoyer !" << endl;
		}
	}


	return 0;
}

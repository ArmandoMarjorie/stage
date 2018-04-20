#include <iostream>
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

using namespace std;

#define SERVER_PORT htons(50007)

void error(string message)
{
	cerr << message;
	exit(EXIT_FAILURE);
}

int main() 
{
	string err;
	
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_ == -1)
	{
		err = "Error socket : " + to_string(errno) + "\n";
		error(err);
	}

	sockaddr_in server_adrr, client_addr;
	server_adrr.sin_family = AF_INET;
	server_adrr.sin_port = SERVER_PORT;
	server_adrr.sin_addr.s_addr = INADDR_ANY;

	/* bind (this socket, local address, address length)
	   bind server socket (serverSock) to server address (serverAddr).  
	   Necessary so that server can use a specific port */ 
	if( bind(server_socket, (struct sockaddr*)&server_adrr, sizeof(struct sockaddr)) == -1 )
	{
		err = "Error assigning address : " + to_string(errno) + "\n";
		error(err);
	}

	cout << "Waiting a client ...\n";
	
	// wait for a client
	/* listen (this socket, request queue length) */
	if ( listen(server_socket, 1) != 0)
	{
		err = "Error listen : " + to_string(errno) + "\n";
		error(err);		
	}
	int len = sizeof(client_adrr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);
	if(client_socket == -1)
	{
		err = "Error accepting the client : " + to_string(errno) + "\n";
		error(err);
	}
	char buff[INET6_ADDRSTRLEN] = {0};
	string clientAddress = inet_ntop(client_adrr.sin_family, (void*)&(client_adrr.sin_addr), buff, INET6_ADDRSTRLEN);
	cout << "Connection from " << clientAddress.c_str() << " : " << client_adrr.sin_port << endl;
	
	char buffer_in[1000]; // what is received
	char buffer_out[1000]; // what is sent
	
	/* The Code Here !! asking predict here !! */
	while(!strcmp(buffer_in, "quit"))
	{
		bzero(buffer, 1000);
		
		//receive a message from a client
		n = read(client_socket, buffer_in, 1000);
		cout << "Server received:  " << buffer_in << endl;

		strcpy(buffer_out, "test");
		n = write(client_socket, buffer_out, strlen(buffer_out));
	}
	return 0;
}

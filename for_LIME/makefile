CC=g++
#CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/storage/raid1/homedirs/marjorie.armando/lib/dynet -I/storage/raid1/homedirs/marjorie.armando/lib/eigen
#LDFLAGS=-L/storage/raid1/homedirs/marjorie.armando/lib/dynet/build/dynet -ldynet
#CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/home/marjorie/Documents/LIB/dynet -I/home/marjorie/Documents/LIB/eigen `pkg-config --cflags gtkmm-3.0`
#LDFLAGS=-L/home/marjorie/Documents/LIB/dynet/build/dynet -ldynet `pkg-config --libs gtkmm-3.0`

CFLAGS= -g -W -Wall -std=c++11 -pedantic -O3 -I/home/marjorie/Documents/LIB/dynet -I/home/marjorie/Documents/LIB/eigen
LDFLAGS=-L/home/marjorie/Documents/LIB/dynet/build/dynet -ldynet
EXEC= Exe_Serveur

all: $(EXEC)

Exe_Serveur: server.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o bow.o
	$(CC) -o Exe_Serveur server.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o bow.o $(LDFLAGS)

server.o: server.cpp modele/LSTM.hpp modele/BILSTM.hpp code_data/dataset.hpp 
	$(CC) -o server.o -c server.cpp $(CFLAGS)
	
rnn.o: modele/rnn.cpp modele/rnn.hpp
	$(CC) -o rnn.o -c modele/rnn.cpp $(CFLAGS)
	
LSTM.o: modele/LSTM.cpp modele/LSTM.hpp
	$(CC) -o LSTM.o -c modele/LSTM.cpp $(CFLAGS)
	
BILSTM.o: modele/BILSTM.cpp modele/BILSTM.hpp
	$(CC) -o BILSTM.o -c modele/BILSTM.cpp $(CFLAGS)

data.o: code_data/data.cpp code_data/data.hpp 
	$(CC) -o data.o -c code_data/data.cpp $(CFLAGS)
	
dataset.o: code_data/dataset.cpp code_data/dataset.hpp 
	$(CC) -o dataset.o -c code_data/dataset.cpp $(CFLAGS)
	
embedding.o: code_data/embedding.cpp code_data/embedding.hpp
	$(CC) -o embedding.o -c code_data/embedding.cpp $(CFLAGS)
	
sw.o: code_data/switch_words.cpp code_data/switch_words.hpp
	$(CC) -o sw.o -c code_data/switch_words.cpp $(CFLAGS)
	
bow.o: code_data/bow.cpp code_data/bow.hpp
	$(CC) -o bow.o -c code_data/bow.cpp $(CFLAGS)
	


clean:
	rm -rf *.o

clean_weight:
	rm -rf *.params

clean_embedding:
	rm -rf *.emb

rmproper: clean
	rm -rf $(EXEC) 

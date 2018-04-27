CC=g++
#CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/storage/raid1/homedirs/marjorie.armando/lib/dynet -I/storage/raid1/homedirs/marjorie.armando/lib/eigen
#LDFLAGS=-L/storage/raid1/homedirs/marjorie.armando/lib/dynet/build/dynet -ldynet
CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/home/marjorie/Documents/LIB/dynet -I/home/marjorie/Documents/LIB/eigen
LDFLAGS=-L/home/marjorie/Documents/LIB/dynet/build/dynet -ldynet
EXEC= Exe_Serveur

all: $(EXEC)

Exe_Serveur: server.o rnn.o LSTM.o BILSTM.o data.o embedding.o couple.o
	$(CC) -o Exe_Serveur server.o rnn.o LSTM.o BILSTM.o data.o embedding.o couple.o $(LDFLAGS)

server.o: server.cpp ../modele/LSTM.hpp ../modele/BILSTM.hpp
	$(CC) -c server.cpp $(CFLAGS)
	
rnn.o: ../modele/rnn.cpp ../modele/rnn.hpp
	$(CC) -c ../modele/rnn.cpp $(CFLAGS)
	
LSTM.o: ../modele/LSTM.cpp ../modele/LSTM.hpp
	$(CC) -c ../modele/LSTM.cpp $(CFLAGS)
	
BILSTM.o: ../modele/BILSTM.cpp ../modele/BILSTM.hpp
	$(CC) -c ../modele/BILSTM.cpp $(CFLAGS)

data.o: ../code_data/data.cpp ../code_data/data.hpp 
	$(CC) -c ../code_data/data.cpp $(CFLAGS)
	
embedding.o: ../code_data/embedding.cpp ../code_data/embedding.hpp
	$(CC) -c ../code_data/embedding.cpp $(CFLAGS)

couple.o: ../code_data/couple.cpp ../code_data/couple.hpp
	$(CC) -c ../code_data/couple.cpp $(CFLAGS)


clean:
	rm -rf *.o

clean_weight:
	rm -rf *.params

clean_embedding:
	rm -rf *.emb

rmproper: clean
	rm -rf $(EXEC) 

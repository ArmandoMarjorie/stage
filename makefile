CC=g++
CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/home/marjorie/Documents/LIB/dynet -I/home/marjorie/Documents/LIB/eigen
LDFLAGS=-L/home/marjorie/Documents/LIB/dynet/build/dynet -ldynet
EXEC= Training Testing ServeurLime Baxi

all: $(EXEC)

Training: train.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o 
	$(CC) -o Training train.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o $(LDFLAGS)

Testing: predict.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o
	$(CC) -o Testing predict.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o $(LDFLAGS)
	
Baxi : BAXI.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o
	$(CC) -o Baxi BAXI.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o $(LDFLAGS)

ServeurLime : serveurLime.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o
	$(CC) -o ServeurLime serveurLime.o rnn.o LSTM.o BILSTM.o data.o embedding.o sw.o dataset.o instance_expression.o $(LDFLAGS)

predict.o: run/predict.cpp modele/rnn.hpp code_data/dataset.hpp 
	$(CC) -o predict.o -c run/predict.cpp $(CFLAGS)

train.o: run/train.cpp modele/rnn.hpp code_data/dataset.hpp 
	$(CC) -o train.o -c run/train.cpp $(CFLAGS)
	
serveurLime.o: run/serveur_lime.cpp modele/rnn.hpp code_data/dataset.hpp 
	$(CC) -o serveurLime.o -c run/serveur_lime.cpp $(CFLAGS)
	
	
	
	
	

rnn.o: modele/rnn.cpp modele/rnn.hpp code_data/dataset.hpp
	$(CC) -o rnn.o -c modele/rnn.cpp $(CFLAGS)
	
LSTM.o: modele/LSTM.cpp modele/LSTM.hpp code_data/dataset.hpp modele/rnn.hpp
	$(CC) -o LSTM.o -c modele/LSTM.cpp $(CFLAGS)
	
BILSTM.o: modele/BILSTM.cpp modele/BILSTM.hpp code_data/dataset.hpp modele/rnn.hpp
	$(CC) -o BILSTM.o -c modele/BILSTM.cpp $(CFLAGS)

data.o: code_data/data.cpp code_data/data.hpp 
	$(CC) -o data.o -c code_data/data.cpp $(CFLAGS)
	
dataset.o: code_data/dataset.cpp code_data/dataset.hpp 
	$(CC) -o dataset.o -c code_data/dataset.cpp $(CFLAGS)
	
embedding.o: code_data/embedding.cpp code_data/embedding.hpp
	$(CC) -o embedding.o -c code_data/embedding.cpp $(CFLAGS)
	
sw.o: code_data/switch_words.cpp code_data/switch_words.hpp
	$(CC) -o sw.o -c code_data/switch_words.cpp $(CFLAGS)
	
instance_expression.o: code_data/instance_expression.cpp code_data/instance_expression.hpp
	$(CC) -o instance_expression.o -c code_data/instance_expression.cpp $(CFLAGS)

BAXI.o: run/BAXI.cpp modele/BAXI.hpp
	$(CC) -o BAXI.o -c run/BAXI.cpp $(CFLAGS)
	
clean:
	rm -rf *.o

clean_weight:
	rm -rf *.params

clean_embedding:
	rm -rf *.emb

rmproper: clean
	rm -rf $(EXEC) 

	

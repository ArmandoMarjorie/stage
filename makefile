CC=g++
#CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/storage/raid1/homedirs/marjorie.armando/lib/dynet -I/storage/raid1/homedirs/marjorie.armando/lib/eigen
#LDFLAGS=-L/storage/raid1/homedirs/marjorie.armando/lib/dynet/build/dynet -ldynet
CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/home/marjorie/Documents/LIB/dynet -I/home/marjorie/Documents/LIB/eigen
LDFLAGS=-L/home/marjorie/Documents/LIB/dynet/build/dynet -ldynet
EXEC= Training Testing

all: $(EXEC)

Training: train.o rnn.o LSTM.o BILSTM.o data.o embedding.o couple.o detoken.o
	$(CC) -o Training train.o rnn.o LSTM.o BILSTM.o data.o embedding.o couple.o detoken.o $(LDFLAGS)

Testing: predict.o rnn.o LSTM.o BILSTM.o data.o embedding.o couple.o detoken.o
	$(CC) -o Testing predict.o rnn.o LSTM.o BILSTM.o data.o embedding.o couple.o detoken.o $(LDFLAGS)

predict.o: run/predict.cpp modele/rnn.hpp code_data/data.hpp
	$(CC) -o predict.o -c run/predict.cpp $(CFLAGS)

train.o: run/train.cpp modele/rnn.hpp code_data/data.hpp
	$(CC) -o train.o -c run/train.cpp $(CFLAGS)

rnn.o: modele/rnn.cpp modele/rnn.hpp code_data/data.hpp tokenizer/detoken_explication.hpp
	$(CC) -o rnn.o -c modele/rnn.cpp $(CFLAGS)
	
LSTM.o: modele/LSTM.cpp modele/LSTM.hpp code_data/data.hpp modele/rnn.hpp
	$(CC) -o LSTM.o -c modele/LSTM.cpp $(CFLAGS)
	
BILSTM.o: modele/BILSTM.cpp modele/BILSTM.hpp code_data/data.hpp modele/rnn.hpp
	$(CC) -o BILSTM.o -c modele/BILSTM.cpp $(CFLAGS)

data.o: code_data/data.cpp code_data/data.hpp code_data/embedding.hpp code_data/couple.hpp
	$(CC) -o data.o -c code_data/data.cpp $(CFLAGS)
	
embedding.o: code_data/embedding.cpp code_data/embedding.hpp
	$(CC) -o embedding.o -c code_data/embedding.cpp $(CFLAGS)

couple.o: code_data/couple.cpp code_data/couple.hpp
	$(CC) -o couple.o -c code_data/couple.cpp $(CFLAGS)

detoken.o: tokenizer/detoken_explication.cpp tokenizer/detoken_explication.hpp
	$(CC) -o detoken.o -c tokenizer/detoken_explication.cpp $(CFLAGS)
	
clean:
	rm -rf *.o

clean_weight:
	rm -rf *.params

clean_embedding:
	rm -rf *.emb

rmproper: clean
	rm -rf $(EXEC) 

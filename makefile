CC=g++
#CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/storage/raid1/homedirs/marjorie.armando/lib/dynet -I/storage/raid1/homedirs/marjorie.armando/lib/eigen
#LDFLAGS=-L/storage/raid1/homedirs/marjorie.armando/lib/dynet/build/dynet -ldynet
CFLAGS=-W -Wall -std=c++11 -pedantic -O3 -I/home/marjorie/Documents/LIB/dynet -I/home/marjorie/Documents/LIB/eigen
LDFLAGS=-L/home/marjorie/Documents/LIB/dynet/build/dynet -ldynet
EXEC= Training Testing

all: $(EXEC)

Training: train.o rnn.o data.o
	$(CC) -o Training train.o rnn.o data.o $(LDFLAGS)

Testing: predict.o rnn.o data.o
	$(CC) -o Testing predict.o rnn.o data.o $(LDFLAGS)

predict.o: run/predict.cpp modele/rnn.hpp code_data/data.hpp
	$(CC) -o predict.o -c run/predict.cpp $(CFLAGS)

train.o: run/train.cpp modele/rnn.hpp code_data/data.hpp
	$(CC) -o train.o -c run/train.cpp $(CFLAGS)

rnn.o: modele/rnn.cpp modele/rnn.hpp code_data/data.hpp
	$(CC) -o rnn.o -c modele/rnn.cpp $(CFLAGS)

data.o: code_data/data.cpp code_data/data.hpp
	$(CC) -o data.o -c code_data/data.cpp $(CFLAGS)


clean:
	rm -rf *.o

clean_weight:
	rm -rf *.params

clean_embedding:
	rm -rf *.emb

rmproper: clean
	rm -rf $(EXEC) 

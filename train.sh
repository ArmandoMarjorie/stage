#!/bin/bash

LD_LIBRARY_PATH=/usr/local/bin
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/storage/raid1/homedirs/marjorie.armando/lib/dynet/build/dynet
export LD_LIBRARY_PATH

./Training Files/train Files/dev 1 100 100 0.3 20 16 3 Files/glove_snli_100d.emb Files/output.emb

#!/bin/bash

LD_LIBRARY_PATH=/usr/local/bin
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/storage/raid1/homedirs/marjorie.armando/lib/dynet/build/dynet
export LD_LIBRARY_PATH

./Testing Files/test Files/output.emb 1 100 100 #A changer : rnn_D-0.3_L-1_I-100_H-100_pid-100556.params# 3
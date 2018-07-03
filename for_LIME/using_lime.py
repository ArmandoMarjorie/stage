#!/usr/bin/env python3
# coding: utf-8
from __future__ import print_function
#import lime
import numpy as np
import socket
import sys
import re

class_names = ["neutral","entailment","contradiction"]


instance = []
labels = []

""" Lecture du fichier mots_pour_app contenant les phrases en brut """
test_file = open(sys.argv[1], "r")
for line in test_file: #lit -3 de début
    l = test_file.readline() #lit le label
    if line.rstrip('\n\r') == "neutral":
        labels.append(0)
    elif line.rstrip('\n\r') == "entailment":
        labels.append(1)
    else:
        labels.append(2)
        
    string_tmp = ""
    for j in range(2):
        l = test_file.readline() #lit 'premise -2'
        l = test_file.readline() #lit le nb d'expression
        nb = int(l.rstrip('\n\r'))
        line = ""
        li = ""
        for i in range(nb):
            l = test_file.readline()
            li = ''.join(re.split("\[[^a-zA-Z0-9_.,?!:;-]\] | \{[^a-zA-Z0-9_.,?!:;-]\}",l.rstrip('\n\r')))
            line = line + li
            line = line + " "
            #print(line)
        
        if j==0:
            string_tmp = line
        else:
            instance.append(string_tmp + line)
    
    for j in range(3):
        l = test_file.readline() #lit -3 de fin + 2 sauts de ligne



#print(instance[1])




""" A retenir :
# Envoyer au serveur :
msg = "hello sunshine"
socket.send(msg.encode())

# Recevoir du serveur :
msg = socket.recv(1000)
print(len(msg.decode()))
socket.close()
"""



















"""

# Lecture du fichier contenant le nb de mots importants 
lenght_file = open(sys.argv[2], "r")
array_len = []
for line in lenght_file:
    array_len.append(int(line.rstrip('\n\r')))
lenght_file.close()    



#    Utilisation de socket pour appeler le serveur en c++

hote = "localhost"
port = 50007

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((hote, port))
print ("Connection on {}".format(port))





from lime.lime_text import LimeTextExplainer
import operator
explainer = LimeTextExplainer(class_names=class_names)
n_samp = 0
for i in range(len(premise)) :
    num_features = array_len[n_samp*2]
    if(num_features != 0):
        msg = "prem"
        socket.send(msg.encode())
        msg = socket.recv(1000)
        #print("recu : " + msg.decode())
        exp = explainer.explain_instance(premise[i],hypothesis[i], socket, n_samp=n_samp, labels=[0, 1, 2], num_features=num_features)
        #on envoie hypothesis[i] car on enverra au serveur la prémisse modifiée + l'hypothèse
        print('Sample numero ', n_samp)
        print('For premise = ')
        print ('Explanation for class %s' % class_names[0]) # Les mots en faveur de la classe neutral
        print ('\n'.join(map(str, exp.as_list(label=0))))
        print ()
        print ('Explanation for class %s' % class_names[1]) # Les mots en faveur de la classe entailment
        print ('\n'.join(map(str, exp.as_list(label=1))))
        print ()
        print ('Explanation for class %s' % class_names[2]) # Les mots en faveur de la classe contradiction
        print ('\n'.join(map(str, exp.as_list(label=2))))
        str_name = "premise_" + str(n_samp) + ".html"
        exp.save_to_file(str_name)
    
    num_features = array_len[n_samp*2+1]
    msg = "hyp"
    socket.send(msg.encode())
    msg = socket.recv(1000)
    #print("recu : " + msg.decode())
    exp = explainer.explain_instance(hypothesis[i], premise[i], socket, n_samp=n_samp, num_features=num_features, labels=[0, 1, 2])
    print('For hypothesis = ')
    print ('Explanation for class %s' % class_names[0]) # Les mots en faveur de la classe neutral
    print ('\n'.join(map(str, exp.as_list(label=0))))
    print ()
    print ('Explanation for class %s' % class_names[1]) # Les mots en faveur de la classe entailment
    print ('\n'.join(map(str, exp.as_list(label=1))))
    print ()
    print ('Explanation for class %s' % class_names[2]) # Les mots en faveur de la classe contradiction
    print ('\n'.join(map(str, exp.as_list(label=2))))
    str_name = "hypothesis_" + str(n_samp) + ".html"
    exp.save_to_file(str_name)
    n_samp = n_samp + 1

msg = "quit"
socket.send(msg.encode())

socket.close()
"""

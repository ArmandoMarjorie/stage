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
labels_instance = []

""" Lecture du fichier mots_pour_app contenant les phrases en brut """
test_file = open(sys.argv[1], "r")
n_samp = 0

for line in test_file: #lit -3 de début
    l = test_file.readline() #lit le label
    li = l.rstrip('\n\r')
    if li == "neutral":
        labels_instance.append(0)
    elif li == "entailment":
        labels_instance.append(1)
    else:
        labels_instance.append(2)
        
    string_tmp = ""
    for j in range(2):
        l = test_file.readline() #lit 'premise -2'
        l = test_file.readline() #lit le nb d'expression
        l = test_file.readline()
        line = l.rstrip('\n\r')
           
        
        if j==0:
            string_tmp = line
        else:
            instance.append(string_tmp + line)
    
    for j in range(3):
        l = test_file.readline() #lit -3 de fin + 2 sauts de ligne
    



"""

# Lecture du fichier contenant le nb de mots importants 
lenght_file = open(sys.argv[2], "r")
array_len = []
for line in lenght_file:
    array_len.append(int(line.rstrip('\n\r')))
lenght_file.close()  

"""


# Utilisation de socket pour appeler le serveur en c++
hote = "localhost"
port = 50007

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((hote, port))
print ("Connection on {}".format(port))


# Utilisation de LIME

from lime.lime_text import LimeTextExplainer
import operator
explainer = LimeTextExplainer(class_names=class_names)
imp_file = open(sys.argv[2], "w")
for i in range(len(instance)) :
    #num_features = array_len[n_samp] pas encore fait
    #num_features = 3
    exp = explainer.explain_instance(instance[i], socket, labels=[0, 1, 2], num_features=len(instance[i]))
    
    str_samp = 'Sample numero '+ str(i) + '\n'
    imp_file.write(str_samp)
    if(labels_instance[i]==0):
        str_label = "neutral" + "\n"
    elif(labels_instance[i]==0):
        str_label = "entailment" + "\n"
    else:
        str_label = "contradiction" + "\n"
    
    imp_file.write( str_label ) # Les mots en faveur de la classe
    str_weight = '\n'.join(map(str, exp.as_list(label=labels_instance[i])))
    imp_file.write(str_weight)
    imp_file.write('\n-3\n')
    
    """
    imp_file.write('Explanation for class entailment \n') # Les mots en faveur de la classe entailment
    str_weight = '\n'.join(map(str, exp.as_list(label=1)))
    imp_file.write(str_weight)
    imp_file.write('\n')
    
    imp_file.write('Explanation for class contradiction \n') # Les mots en faveur de la classe contradiction
    str_weight = '\n'.join(map(str, exp.as_list(label=2)))
    imp_file.write(str_weight)
    imp_file.write('\n\n')
    """
    str_name = "sample_" + str(i) + ".html"
    exp.save_to_file(str_name)

 

msg = "quit"
socket.send(msg.encode())

socket.close()



""" A retenir :
# Envoyer au serveur :
msg = "hello sunshine"
socket.send(msg.encode())

# Recevoir du serveur :
msg = socket.recv(1000)
print(len(msg.decode()))
socket.close()
"""


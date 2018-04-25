#!/usr/bin/env python3
# coding: utf-8
from __future__ import print_function
#import lime
import numpy as np
import socket
import sys

class_names = ["neutral","entailment","contradiction"]

labels = [] # contient les labels pour chaque sample
data = [] # contient les 2 phrases pour chaque sample

""" Lecture du fichier test_for_lime contenant le label et les deux phrases en brut """
test_file = open(sys.argv[1], "r")
i=0
string_tmp = ""
for line in test_file:
	if i==0:
		labels.append(line.rstrip('\n\r'))
	#traitement premise et hypothese...
	elif i==1:
		string_tmp = line.rstrip('\n\r')
	elif i==2:
		data.append(string_tmp + " " + line.rstrip('\n\r'))
		i = -1
	i = i+1
test_file.close()


""" 
	Utilisation de socket pour appeler le serveur en c++
"""
hote = "localhost"
port = 50007

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((hote, port))
print ("Connection on {}".format(port))
# Envoyer au serveur :
msg = "hello sunshine"
socket.send(msg.encode())

# Recevoir du serveur :
# msg = socket.recv(1000)
# print(len(msg.decode()))
socket.close()


"""

from lime.lime_text import LimeTextExplainer
explainer = LimeTextExplainer(class_names=class_names)
for i in data :
	exp = explainer.explain_instance(data[i], socket, num_features=6, labels=[0, 1, 2])
	
 ce sera au serveur d'afficher ça 
#print('Document id: %d' % idx)
#print('Predicted class =', class_names[nb.predict(test_vectors[idx]).reshape(1,-1)[0,0]])
#print('True class: %s' % class_names[newsgroups_test.target[idx]])


 
	explanations for different labels. 
	Notice that the positive and negative signs are with respect to a particular label.
	So that words that are negative towards class 0 may be positive towards class 15, and vice versa.

print ('Explanation for class %s' % class_names[0]) # Les mots en faveur de la classe neutral
print ('\n'.join(map(str, exp.as_list(label=0))))
print ()
print ('Explanation for class %s' % class_names[1]) # Les mots en faveur de la classe entailment
print ('\n'.join(map(str, exp.as_list(label=1))))
print ()
print ('Explanation for class %s' % class_names[2]) # Les mots en faveur de la classe contradiction
print ('\n'.join(map(str, exp.as_list(label=2))))

  visualization of the original document, with the words in the explanations highlighted 
exp.show_in_notebook(text=True)

socket.close()
"""

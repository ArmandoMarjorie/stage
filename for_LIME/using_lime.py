import lime
import numpy as np
from __future__ import print_function

class_names = ["neutral","entailment","contradiction"]

# Reading probas given by our c++ programm
"""
probas = []
proba = []
i = 0
import sys
for line in sys.stdin:
	proba[i] = line
	i++
probas.append(proba)
"""



from lime.lime_text import LimeTextExplainer
explainer = LimeTextExplainer(class_names=class_names)

# changer newsgroups_test.data[idx], c.predict_proba
# newsgroups_test.data[idx] le sample qu'on veut expliquer (raw text)
# c.predict_proba : classifier prediction probability function, which takes a list of d strings and outputs a (d, k) numpy array with
	# prediction probabilities, where k is the number of classes.
	# For ScikitClassifiers , this is classifier.predict_proba.
	
exp = explainer.explain_instance(newsgroups_test.data[idx], APPEL A NOTRE SERVEUR, num_features=6, labels=[0, 1, 2])
print('Document id: %d' % idx)
print('Predicted class =', class_names[nb.predict(test_vectors[idx]).reshape(1,-1)[0,0]])
print('True class: %s' % class_names[newsgroups_test.target[idx]])

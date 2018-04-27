


tester avec phrase en entrer :
echo "2 A man playing an electric guitar on stage . -1 A man is performing for cash . -1" | ./Testing Files/snli.lexique Files/output_sys2_glove_unk_not_same.emb 1 100 100 rnn_D-0.3_L-1_I-100_H-100_pid-100556.params 2 2

tester avec id des mots en entrer :
echo "2 1 8 12 60 -1 10 2 3 40 -1 -2" | ./Testing Files/snli.lexique Files/output_sys2_glove_unk_not_same.emb 1 100 100 rnn_D-0.3_L-1_I-100_H-100_pid-100556.params 2 1

tester avec fichier de test :
./Testing Files/test Files/output_sys2_glove_unk_not_same.emb 1 100 100 rnn_D-0.3_L-1_I-100_H-100_pid-100556.params 2 0



__________________________________

faire marcher le serveur :

./Exe_Serveur ../Files/test_length ../Files/snli.lexique ../Files/output.emb 1 100 100 rnn_D-0.3_L-1_I-100_H-100_pid-4590.params 2

faire marcher le client :
aller dans lime
python3 using_lime.py ../../Files/test_for_lime

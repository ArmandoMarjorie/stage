#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <locale>

using namespace std;

//sert pour tokenizer les mots de remplacements

void reading_lexique(char* lexique_filename, map<string, int>& word_to_id)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}
	string word;
	int id;
	while(lexique_file >> word && lexique_file >> id)
		word_to_id[word] = id;
	word_to_id["-1"] = -1;
	//word_to_id["0"] = 0;
	word_to_id["-2"] = -2;
	word_to_id["-3"] = -3;
	word_to_id["ACTUAL"] = -4;
	word_to_id["NEXT"] = -5;
	word_to_id["PREV"] = -6;
	word_to_id["NOWORD"] = 0;
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
}

unsigned write_label(string label)
{
	if(label == "neutral")
		return 0;
	if(label == "entailment")
		return 1;
	if(label == "contradiction")
		return 2;
	else
		return 44;
}

bool verif(char crochet, unsigned num_word, string& word, char acc, char croch, unsigned num_example)
{
	bool cro;
	if(crochet == acc)
		cro = false;
	else if(crochet == croch)
		cro = true;
	else
	{
		cout << "encadrement non valide (ni { ni [) mot numero " << num_word << ", dans l'exemple " << num_example << " : " << word << endl;
		exit(EXIT_FAILURE);
	}
	return cro;
}


bool type_word(char c_ouvrant, char c_fermant, unsigned num_word, string& word, unsigned num_example)
{
	bool crochet_ouvrant = verif(c_ouvrant,num_word,word,'{','[',num_example);
	bool crochet_fermant = verif(c_fermant,num_word,word,'}',']',num_example);
		
	if(crochet_ouvrant != crochet_fermant)
	{
		cout << "ATTENTION mauvais encadrement !! mot numero" << num_word << " : " << word << endl;
		exit(EXIT_FAILURE);
	}
	
	return crochet_ouvrant;
}

void tokeniser_nouveau_remplacement(char* lexique, char* file, char* output_f)
{
	ifstream f(file, ios::in);
	if(!f)
	{ 
		cerr << "Impossible to open the file " << file << endl;
		exit(EXIT_FAILURE);
	}		
	ofstream output(output_f, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file "<< output_f << endl;
		exit(EXIT_FAILURE);
	}
	map<string, int> word_to_id;
	reading_lexique(lexique, word_to_id);
	
	map< string, int >::iterator it;
	string word, w, word2;
	unsigned nb_words, num_example=0, nb_words_remplacement, nb_line=0;
	bool crochet = true;
	
	while(f >> word) //lit "-3"
	{
		output << word_to_id[word] << endl; //écrit "-3"
		++nb_line;
		f >> word; f >> word; //lit "label:" et le label
		++nb_line;
		output << write_label(word) << endl; //écrit le label
		
		for(unsigned nb_phrase=0; nb_phrase<2; ++nb_phrase)
		{
			f >> word; f >> word; //lit "premise" et "-2"
			++nb_line;
			f >> nb_words; //nb d'expressions composant la phrase
			output << nb_words << endl;
			++nb_line;
			
			for(unsigned i=0; i < nb_words; ++i) //pour toutes les expressions composant la phrase
			{
				// EXPRESSION DE LA PHRASE
				f >> word;
				++nb_line;
				//cout << "word = " << word << endl;
				while(word[word.size()-1] != ']' && word[word.size()-1] != '}')
				{
					
					f >> word2;
					word = word + " ";
					word = word + word2;
				}
				
				//cout << "EXPRESSION DANS LA PHRASE = " << word << endl;
				crochet = type_word(word[0], word[word.size()-1], i,word, num_example); //crocher = true si [mot], false si {mot}
				output << word[0];
				//extract mots entre crochets
				for(unsigned i=1; i < word.size()-1; ++i)
				{
					stringstream ss;
					while(word[i] != ' ' && i < word.size()-1)
					{
						ss << word[i];
						++i;
					}
					w = ss.str();
					it = word_to_id.find(w);
					if( it == word_to_id.end() )
					{
						cout << "MOT INCONNU (ligne " << nb_line << ", " << w << ")\n";
						output << "INCONNU ";
					}
					else
						output << word_to_id[w] << " ";
				}
				output << word[word.size()-1] << " ";
				
				// EXPRESSIONS DE REMPLACEMENT
				if(crochet)
				{
					f >> nb_words_remplacement;
					output << nb_words_remplacement << " ";
					for(unsigned j=0; j < nb_words_remplacement; ++j)
					{
						f >> word;
						while(word[word.size()-1] != '#')
						{
							f >> word2;
							word = word + " ";
							word = word + word2;
						}
						//cout << "EXPRESSION DE REMPLACEMENT = " << word << endl;
						while(word != "-1")
						{
							output << word[0];
							for(unsigned i=1; i < word.size()-1; ++i)
							{
								stringstream ss;
								while(word[i] != ' ' && i < word.size()-1)
								{
									ss << word[i];
									++i;
								}
								w = ss.str();
								it = word_to_id.find(w);
								if( it == word_to_id.end() )
								{
									cout << "MOT INCONNU (ligne " << nb_line << ", " << w << ")\n";								
									output << "INCONNU ";
								}
								else
									output << word_to_id[w] << " ";						
							}
							output << word[word.size()-1] << " ";
							f >> word; //lit ACTUAL, NEXT ou PREV
							//cout << "\tIL EST = " << word << endl;
							output << word_to_id[word] << " "; 
							f >> word; //lit une expression de remplacement
							if(word == "-1")
							{
								//cout << "\t-1\n";
								continue;
							}
							while(word[word.size()-1] != '#')
							{
								f >> word2;
								word = word + " ";
								word = word + word2;
							}
							//cout << "\tEXPRESSION DE REMPLACEMENT = " << word << endl;
						}
						output << "-1 ";
					}
					
				}
				output << endl;
				
			}
		}
		f >> word; //lit -3
		output << word_to_id[word] << endl << endl << endl; //écrit -3
		
		++num_example;
	}
	
	f.close();	
	output.close();
}


void verif_nb_expressions(char* file)
{
	
	ifstream f(file, ios::in);
	if(!f)
	{ 
		cerr << "Impossible to open the file " << file << endl;
		exit(EXIT_FAILURE);
	}	
	int nb, num=0, nb_words;
	string word;
	std::string::size_type sz;
	while(getline(f,word)) //lit "-3"
	{	
		getline(f,word); //lit "label:" et le label
		getline(f,word);//lit "premise" et "-2"
		
		for(unsigned nb_phrase=0; nb_phrase<2; ++nb_phrase)
		{
			
			getline(f,word);
			nb_words = std::stoi (word,&sz);	//chelou	
			
			cout << "NB MOTS = " <<  nb_words << endl;
			nb=0;
			getline(f,word);
			while(word != "-3" && word != "hypothesis -2")
			{
				cout << "ligne = " << word << endl;
				
				++nb;
				getline(f,word);
			}
			cout << "ligne = " << word << endl;
			
			if(nb != nb_words)
			{
				cout << "ATTENTION pas le bon nb d'expressions (ecrit " << nb_words << " au lieu de " << nb << ")\n"
				<< "sample " << num << endl;
				
			}
			
		}
		++num;
		
	}
	
}

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique\n"
			 << "file mots remplacant\n"
			 << "output (mots remplacant tokenisé)\n";
		exit(EXIT_FAILURE);
	}
	

	verif_nb_expressions(argv[2]);
	//tokeniser_nouveau_remplacement(argv[1], argv[2], argv[3]);
	return 0;
}

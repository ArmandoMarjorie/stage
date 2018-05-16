#include "detoken_explication.hpp"
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <algorithm>

/** LEXIQUE = VOCABULAIRE + LEUR ID */
// g++ -std=c++11 detoken_explication.cpp -o Detok_expl

using namespace std;

string detoken_label(int label)
{
	switch(label)
	{
		case 0:
		{
			return "neutral";
		}
		case 1:
		{
			return "entailment";
		}
		case 2:
		{
			return "contradiction";
		}
		default:
		{
			return "not a label";
		}
	}
	
}

void reading_lexique(char* lexique_filename, map<unsigned, string>& id_to_word, bool DI)
{
	ifstream lexique_file(lexique_filename, ios::in);
	if(!lexique_file)
	{ 
		cerr << "Impossible to open the file " << lexique_filename << endl;
		exit(EXIT_FAILURE);
	}
	string word;
	unsigned id;
	while(lexique_file >> word && lexique_file >> id)
		id_to_word[id] = word;
	if(!DI)
		id_to_word[0] = "<every words>";
	else
		id_to_word[0] = "<no words>";
	cerr << lexique_filename << " has been read" << endl;
	lexique_file.close();	
}
/* faire detoken pr removing couple (avec DI) */

void detokenizer_with_couple(char* lexique_filename, char* explication_filename, char* output_filename, bool DI)
{
	ifstream explication_file(explication_filename, ios::in);
	if(!explication_file)
	{ 
		cerr << "Impossible to open the file " << explication_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	cerr << "Reading " << lexique_filename << endl;
	
	/*Reading lexique and saving in a map the word of an id*/
	map<unsigned, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word, DI);
	
	int val;
	float val_di=0;
	unsigned cpt;
	unsigned num_sample = 1;
	
	while(explication_file >> val)
	{
		output << "\tsample numero " << num_sample << "\nlabel : " << detoken_label(val) << ", ";
		explication_file >> val;
		output<< "label predicted : " << detoken_label(val) << endl;
		
		explication_file >> val;
		for(cpt=0; cpt <2; ++cpt) // reading the premise and the hypothesis
		{
			if(cpt==0)
				output << "premise : ";
			else
				output << "hypothesis : ";
			while(val != -1)
			{
				output << id_to_word[val] << " ";
				explication_file >> val;
			}
			explication_file >> val; //read the "-1"
			output << endl;
		}
		while(val != -3)
		{
			while(val != -1) //reading couple
			{
				if(val == -2)
					output << " ; ";
				else
					output << id_to_word[val] << " ";
				explication_file >> val;
			}
			if(!DI)
			{
				explication_file >> val; //reading label's couple
				output << " = label predicted : " << detoken_label(val) << " , true label : " ;
				explication_file >> val;
				output << detoken_label(val) << endl;
			}
			else
			{
				for(unsigned l=0; l<3; ++l) //read the 3 DIs
				{
					explication_file >> val_di; //reading DI
					output << " DI["<< detoken_label(l) <<"] : " << val_di << " ; ";
				}
				output << endl;
			}
			explication_file >> val;
		}
		++num_sample;
		output << endl;
	}
	
}

string color(unsigned lab)
{
	switch(lab)
	{
		case 0:
		{
			return "cyan";
		}
		case 1:
		{
			return "yellow";
		}
		case 2:
		{
			return "red";
		}
		default:
		{
			return "green";
		}
	}	
}

void detoken_expl_sys4(char* lexique_filename, char* explication_filename, char* output_filename)
{
	
	Gtk::Window window;
	window.set_title("Interpretation of the model");
	window.resize(1000, 1000);
	window.set_position(Gtk::WIN_POS_CENTER);
	Gtk::ScrolledWindow barresDeDefilement;
	barresDeDefilement.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    window.add(barresDeDefilement);
   // Gtk::TextView zoneDeTexte; //Création d'une zone de texte.
    //barresDeDefilement.add(etiquette); //Ajout de la zone de texte au conteneur.
    
	
	Gtk::Label etiquette;
	etiquette.set_line_wrap();
	
	ifstream explication_file(explication_filename, ios::in);
	if(!explication_file)
	{ 
		cerr << "Impossible to open the file " << explication_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	/*Reading lexique and saving in a map the word of an id*/
	map<unsigned, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word, false);		
	int val;
	unsigned cpt;
	unsigned num_sample=1;
	string text;
	float proba_label[3] = {0};
	float prob;
	stringstream ss;
	unsigned i, lab;
	unsigned important_couple[4] = {0};
	while(explication_file >> val)
	{
		
		ss << "\t\tsample numero " << num_sample << "\nlabel : " << detoken_label(val) << ", ";
		++num_sample;
		explication_file >> val;
		ss << "label predicted : " << detoken_label(val) << "\n";
		
		for(lab=0; lab < 3; ++lab)
		{
			explication_file >> prob;
			ss << "p("<< detoken_label(lab) <<") = "<<  prob*100 << " , ";	
		}
		ss << "\n";
		explication_file >> val;
		vector<string> premise;
		vector<string> hypothesis;
		for(i=0; i<4; ++i)
		{
			important_couple[i] = val;
			explication_file >> val;
		}
		

		for(cpt=0; cpt <2; ++cpt) // reading the premise and the hypothesis
		{	
			while(val != -1)
			{
				if(!cpt)
					premise.push_back(id_to_word[val]);
				else
					hypothesis.push_back(id_to_word[val]);
				explication_file >> val;
			}
			explication_file >> val;
		}		
		

		ss << "premise : ";
		for(i=0; i<premise.size(); ++i)	
		{
			
			if( important_couple[0] == i )
				ss << "<span background='cyan'>" << premise[i] << "</span> ";	
			else if( important_couple[2] == i )
				ss << "<span background='red'>" << premise[i] << "</span> ";	
			else
				ss << premise[i] << " ";
		}
		ss << "\n";
		ss << "hypothesis : ";
		for(i=0; i<hypothesis.size(); ++i)
		{
			
			if( important_couple[1] == i )
				ss << "<span background='cyan'>" << hypothesis[i] << "</span> ";	
			else if( important_couple[3] == i )
				ss << "<span background='red'>" << hypothesis[i] << "</span> ";	
			else
				ss << hypothesis[i] << " ";	
		}		
		ss << "\n";
		
	}	
	output.close();
	explication_file.close();
	text = ss.str();
	etiquette.set_markup(text);
	barresDeDefilement.add(etiquette);
	window.show_all();
	Gtk::Main::run(window);
}

void detoken_expl(char* lexique_filename, char* explication_filename, char* output_filename)
{
	
	Gtk::Window window;
	window.set_title("Interpretation of the model");
	window.resize(1000, 1000);
	window.set_position(Gtk::WIN_POS_CENTER);
	Gtk::ScrolledWindow barresDeDefilement;
	barresDeDefilement.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    window.add(barresDeDefilement);
   // Gtk::TextView zoneDeTexte; //Création d'une zone de texte.
    //barresDeDefilement.add(etiquette); //Ajout de la zone de texte au conteneur.
    
	
	Gtk::Label etiquette;
	etiquette.set_line_wrap();
	
	ifstream explication_file(explication_filename, ios::in);
	if(!explication_file)
	{ 
		cerr << "Impossible to open the file " << explication_filename << endl;
		exit(EXIT_FAILURE);
	}
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	/*Reading lexique and saving in a map the word of an id*/
	map<unsigned, string> id_to_word;
	reading_lexique(lexique_filename, id_to_word, false);	
	int val;
	unsigned cpt;
	unsigned num_sample=1;
	string text;
	float proba_label[3] = {0};
	float prob;
	stringstream ss;
	unsigned nb_words, i, lab;
	while(explication_file >> val)
	{
		
		ss << "\t\tsample numero " << num_sample << "\nlabel : " << detoken_label(val) << ", ";
		++num_sample;
		explication_file >> val;
		ss << "label predicted : " << detoken_label(val) << "\n";
		
		for(lab=0; lab < 3; ++lab)
		{
			explication_file >> prob;
			proba_label[lab] = prob*100;	
		}
		
		explication_file >> val;
		/* important words in prem and in hyp */
		//output << "in premise : ";
		vector<vector<unsigned>> imp_word;
		vector<vector<unsigned>> imp_word_h;
		vector<string> premise;
		vector<string> hypothesis;
		for(i=0; i<6; ++i)
		{
			vector<unsigned> tmp;
			while(val != -1)
			{
				tmp.push_back(val);
				explication_file >> val;
			}
			explication_file >> val;
			if(i<3)
				imp_word.push_back(tmp); //premise
			else
				imp_word_h.push_back(tmp); //hypothesis
		}
		

		for(cpt=0; cpt <2; ++cpt) // reading the premise and the hypothesis
		{	
			while(val != -1)
			{
				if(!cpt)
					premise.push_back(id_to_word[val]);
				else
					hypothesis.push_back(id_to_word[val]);
				explication_file >> val;
			}
			explication_file >> val;
		}	
		for(lab=0; lab<3; ++lab)
		{
			ss << "\tfor label " << detoken_label(lab) << " ( " << proba_label[lab] << " % ) :\n";
			ss << "premise : ";
			for(i=0; i<premise.size(); ++i)	
			{
				
				if( std::find(imp_word[lab].begin(), imp_word[lab].end(), i) != imp_word[lab].end())
					ss << "<span background='" << color(lab) << "'>" << premise[i] << "</span> ";	
				else
					ss << premise[i] << " ";
			}
			ss << "\n";
			ss << "hypothesis : ";
			for(i=0; i<hypothesis.size(); ++i)
			{
				
				if( std::find(imp_word_h[lab].begin(), imp_word_h[lab].end(), i) != imp_word_h[lab].end())
					ss << "<span background='" << color(lab) << "'>" << hypothesis[i] << "</span> ";	
				else
					ss << hypothesis[i] << " ";	
			}		
			ss << "\n";
		}
		ss << "\n";
		
	}	
	output.close();
	explication_file.close();
	text = ss.str();
	etiquette.set_markup(text);
	barresDeDefilement.add(etiquette);
	window.show_all();
	Gtk::Main::run(window);
}

/*
int main(int argc, char** argv)
{
	if(argc != 5)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "lexique_file\n"
			 << "explication_file (format with id)\n"
			 << "with DI (1) or without DI (0)\n"
			 << "output_name (explication file detokenized)\n";
		exit(EXIT_FAILURE);
	}
	bool DI = (atoi(argv[3]) == 1);
	
	detokenizer(argv[1], argv[2], argv[4], DI);
	
	return 0;
}
*/

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

void generating_output_file(char* set_filename, char* output_filename, int id_to_remove)
{
	ifstream snli_file(set_filename, ios::in);
	if(!snli_file)
	{ 
		cerr << "Impossible to open the file " << set_filename << endl;
		exit(EXIT_FAILURE);
	}	
	ofstream output(output_filename, ios::out | ios::trunc);
	if(!output)
	{
		cerr << "Problem with the output file " << output_filename << endl;
		exit(EXIT_FAILURE);
	}
	unsigned i;
	int word;
	cerr << "generating" << output_filename << " ...\n";
	while( snli_file >> word ) //read a label
	{
		output << word << endl;
		
		/* read premises and hypothesis */
		for(i=0; i<2; ++i)
		{
			snli_file >> word;
			while(word != -1)
			{
				if(i==0 || (i==1 && word != id_to_remove))
					output << word << " ";
				snli_file >> word;
			}
			output << word << " "; //write -1
			snli_file >> word; //read the sentence size
			output << word << endl; //write the sentence size
		}		
		
	}
	snli_file.close();
	output.close();
	
}


int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "Usage :\n " << argv[0] << "\n\n"
			 << "test_file with id\n"
			 << "output_test_filename\n";
		exit(EXIT_FAILURE);
	}
	generating_output_file(argv[1], argv[2], 34);
	
	
	return 0;
}


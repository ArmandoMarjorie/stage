


void write_vocab_file(char* snli_json_filename, map< string, unsigned >& vocab, ofstream& output)
{
	ifstream snli_file(snli_json_filename, ios::in);
	if(!snli_file)
	{ 
		cerr << "Impossible to open the file " << snli_json_filename << endl;
		exit(EXIT_FAILURE);
	}		
	cerr << "Reading " << snli_json_filename << " ...\n";
	//vector<string>::iterator it;
	map< string, unsigned >::iterator it;
	string word;
	unsigned i;
	while(snli_file >> word) //read a label
	{
		/* read premises and hypothesis */
		for(i=0; i<2; ++i)
		{
			snli_file >> word;
			while(word != "-1")
			{
				std::transform(word.begin(), word.end(), word.begin(), ::tolower); 
				it = vocab.find(word);
				if( it == vocab.end() ) //unknown word
				{
					output << word << endl;
					vocab[word] = 1;
					//cerr << word << endl;				
				}
				snli_file >> word;
			}
			snli_file >> word; //read the sentence size
		}
	}
	
	snli_file.close();
}

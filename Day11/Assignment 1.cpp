#include <iostream>
#include <vector>
#include <cassert>

class Sentence 
{
	public:
	char* sentence;
	std::vector<int> vsentence; // stores the word offsets
public:
	std::string last_call = "None";
	Sentence(char* sent) // regular constructor
	{
		//sentence = sent;
		//FindWords();
		replace(sent);
		last_call = "Regular Constructor";
	}
   
	// insert a deep copy constructor here
	Sentence(const Sentence& i_sent)
	{
		vsentence = i_sent.vsentence;
		sentence = new char[64]();
		int i = 0;
		char* temp = i_sent.data();
		while((temp[i] != '\n')&&(i < 62))
		{
			sentence[i] = temp[i];
			i++;
		}
		sentence[i] = '\n';
		last_call = "Copy Constructor";
	}
    
	// insert a deep copy assignment operator here
	Sentence& operator =(const Sentence& n_sent) 
    	{
		if(sentence != nullptr)
		{
			delete(sentence);	//deletes any existing internal sentence (not the source's), which *shouldn't* exist yet
		}
		vsentence = n_sent.vsentence;
		sentence = new char[64]();
		int i = 0;
		char* temp = n_sent.data();
		while((temp[i] != '\n')&&(i < 62))
		{
			sentence[i] = temp[i];
			i++;
		}
		sentence[i] = '\n';
		//sentence = new char(*n_sent.sentence);	//should copy the contents of the source's sentence, not the location
		last_call = "Copy Operator";
		return *this;
	}

	//move constructor
	Sentence(Sentence&& i_sent)
	{
		if(sentence != nullptr)
		{
			std::cout <<"delete";
			delete(sentence);
		}
		vsentence = i_sent.vsentence;
		sentence = i_sent.sentence;
		last_call = "Move Constructor";
		vsentence.clear();
		i_sent.sentence = nullptr;
	//True move would steal the last_call state, but I don't want that behaviorA
	}

	//move operator 
	Sentence& operator=(Sentence&& n_sent)
	{
		if(this == &n_sent)	//if not actually moving, just return ref to self
		{	
			return *this;
		}

		if(sentence != nullptr)
		{
			delete(sentence);
		}
		
		vsentence = n_sent.vsentence;
		sentence = n_sent.sentence;
		last_call = "Move Operator";
		vsentence.clear();
		n_sent.sentence = nullptr;
		return *this;
	}

	char* data()	const		// returns the data for sentence
	{
		return sentence;
	}

	std::string datastring() const
	{
		std::string returnstring = "";
		int i = 0;

		while(sentence[i] != '\n')
		{
			returnstring.push_back(sentence[i]);
			i++;
		}
		returnstring.push_back('\n');
		return returnstring;
	}
	void print()	const
	{
		int i = 0;
		while(sentence[i] != '\n')
		{
			std::cout << sentence[i];
			i++;
		}
		std::cout << std::endl;
	}
	int num_words() const		// fetches the number of words 
	{
		return vsentence.size();
	}

	std::string get_word(int n)	// get nth word in sentence 
	{
		std::string returnstring = "";
		if (n >= vsentence.size())	//if n is past the end of vsentence
		{
			std::cout << "Invalid word selection" << std::endl;
			return returnstring;
		}
		
		int i = vsentence.at(n);
		if(n == vsentence.size() - 1)	//if n is last element of vsentence
		{
			while(sentence[i] != '\n')
			{
				returnstring.push_back(sentence[i]);
			}
		}
		else
		{
			while(sentence[i] < vsentence.at(n + 1))
			{
				returnstring.push_back(sentence[i]);
			}
		}

		return returnstring;

	}
	
	void replace(char* sent)	// replace existing sentence
	{
		sentence = new char[64]();
		int i = 0;
		char* temp = sent;
		while((temp[i] != '\n')&&(i < 62))
		{
			sentence[i] = temp[i];
			i++;
		}
		sentence[i] = '\n';
		FindWords();
	}
	
	~Sentence() // destroy any memory allocated
	{
		//delete(sentence);	//how does this lead to a double-freeing?
	}
	
	//FindWords will give odd results if sentence contains multiple spaces in a row or if it ends with a space. sentence MUST HAVE A NULL TERMINATOR AT THE END.
	void FindWords()
	{
		vsentence.clear();
		
		unsigned int i = 0;
		vsentence.push_back(0);
		bool space_was_last = false;
		//sentence better be a properly null-terminated c-string, or we get a mess
		while(sentence[i] != '\n')
		{
			if(sentence[i] == ' ')
			{
				vsentence.push_back(i + 1);	// next char is next word start
			}

			i++;
		}

	}
};


int main ()
{
	std::string test1 = "This is a string of words.";
	test1.push_back('\n');
	char* test_sent1 = &test1[0];

	std::string test2 = "Arbitrary words go here for demo purposes";
	test2.push_back('\n');
	char* test_sent2 = &test2[0];

	std::string test3 = "Move and copy constructor practice.";
	test3.push_back('\n');
	char* test_sent3 = &test3[0];

	//char* test2_sent = test.c_str();
	//char* test3_sent3 = &test.c_str();
	assert(test1.back() == '\n');
	Sentence Sent1(test_sent1);
	assert(Sent1.last_call == "Regular Constructor"); 

	Sentence Sent2 = Sent1;
	assert(Sent2.last_call == "Copy Constructor"); 
	//std::cout << "Copy Constructor" << std::endl;
	//Sent1.print();
	//Sent2.print();
	assert(Sent2.data() != Sent1.data());
	assert(Sent2.datastring() == Sent1.datastring());

	Sent2.replace(test_sent2);
	Sent1 = Sent2;
	assert(Sent1.last_call == "Copy Operator");
	//std::cout << "Copy Operator" << std::endl;
	//Sent1.print();
	//Sent2.print();
	assert(Sent2.data() != Sent1.data());
	//assert(Sent2.datastring() == Sent1.datastring());

	Sentence Sent3 = std::move(Sent2);
	assert(Sent3.last_call == "Move Constructor");
	//std::cout << "Move Constructor" << std::endl;
	//Sent3.print();
	assert(Sent3.data() != Sent2.data());

	Sent3 = std::move(Sent1);
	assert(Sent3.last_call == "Move Operator");
	//std::cout << "Move Operator" << std::endl;
	//Sent3.print();
	assert(Sent3.data() != Sent1.data());

	Sent2.replace(test_sent3);
	Sent1.replace(test_sent3);
	assert(Sent2.data() != Sent1.data());
	assert(Sent2.datastring() == Sent1.datastring());



	//Sent1.print();
	//Sent2.print();
	//Sent3.print();


//	Sent1.data();

	//	last_call = "Regular Constructor";
	//	last_call = "Copy Constructor";
	//	last_call = "Copy Operator";
	//	last_call = "Move Constructor";
	//	last_call = "Move Operator";
	//for (int i = 0; i < 
	//std::cout << 
	return 0;
}

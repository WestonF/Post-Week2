#include <iostream>
#include <cassert>
#include <vector>

constexpr int SIZELIMIT = 64;

class badstring
{
	//	char notstringdata[SIZELIMIT];
		char notstring[SIZELIMIT];// = notstringdata;
		int size;
	public:
		//purge contents
		void empty()
		{
			for (int i = 0; i < SIZELIMIT; i++)
			{
				notstring[i] = '\0';
				size = 0;
			}
		}

		void set(char* instring, int location = 0)
		{
			empty();
			int i = location;
			while((instring[i] != '\0')&& ( i < SIZELIMIT))
			{
				notstring[i] = instring[i];
				i++;
			}
			
			if(i == SIZELIMIT)
			{
				i = SIZELIMIT - 1;
			}

			notstring[i] = '\0';
			size = i;

		}
		
		void set(const char* instring, int location = 0)
		{
			empty();
			int i = location;
			while((instring[i] != '\0')&& ( i < SIZELIMIT))
			{
				notstring[i] = instring[i];
				i++;
			}
			
			if(i == SIZELIMIT)
			{
				i = SIZELIMIT - 1;
			}

			notstring[i] = '\0';
			size = i;
		}
		char* get()	
		{
			return &notstring[0];
		}

		char* getcopy()	const
		{
			char* temp = new char[64];
			int i = 0;
			while((notstring[i] != '\0')&& ( i < SIZELIMIT))
			{
				temp[i] = notstring[i];
				i++;
			}
			temp[SIZELIMIT - 1] = '\0';

			return &temp[0];
		}

		int getsize()	const
		{
			return size;
		}

//		void operator ()(
		char* operator +(char* instring)
		{
			
			char* temp = new char[SIZELIMIT];
			int i = 0;
			while((notstring[i] != '\0')&& ( i < SIZELIMIT))
			{
				temp[i] = notstring[i];
				i++;
			}
			int j = 0;
			while((instring[j] != '\0')&& ((i+ j) < SIZELIMIT))
			{
				temp[i + j] = instring[j];
				j++;
			}
			
			if((i + j) >= SIZELIMIT)
			{
				i = SIZELIMIT - 1;
				j = 0;
			}

			temp[i + j] = '\0';

			return temp;
			
		}

		
		char* operator *(int count)
		{
			//load temp array with contents of notstring
			char* temp = new char[SIZELIMIT];
			int i = 0;
			int offset = 0;
			for (int j = 0; j < count; j++)
			{
				i = 0;
				while((notstring[i] != '\0')&& ( (i + (offset * j)) < SIZELIMIT))
				{
					temp[i + (offset * j)] = notstring[i];
					i++;
				}
				offset = (i);
			}
			return &temp[0];
		}
		
		char operator [](int index)
		{
			if(index >= size)
			{
				return '\n';
			}

			return notstring[index];
		}
		
	
		//initialize empty
		badstring()
		{
			empty();
		}

		badstring(char instring[])
		{
			empty();
			set(instring);
		}

		//copy constructor
		badstring(const badstring &source)
		{
			empty();
			char* temp = source.getcopy();
			set(temp);
			delete[](temp);
		} 

		//copy assignment operator
		badstring& operator =(const badstring &source)
		{
			empty();
			char* temp = source.getcopy();
			set(temp);
			delete[](temp);
			return *this;
		}

		//move constructor
		badstring(badstring&& source)
		{
			empty();
			set(source.get());
			source.empty();
		}

		//move assignment operator
		badstring& operator =(badstring&& source)
		{
			empty();
			set(source.get());
			source.empty();
			return *this;
		}

};


int main()
{
	std::string words = "These are words";
	const char* p_words = words.c_str();
	//std::string tests = "and tests";
	//const char* p_tests = tests.c_str();

	char tests[] = " and tests\0";

	badstring bad;
	bad.set(p_words);
	bad.empty();
	bad.set(p_words);
//	bad.set(tests);
//	std::cout << bad.get() << std::endl;
//	bad.set(bad + &tests[0]);
//	std::cout << bad.get() << std::endl
	words = "These are words and tests";
	assert((bad + &tests[0]) == words);
	words = "These are wordsThese are wordsThese are wordsThese are words";
	assert((bad * 4) == words);
	char letter = 's';
	assert(bad[3] == letter);
	return 0;
}

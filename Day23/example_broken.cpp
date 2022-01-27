#include <iostream>
#include <thread>
#include <chrono>

class country
{
public:
	int number;
	country():number(0){}
	country(int i_number):number(i_number){}

	void increment_number(int inc = 1)
	{
		number += inc;
		if(!(number % 1000))
		{
			std::cout << std::this_thread::get_id() << std::endl;
		}
	}
};

void BeCountry()
//void BeCountry(bool& done_trading)
{
	thread_local country C;
	bool done_trading = false;
	while(!done_trading)
	{
		C.increment_number();	//increments C.number and (if C.number is an even multiple of 1000) prints the thread ID to console so I can watch it happen and see which thread is doing it
		if(C.number >= 5000)	//Increased the comparison value so that the first thread wasn't done before the second started.
		{
			done_trading = true;
			std::cout << "Done" <<std::endl;
		}
	}
}

int main()
{
	//bool done_trading = false;	
	std::thread t1{BeCountry};
	std::thread t2{BeCountry};

	//std::thread t1{BeCountry, std::ref(done_trading)};
	//std::thread t2{BeCountry, std::ref(done_trading)};
	
	t1.join();
	t2.join();

	std::cout << "Actually done" <<std::endl;
	return 0;
}

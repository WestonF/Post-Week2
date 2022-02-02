#include<iostream>
#include<vector>
#include<memory>
#include<atomic>
#include<thread>
#include<cassert>

#define MO std::memory_order_seq_cst
//#define MO std::memory_order_relaxed
//#define MO std::memory_order_seq_cst
//#define MO std::memory_order_seq_cst


// I had to re-write this program like 6 times to actually be able to observe the effects of the memory ordering. I got tired of changing the memory order entry of every load for every memory type every time I had to re-draft the program, thus MO was born


std::atomic<int> x = {0}, y = 0;
char x_first_char = '\0', y_first_char = '\0';

void set_x()
{
	x.store(1, MO);
}
void set_y()
{
	y.store(1, MO);
}

void check_x_then_y()
{
	int a,b;
	a = x.load(MO);
	b = y.load(MO);

	if(a)		// x was set
	{
		if(a == 1)		// x was most recently set by set_x()
		{
			if(b)		// y was set
			{
				x_first_char = 'b';
			}
			else 		// y was not set
			{
				x_first_char = 'x';
			}
			y.store(2, MO);
		}
		else		// x was most recently set by check_y_then_x(), which means that y MUST have been set by set_y()
		{
			if(b == 1)	//y was most recently set by set_y()
			{
				x_first_char = 'Y';
			}
			else		// something really weird happened
			{
				x_first_char = static_cast<char>(b + static_cast<int>('0'));
			}
		}
	}
	else	//x was not set
	{
		if(b)	// y was set
		{
			x_first_char = 'y';
		}
		else	// y was not set
		{
			x_first_char = 'n';
		}
	}
	

}

void check_y_then_x()
{
	int a,b;
	b = y.load(MO);
	a = x.load(MO);

	if(b)		//if y was set
	{
		if(b == 1)		// y was most recently set by set_y()
		{
			if(a)		// x was set
			{
				y_first_char = 'b';
			}
			else 		//if x was not set
			{
				y_first_char = 'y';
			}
			x.store(2,MO);
		}
		else		// y was most recently set by check_x_then_y(), which means that x MUST have been set by set_x()
		{
			if(a == 1)	//x was most recently set by set_x()
			{
				y_first_char = 'X';
			}
			else		// something really weird happened
			{
				y_first_char = static_cast<char>(a + static_cast<int>('0'));
			}
		}
	}
	else	//x was not set
	{
		if(a)	// x was set
		{
			y_first_char = 'x';
		}
		else	// y was not set
		{
			y_first_char = 'n';
		}
	}
	

}

	//this version was fun, but put too much emphasis on the order in which threads/functions accessed the variables, not the configuration the variables at any given instant
/*
void check_x_then_y()
{
	z.store(z.load(std::memory_order_relaxed) + x.load(std::memory_order_relaxed), std::memory_order_relaxed);
	z.store(z.load(std::memory_order_relaxed) + y.load(std::memory_order_relaxed), std::memory_order_relaxed);
}

void check_y_then_x()
{
	z.store(z.load(std::memory_order_relaxed) * y.load(std::memory_order_relaxed), std::memory_order_relaxed);
	z.store(z.load(std::memory_order_relaxed) * x.load(std::memory_order_relaxed), std::memory_order_relaxed);
}
*/


int main()
{
	int foo = 1;
	char lastset[] = {'\0', '\0'};
	std::vector<char> check_log;
	for(int i = 0; i < 10000; i++)
	{
		std::thread a(set_x);
		std::thread b(set_y);
		std::thread c(check_x_then_y);
		std::thread d(check_y_then_x);
		a.join();
		b.join();
		c.join();
		d.join();
		//memory order ops don't matter between here and the resumption of the loop as there's only one live thread, the one running main
		//if((x_first_char != 'b')||(y_first_char != 'b')) //if both are 'b' nothing interesting can be observed as both variables were in their final state before any checking occured, so no need to record those events
//		if((x_first_char != 'b')||(y_first_char != 'b'))
		if((x_first_char != lastset[0])||(y_first_char != lastset[1]))	//if this entry does not match the last one
		{
			check_log.push_back(x_first_char);
			check_log.push_back(y_first_char);
			check_log.push_back(' ');
			//only unique combination are really interesting, but I'll settle for just preventing streams of duplicates
			lastset[0] = x_first_char;
			lastset[1] = y_first_char;
		}
		y.store(0);
		x.store(0);
	}
	for(unsigned int i = 0; i < check_log.size(); i++)
	{
		std::cout << check_log.at(i);
	}
	std::cout << std::endl;
	return 0;
}

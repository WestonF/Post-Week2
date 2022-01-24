#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

bool finished = false;
std::mutex hail_locker;
void nexthail (std::vector<int> &hail)
{
	thread_local int temp = 0;
	while(finished != true)
	{
		if(hail_locker.try_lock())
		{
			temp = hail.back();
			//just to help existing race conditions to actually cause issues
			for(int j = 0; j < 1000; j++)	//not sure how adding this loop managed to push the program to recursively call terminate, but I'm impressed.	//with addition of mutex, this loop no longer causes the program to crash
			{
			}
	 
			if(temp % 2)	//if last element of hail is odd
			{
				temp = temp * 3 + 1;
				hail.push_back(temp);
			}
			else
			{
				temp /= 2;
				hail.push_back(temp);
			}

			hail_locker.unlock();
		}

		if (hail.size() >= 3)
		{
			if (hail.at(hail.size() - 1) == 1)
			{
				if(hail.at(hail.size() - 2) == 2)
				{
					if(hail.at(hail.size() - 3) == 4)
					{
						finished = true;
					}
				}

			}
			else if (hail.at(hail.size() - 1) == 2)
			{
				if(hail.at(hail.size() - 2) == 4)
				{
					if(hail.at(hail.size() - 3) == 1)
					{
						finished = true;
					}
				}

			}
			else if (hail.at(hail.size() - 1) == 4)
			{
				if(hail.at(hail.size() - 2) == 1)
				{
					if(hail.at(hail.size() - 3) == 2)
					{
						finished = true;
					}
				}

			}
		}
	}
}
int main()
{
	std::vector<int> hail;
	hail.push_back(257);
//	bool finished = false;
//	nexthail(hail, finished);
//	std::thread t1{nexthail, std::ref(hail), std::ref(finished)};
	std::thread t1{nexthail, std::ref(hail)};
	std::thread t2{nexthail, std::ref(hail)};
	std::thread t3{nexthail, std::ref(hail)};
	std::thread t4{nexthail, std::ref(hail)};
	std::thread t5{nexthail, std::ref(hail)};
	std::thread t6{nexthail, std::ref(hail)};
	std::thread t7{nexthail, std::ref(hail)};
	std::thread t8{nexthail, std::ref(hail)};
	std::thread t9{nexthail, std::ref(hail)};
	std::thread t10{nexthail, std::ref(hail)};
	std::thread t11{nexthail, std::ref(hail)};
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
	t9.join();
	t10.join();
	t11.join();


	for(int i = 0; i < hail.size() - 1; i++)
	{
		std::cout << hail.at(i) << ", ";
	}
	std::cout << hail.at(hail.size() - 1) << std::endl;

	return 0;

}

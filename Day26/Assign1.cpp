#include <iostream>
#include <future>
#include <vector>
#include <cassert>

//so today I get to learn how to actually learn iterators on top of everything else I need to catch up on. Well, it had to happen eventually

//random placeholder test
bool demotest(int thing)
{
	return (bool)(thing % 2);
}

//sequential any_of
template<typename itt, class T>
bool wf_any_of( bool(test)(T), itt begin, itt end, std::promise<itt>* prom, std::atomic<bool>* job_done)
{

	for(itt iter = begin; iter != end; ++iter)
	{
		if(job_done->load())
		{
			return false;
		}

		if (test(*iter))
		{
			prom->set_value(iter);
			job_done->store(true);
			return true;
		}
	}

	return false;
}


template<typename itt, class T>
bool parallel_any_of(bool(test)(T), itt begin, itt end, std::promise<itt>* prom)
{
	//bool(any*)(bool()(T), itt, itt, std::promise<itt>*, std::atomic<bool>*) = &wf_any_of;
	auto any = &wf_any_of<itt, T>;
	constexpr int threadcount = 4; // a good program would have some hardware thread test and use that to pick the number of threads
	std::atomic<bool> job_done(false);

	std::vector<std::thread> threads;
	int size = std::distance(begin, end); 
	if(size < 25)
	{
		return wf_any_of(test, begin, end, prom, &job_done);
		//the return actually has no meaning, but calling the function still makes the promise set the value of the future
	}


	int block_size = size / 4;

	for(int i = 0; i < threadcount - 1; i++)
	{
		itt t_begin = begin;
		itt t_end = begin;
	
		std::advance(t_begin, i * size);
		std::advance(t_end, ((i + 1) * size) - 1);
		
		threads.push_back(std::thread(any, test, (t_begin < end ? t_begin: end), (t_end < end ? t_end : end), prom, &job_done));
	}
	std::advance(begin, (threadcount - 1) * block_size);
	threads.push_back(std::thread(any, test, begin, end, prom, &job_done));	//catch any last elements that would have been lost due to rounding

	for(int i = 0; i < threads.size(); i++)
	{
		std::cout << "awaiting join" << std::endl;
		threads.at(i).join();
	}
	
	return job_done.load();
}




int main()
{
	std::vector<int> thingvec(100000, 2);
	std::promise<std::vector<int>::iterator> prom;
	std::future<std::vector<int>::iterator> fut = prom.get_future();
//	thingvec[5] = 1;
	thingvec.push_back(19);
	auto fpoint = &demotest;
	bool test = parallel_any_of(fpoint, thingvec.begin(), thingvec.end(), &prom);
	assert(thingvec.begin() < thingvec.end());
	auto fuut = fut.get();
	std::cout << "future: " << *fuut << std::endl;
	std::cout << "at element: " << std::distance(thingvec.begin(), fuut) << std::endl;
	assert(test);
	return 0;
}

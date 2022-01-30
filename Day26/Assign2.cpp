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
bool wf_nany_of( bool(test)(T), itt begin, itt end, std::promise<itt>* prom, std::atomic<bool>* job_done)
{

	for(itt iter = begin; iter != end; ++iter)
	{
		if(job_done->load())
		{
			return true;
		}

		if (!test(*iter))
		{
			prom->set_value(iter);
			job_done->store(true);
			return false;
		}
	}

	return true;
}


template<typename itt, class T>
bool parallel_nany_of(bool(test)(T), itt begin, itt end, std::promise<itt>* prom, std::atomic<bool>* job_done, int min_to_split = 2500)
{
	int size = std::distance(begin, end); 
	if(size < min_to_split)
	{
		return wf_nany_of(test, begin, end, prom, job_done);
	}
	//bool(any*)(bool()(T), itt, itt, std::promise<itt>*, std::atomic<bool>*) = &wf_any_of;
	//auto any = &wf_any_of<itt, T>;
	//constexpr int threadcount = 4; // a good program would have some hardware thread test and use that to pick the number of threads

	//std::vector<std::thread> threads;
	itt midpt = begin;
	std::advance(midpt, size / 2);
	
	auto handle = std::async(std::launch::async, parallel_nany_of<itt, T>, test, midpt, end, prom, job_done, min_to_split); 

	bool test1 = parallel_nany_of<itt, T>(test, begin, midpt, prom, job_done, min_to_split);
	bool test2 = handle.get();

	return (test1 || test2);
}




int main()
{
	std::vector<int> thingvec(100000, 3);
	std::promise<std::vector<int>::iterator> prom;
	std::future<std::vector<int>::iterator> fut = prom.get_future();
//	thingvec[5] = 1;
	thingvec.push_back(20);
	std::atomic<bool> job_done(false);
	auto fpoint = &demotest;
	bool test = parallel_nany_of(fpoint, thingvec.begin(), thingvec.end(), &prom, &job_done);
	assert(thingvec.begin() < thingvec.end());
	auto fuut = fut.get();
	std::cout << "future: " << *fuut << std::endl;
	std::cout << "at element: " << std::distance(thingvec.begin(), fuut) << std::endl;
	assert(test);
	return 0;
}

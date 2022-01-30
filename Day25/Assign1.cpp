#include <vector>
#include <thread>
#include <numeric>
#include <iostream>
#include <future>
//#include <boost/multiprecision/cpp_int.hpp>
//using namespace boost::multiprecision;

template<typename Iterator,typename T>
struct accumulate_block
{
	void operator()(Iterator first, Iterator last, std::promise<T> result)
	{
		result.set_value(std::accumulate(first, last, T{}));	//attempt to create an instance of T with default constructor. This is as close to a generalized zero-initialization as I can think of.
	}
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
   	unsigned long const length=std::distance(first, last);
	if (!length)
		return init;
	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length+min_per_thread-1)/min_per_thread;
	unsigned long const hardware_threads = std::thread::hardware_concurrency();
	unsigned long const num_threads = std::min(hardware_threads!=0?hardware_threads:2,max_threads);
	std::cout << "num_threads: " << num_threads << std::endl;
	unsigned long const block_size = length/num_threads;
	//std::vector<T> results(num_threads);
	//std::vector<std::promise<T>> mypromises;
	std::vector<std::future<T>> myfutures;
	std::vector<std::thread> threads(num_threads-1);
	Iterator block_start = first;
	for (unsigned long i = 0; i < (num_threads-1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);	//review what advance does
		//threads[i] = std::thread(accumulate_block<Iterator,T>(), block_start, block_end,std::ref(results[i]));
		std::promise<T> newpromise;
		myfutures.push_back(newpromise.get_future());
		threads[i] = std::thread(accumulate_block<Iterator,T>(), block_start, block_end, std::move(newpromise));
		block_start = block_end;
	}
	std::promise<T> newpromise;
	myfutures.push_back(newpromise.get_future());
	accumulate_block<Iterator,T>() (block_start, last, std::move(newpromise));
    	for (auto& entry: threads)
        	entry.join();
	//return std::accumulate(results.begin(), results.end(), init);
	if(myfutures.size() == 0 )
	{
		return T{};
	}
	
	T results = myfutures.at(0).get();
	
	for(unsigned long i = 1; i < myfutures.size(); i++)
	{
		results += myfutures.at(i).get();
	}
	return results;
}

int main(void) 
{
	constexpr long LEN = 20000;
	std::vector<long> myvec;
	for (long ii = 0; ii < LEN; ii++) 
	{
        	myvec.push_back(ii);
	}	
	long sum = parallel_accumulate(myvec.begin(), myvec.end(), 0);
	std::cout << "sum: " << sum << std::endl;
}

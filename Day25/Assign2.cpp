#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <cassert>
#include <iostream>

template <typename RandomIt, typename T>
void parallel_sum(RandomIt beg, RandomIt end, std::promise<T>& sum_prom)//need to spend some more time studying different ways of passing promises to functions, threads and std::async objects, but need to catch up first :( 
{
    auto len = end - beg;
    
    if (len < 1000)
    {
	sum_prom.set_value(std::accumulate(beg, end, 0));
	return;
    }

    RandomIt mid = beg + len/2;
    std::promise<T> firsthalf_prom;
    std::future<T> firsthalf_fut = firsthalf_prom.get_future();
    std::promise<T> secondhalf_prom;
    std::future<T> secondhalf_fut = secondhalf_prom.get_future();
    //auto handle = std::async(std::launch::async, parallel_sum<RandomIt, T>, mid, end, secondhalf_prom);
    //auto handle = std::async(std::launch::async, parallel_sum<RandomIt, T>, mid, end, std::move(secondhalf_prom));
    auto handle = std::async(std::launch::async, parallel_sum<RandomIt, T>, mid, end, std::ref(secondhalf_prom));
     // recursive if n >= 1000
    //int sum = parallel_sum(beg, mid);
    //return sum + handle.get();
    
    parallel_sum(beg, mid, firsthalf_prom);
    sum_prom.set_value(firsthalf_fut.get() + secondhalf_fut.get());
}

int main() {
    std::vector<int> v(2000, 4);
    std::promise<int> sum_prom;
    std::future<int> sum_fut = sum_prom.get_future();
	//parallel_sum<decltype(v.begin()), int>(v.begin(), v.end(), sum_prom);
	parallel_sum(v.begin(), v.end(), sum_prom);
    assert(sum_fut.get() == 8000);
    //std::cout << "Didn't crash" << std::endl;
}

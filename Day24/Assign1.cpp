#include<mutex>
#include<condition_variable>
#include<thread>
#include<queue>
#include<iostream>
#include<sstream>

#define SAMPLE 5

std::string logstring = "";
std::stringstream log(logstring);
std::mutex logex;
class task
{
	static int task_count;
	int task_id;
	public:
		task():task_id(++task_count)
		{
			std::lock_guard<std::mutex> logtask(logex);
			log << "d" << task_id;	
		}
		task(const task& other):task_id(other.task_id)
		{
			std::lock_guard<std::mutex> logtask(logex);
			log << "c" << task_id;	
		}
		task(task&& other)
		{
			task_id = other.task_id;
			task_count++;
			other.task_id = task_count;
			std::lock_guard<std::mutex> logtask(logex);
			log << "m" << task_id;	
		}
		task& operator =(const task& other)
		{
			if(&other == this)
			{
				return *this;
			}
			std::lock_guard<std::mutex> logtask(logex);
			log << "(" << other.task_id << "->" << task_id << ")";
			task_id = other.task_id;
			return *this;
		}
		task& operator =(task&& other)
		{
			if(&other == this)
			{
				return *this;
			}
			
			std::lock_guard<std::mutex> logtask(logex);
			log << "(" << other.task_id << "->" << task_id << ", ";
			task_id = other.task_id;
			task_count++;
			other.task_id = task_count;
			log << other.task_id << "->" << task_id << ")";
			return *this;
		}
		~task()
		{
			std::lock_guard<std::mutex> logtask(logex);	// putting a lock on a destructor is probably a really risky move, but I'm just messing around with the string stream anyway
			log << "~" << task_id;
		}

		void operator()()
		{
			waste_time();
		}

		void waste_time()
		{
			volatile int c = 5;
			for (int i = 0; i < 1000; i++)
			{
				if(c != 5)
				{
					break;
				}
			}
		}
};

int task::task_count = 0;


template <typename T>
class threadsafe_queue 
{
	private:
		mutable std::mutex mut;
		std::queue<T>data_queue;
		std::condition_variable data_cond;
	public:
		threadsafe_queue(){}
		threadsafe_queue(threadsafe_queue const& other)
		{
			std::lock_guard<std::mutex> lk(other.mut);
			data_queue = other.data_queue; 
		}
		
		void push(T new_value)
		{
			std::lock_guard<std::mutex> lk(mut);
			data_queue.push(new_value);
			data_cond.notify_one();
		}

		void wait_and_pop(T& value)
		{
			std::unique_lock<std::mutex> lk(mut);
			data_cond.wait(lk, [this]{return !data_queue.empty();});
			value = data_queue.front();
			data_queue.pup();
		}

//end column on slide

		std::shared_ptr<T> wait_and_pop()
		{
			std::unique_lock<std::mutex> lk(mut);
			data_cond.wait(lk, [this]{return !data_queue.empty();});
			std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
			data_queue.pop();
			return res;
		}

		bool try_pop(T& value)
		{
			std::lock_guard<std::mutex> lk(mut);
			if(data_queue.empty())
				return false;
			value = data_queue.front();
			data_queue.pop();
			return true;
		}
		std::shared_ptr<T> try_pop()
		{
			std::lock_guard<std::mutex> lk(mut);
			if(data_queue.empty())
				return std::shared_ptr<T>();
			std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
			data_queue.pop();
			return res;
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.empty();
		}
};


void fill_queue_with_trash (threadsafe_queue<task>& Q)
{
	task one;
	task two;
	task* three = new task;
	//std::unique_ptr<task> three = std::make_unique<task>();
	//push() ALWAYS creates a copy, which is what it actally keeps. This is good, otherwise all these local variables would die at end of function, and the queue would be full of empty/void element
	//often makes more than one copy as part of the process, but only keeps ownership of one (per push()).
	Q.push(one);
	Q.push(std::move(two));	
	Q.push(*three);
	delete(three);	//this is important because the pointer didn't get passed. push() used the pointer to find three, then make a copy of the data there. The queue no longer cares what is at the address of three
}

void pretend_to_get_work_done(threadsafe_queue<task>& Q)
{
	task task_by_ref;
	if(!Q.try_pop(task_by_ref))
	{
		fill_queue_with_trash(Q);
	}
	
	std::shared_ptr task_by_ptr = Q.try_pop();
	if(task_by_ptr != nullptr)
	{
		std::unique_ptr<task> new_task = std::make_unique<task>();
		Q.push(*new_task);	//still just making a copy to keep, leaving the original and discarding the location of the original
	}
}

void clean_up_after_useless_co_threads(threadsafe_queue<task>& Q)
{
	while(Q.try_pop());
}

int main()
{
	threadsafe_queue<task> Q;
	std::thread t1(fill_queue_with_trash, std::ref(Q));
	std::thread t2(pretend_to_get_work_done, std::ref(Q));
	std::thread t3(pretend_to_get_work_done, std::ref(Q));
	std::thread t4(fill_queue_with_trash, std::ref(Q));
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	std::thread t5(clean_up_after_useless_co_threads, std::ref(Q));
	std::thread t6(clean_up_after_useless_co_threads, std::ref(Q));
	t5.join();
	t6.join();

	logstring = log.str();

	return 0;
}


/*
struct data_chunk{int x;};

data_chunk prepare_data(int ii) 
{
	data_chunk d = {ii};
	std::cerr <<"Prepare: " << d.x << std::endl;
	return d;
}

void process(data_chunk& d)
{
	std::cerr << "Processing: " << d.x << std::endl;
}
bool is_last_chunk(data_chunk& d)
{
	return (d.x == SAMPLE - 1);
}

std::mutex mut;
std::queue<data_chunk>
data_queue;
std::condition_variable data_cond;

void data_preparation_thread() 
{
	for(int ii = 0; ii < SAMPLE; ii++)
	{
		data_chunk const data = prepare_data(ii);
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}
}
*/




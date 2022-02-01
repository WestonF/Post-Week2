#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <coroutine>
//there's 12 elements
const std::string accountIds[] = {"5", "52", "12", "98", "055", "356", "8789", "122", "89", "66", "36", "789"};  

enum myenum
{
	credit,
	debit
};

struct transaction
{
	long post_date;
	std::string src_accountId;
	std::string dest_accountId;
	myenum tranType;
	double amount;

	static int transcount;	
	transaction(long i_post_date, std::string i_src_accountId, std::string i_dest_accountId, myenum i_tranType, double i_amount):post_date(i_post_date), src_accountId(i_src_accountId), dest_accountId(i_dest_accountId), tranType(i_tranType), amount(i_amount)
	{
		std::cout << ++transcount << std::endl;
	}
};

int transaction::transcount = 0;

//this is the no-coroutine version of the function desired
std::unique_ptr<transaction> not_co()
{
	std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	long  post_date= now_time;
	std::string src_accountId = accountIds[rand() % 12];
	std::string dest_accountId = accountIds[rand() % 12];
	while(dest_accountId == src_accountId)
	{
		dest_accountId = accountIds[rand() % 12];
	}
	myenum tranType = static_cast<myenum>(rand() % 2);
	double amount  = rand() % 10000;
	return std::make_unique<transaction>(post_date, src_accountId, dest_accountId, tranType, amount);
}

//functor version of what I want
class notco
{
	public:
	std::unique_ptr<transaction> operator()(long post_date, std::string src_accountId, std::string dest_accountId, myenum tranType, double amount)
	{
		return std::make_unique<transaction>(post_date, src_accountId, dest_accountId, tranType, amount);
	}
	std::unique_ptr<transaction> operator()()
	{
		std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		long post_date = now_time;
		std::string src_accountId = accountIds[rand() % 12];
		std::string dest_accountId = accountIds[rand() % 12];
		while(dest_accountId == src_accountId)
		{
			dest_accountId = accountIds[rand() % 12];
		}
		myenum tranType = static_cast<myenum>(rand() % 2);
		double amount  = rand() % 10000;
		return std::make_unique<transaction>(post_date, src_accountId, dest_accountId, tranType, amount);
	}
};

//co-routine functor
//this is not a co-routine. this is an object for having coroutines (that make coroutines(?)) that make things
struct Generator 
{

	struct promise_type;	//forward declaration so handle definition is legal
	std::coroutine_handle<promise_type> comake;

	Generator(std::coroutine_handle<promise_type> h): comake(h){}	//get handle from external source so that external source has access

	~Generator()
	{
		//if coroutine exists? if coroutine handle exists?
		if(comake)
		{
			comake.destroy();	// kill the coroutine
		}
	}
	// Destructor defined, Rule of Three, dont want to allow copying
	Generator(const Generator&) = delete;
	Generator& operator = (const Generator&) = delete;
	// Rule of Five
	Generator(Generator&& source): comake(source.comake)
	{
		source.comake = nullptr;
	}
	Generator& operator = (Generator&& source)
	{
		if(&source == this)
		{
			return *this;
		}
		comake = source.comake;
		source.comake = nullptr;
	}


	//adapted, details unclear for now
	std::unique_ptr<transaction> getTrans()
	{
		//co_resume?
		//creates(?) a promise object of type promise_type, tries to get data from its data member
		comake.resume();
		return std::move(comake.promise().trans_ptr);
	}
	
	struct promise_type
	{
		//default con/destructors fine

		//always suspend on creation, do nothing
		std::suspend_always initial_suspend()
		{
			return{};
		}

		//not sure how important suspend_always vs suspend_never is as a return type. suspend always from final suspend seems suspect, but I'm just modifying the example while I learn how it ticks
		std::suspend_always final_suspend() noexcept
		{
			return{};
		}

		//should be a unique pointer to a transaction? I really hate use of auto in examples, and rather dislike their existance, as they complicate disambiguation
		auto get_return_object()
		{
			//get a coroutine handle from promise object. not 100% what that actually means though
			return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
		}
		
		std::suspend_always yield_value(std::unique_ptr<transaction>&& input)
		{
			// the following comment is very wrong, but I'm saving it to examine later to better understand the progression of my thought processes
			// I think this is where the promist actually designates the generator's return by assigning it to a location accessable from the generator's getNextValue
			trans_ptr = std::move(input);
			return {};
		}

		//I think this is one of the required functs
		void return_void(){};

		void unhandled_exception()
		{
			std::exit(1);
		}

		//data member of promise_type
		std::unique_ptr<transaction> trans_ptr;
	};
};
Generator MakeTrans()
{
	while(true)
	{
		co_yield not_co();	// this should push a transaction pointer to the Generator's getTrans... I think
		//old = std::move(tp);
	}
}
int main()
{
	auto catcher = MakeTrans();	//if not a coroutine, would ge5t stuck forever in MakeTrans's while(true) loop

	//not calling value from MakeTrans()correctly
	
	//std::unique_ptr<transaction> trans1 = catcher.getTrans();
	auto trans1 = catcher.getTrans();
	auto trans2 = catcher.getTrans();
	auto trans3 = catcher.getTrans();

	std::cout << "post_date(long): " << trans2->post_date << ", scr_accountId: " << trans2->src_accountId << ", dest_accountId: " << trans2->dest_accountId << ", tranType(as int): " << (int)trans2->tranType << ", amount: " << trans2->amount << std::endl;
	return 0;
}

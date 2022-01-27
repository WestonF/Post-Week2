#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <cstdlib>
#include <chrono>

// 1.35 dollars ~= 1 pound
const double threshhold_rate = 1.35;
// 1 / 1.35 ~= 0.74 pound ~= 1 dollar
enum class MONEY
{
	POUND,
	DOLLAR
};

enum class STATUS
{
	PENDING,
	ACCEPTED,
	REJECTED
};

class country;

class request
{
	public:
	country* const source;
	const MONEY type;
	const double amount;
	const double rate;	// rate defined as # of type per 1 of other money
	const bool buy;
	STATUS status;
	public:
	request(MONEY i_type, double i_amount, double i_rate, bool i_buy, country* i_source):type(i_type), amount(i_amount), rate(i_rate), buy(i_buy), source(i_source), status(STATUS::PENDING){}

	request(country* i_source):source(i_source), type(static_cast<MONEY>(rand() % 2)), amount(rand() % 100000), rate(static_cast<bool>(type) ? ((rand()%10)/100. + 1.3):((rand()%10)/100. + 0.7)), buy(static_cast<bool>(rand() % 2)), status(STATUS::PENDING){}	//generate a request with pseudo-random values for all members except source and status. values of rate are influenced by which type of money is held by the request, with pseudo-random variation centered at roughly the threshhold rate

};

class country
{
	public:
	double pound_res;
	double dollar_res;
	//std::vector<request>* reqlog;	//pointer to a vector of request objects
	public:
	std::mutex transex;
	std::vector<std::unique_ptr<request>> trans_log;
	country():pound_res(100000), dollar_res(200000){}
	country(double pound_start, double dollar_start):pound_res(pound_start), dollar_res(dollar_start){}

	std::unique_ptr<request> req;

	std::unique_ptr<request> CreateRequest()
	{
		return std::make_unique<request>(this);
	}

	std::unique_ptr<request> CreateRequest(MONEY type, double amount, double rate, bool buy)
	{
		return std::make_unique<request>(type, amount, rate, buy, this);
	}

	void AssessRequest()
	{
		if((req->buy == true)&&(req->type == MONEY::DOLLAR)&&(req->amount <= dollar_res)&&(req->rate >= threshhold_rate))	// if buying dollars at a rate of more than 1.35 dollars per pound AND buying fewer dollars than we own
		{
			//do confirm stuff
			req->status = STATUS::ACCEPTED;
		}
		else if((req->buy == true)&&(req->type == MONEY::POUND)&&(req->amount <= pound_res)&&(req->rate >= (1 / threshhold_rate)))	// if buying pounds at a rate of more than 0.74 pounds per dollar AND buying fewer pounds than we own
		{
			//do confirm stuff
			req->status = STATUS::ACCEPTED;
		}
		else if((req->buy == false)&&(req->type == MONEY::DOLLAR)&&(req->amount <= (pound_res / req->rate))&&(req->rate <= threshhold_rate))	//if selling dollars at a rate of less than 1.35 dollars per pound and pound cost less than number of pounds owned
		{
			//do confirm stuff
			req->status = STATUS::ACCEPTED;
		}
		else if((req->buy == false)&&(req->type == MONEY::POUND)&&(req->amount <= (dollar_res / req->rate))&&(req->rate <=(1 / threshhold_rate)))	// if selling pounds at a rate of less than 0.74 pounds per dollar and dollar cost less than number of dollars owned
		{
			//do confirm stuff
			req->status = STATUS::ACCEPTED;
		}
		else
		{
		//do reject stuff
		req->status = STATUS::REJECTED;
		}
	}
	
	//take a unique pointer to a request and store it to internal unique pointer "req"
	void ClaimRequest(std::unique_ptr<request> in_req)
	{
		req = std::move(in_req);
	}
	
	std::unique_ptr<request> SurrenderRequest()
	{
		return std::move(req);
	}

	void CheckRequest()
	{
		//might have had to do some locking for these checks if I weren't passing around unique_ptrs, so as long as this country owns the pointer, no other country can look at the request. Probably inefficient, but seems okay

		if(req->status == STATUS::PENDING)
		{
			//if pending, and not from self, accept or reject it
			AssessRequest();
			//member variable req is still loaded with the unique_ptr, but status should have been updated to accepted or rejected
		}
		
		if(req->status == STATUS::ACCEPTED)	//if accepted, start the transaction. no need to pass the request back to the box. ACT NOW!
		{
			// do transaction
			DoTransaction();	//are my function names too literal? Nah!
			return;
		}
		
		if(req->status == STATUS::REJECTED)	//if another country Assessed this request and rejected it, it needs to be deleted
		{
			//do delete stuff
			req.reset();	//delete the object pointed to by req, set contents of req to nullptr
			return;
		}
			// once the transaction succeeds/fails, req will fall through to here, and get passed back to the box ONCE SURRENDERREQUEST IS CALLED, where it will be nullptr checked/deleted. This is an appropriate course of action for all the preceeding conditions. Tying ClaimRequest and SurrenderRequest into the system seems like it would be good, but trying to keep the box unlocked as much as possible might complicate that. For now, just always have to Claim -> Check -> Surrender or behavior is undefined

	}	
	
	void DoTransaction()
	{

		double pound_amount;
		double dollar_amount;
		//I might have gotten the rate math wrong, and it might need to be inverted, but high-quality monetary conversions are not really the focus of this exercise
		if(req->type == MONEY::POUND)
		{
			pound_amount = req->amount;
			dollar_amount = req->amount * req->rate;
			if(req->buy)
			{
				pound_amount *= -1;	//*this will be decreasing pounds owned
			}
			else
			{
				dollar_amount *= -1;	// *this will be decreasing dollars owned
			}
		}
		else	//req->type == MONEY::POUND
		{
			dollar_amount = req-> amount;
			pound_amount = req-> amount * req->rate;

			if(req->buy)
			{
				dollar_amount *= -1;	//*this will be decreasing dollars owned
			}
			else
			{
				pound_amount *= -1;	// *this will be decreasing pounds ownedi
			}
		}
		//none of the above actually changes any country or request's members, nor does it rely on checking any variables(all members of request other than status are const), so no locks needed before here



		//lock access to the dollar_res and pound_res for both *this and req->source	NOTE: these values may still be examined elsewhere, but this is the only location where they are modified. Nothing is preventing those values from being modified by direct calls to the members elsewhere, except that there are no direct calls to the members elsewhere. could push them to private, and add a set of Check(), Withdraw(), and Deposit() functions and transex lock withdraw and deposit, but that seems like overkill in this scenario (Check() wouldn't need a lock because it doesn't change anything, so the location from which it was called would need to manage the locking if it was critical for the results of Check() to remain accurate)

		//not using a lock_guard here made for some really odd possible lock/unlock timing, particularly managing unlocking req->source->transex under both the condition when the transaction failed and req still had the pointer to the other country (adn thereby its mutex) vs when the transaction completed successfully and the pointer to the other country was now stored at this->trans_log.back(). Guards make this much simpler.

	
	//		THIS IS THE BAD VERSION THAT CAUSES DEADLOCK	
	/*
		//if I see "first lock" in the console window without seeing a "second lock", there is a deadlock

		std::cout << "first lock" << std::endl;
		std::lock_guard<std::mutex> this_country(transex);

		//add a wait/sleep here to maximize odds of deadlock
		std::this_thread::sleep_for(std::chrono::seconds(3));

		std::lock_guard<std::mutex> other_country(req->source->transex);

		std::cout << "second lock" << std::endl;
		
	*/
	//	END OF INTENTIONALLY BAD VERSION
		


		//if both accounts still have enough money to enact the transaction	// transaction's signs are inverted for req->source, but must still be greater than zero
		if((dollar_res + dollar_amount >= 0) && (pound_res + pound_amount >= 0) && (req->source->dollar_res - dollar_amount >= 0) && (req->source->pound_res - pound_amount))
		{
			//actually enact the transaction
			dollar_res		+= dollar_amount;
			pound_res		+= pound_amount;
			//transaction's signs are inverted for req->source. Also, either dollar_amount or pound_amount should be negative
			req->source->dollar_res	-= dollar_amount;
			req->source->pound_res	-= pound_amount;

			//std::cout << "TRADE!" << std::endl;

			trans_log.push_back(std::move(req));	//steal the request and save it to the country's log. should fill req with a nullptr
		}
		//transaction is complete OR transaction failed due to insufficient currency.
		//push req back onto the req_box, check if get() is nullptr, and delete the element if it is? seems okay. this will preserve accepted transactions that were temporarily unable to be completed

	}//end of function "DoTransaction" both lock_guards lose scope and unlock both country's transex mutexes
};
//this is the function I want to pass to the threads
void BeCountry(std::vector<std::unique_ptr<request>>& req_box, bool& done_trading, std::mutex& box_ex)
{
	//thread_local country C;						//country should be unique per thread
	country C;								//country should be unique per thread

//	std::vector<std::unique_ptr<request>> req_box;				//req_box should be shared between the threads
//	auto lastthread = this_thread::get_id;
	
	//this was a nice clean program flow when I started
	while(!done_trading)
	{
		box_ex.lock();						//should prevent the box from becoming empty between the empty check and calling .back()
		if(req_box.size() <= 5)
		{
			//if request box is empty, make something to put in it
			req_box.push_back(C.CreateRequest());		//this is still protected by the lock preceding the empty check, but it is possible for another thread to currently be in the middle of CheckRequest(), which is actually what I want
			//std::cout << "Created!" << std::endl;
			box_ex.unlock();	
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		else	// if there are enough things in the box
		{	
			if(req_box.back()->source != &C)	//if top item was not created by *this
			{
				C.ClaimRequest(std::move(req_box.back()));	//claim it
				req_box.pop_back();				//remove the newly emptied unique pointer from the vector
			}
			else	//box_ex is locked and req_box is known to have at least 5 elements
			{
				for(int i = 0; i < req_box.size() - 1; i++)	//dig through elements of box to see if anything was not created by *this
				{
					if(req_box.at(i)->source != &C)
					{
						C.ClaimRequest(std::move(req_box.at(i)));	//get the request
						req_box.erase(req_box.begin() + i);		//erase the container element, as it is now empty
						break;
					}
				}	//if none are found, req.get()should return nullptr, and skip the next code segment
			}
	
			box_ex.unlock();
		}
	
		if(C.req.get() != nullptr)
		{
			C.CheckRequest();
			box_ex.lock();
			req_box.push_back(C.SurrenderRequest());
			if(req_box.back().get() == nullptr)	//if the surrender request just dumped junk, delete the vector element
			{
				req_box.pop_back();
			}
			box_ex.unlock();
		}

		/*		std::cout << "Claimed!" << std::endl;
				box_ex.unlock();				//allow other threads to access the box while this thread checks the claimed request
				
				C.CheckRequest();				//CheckRequest *probably* takes a negligible amount of time, but I like this structure.
					
				box_ex.lock();					// re-lock access to the box so the modified request can be put back in the box
				req_box.push_back(C.SurrenderRequest());	//put updated request back into box
				if(req_box.back().get() == nullptr)
				{
					req_box.pop_back();			//if request was deleted, delete this vector element
				}
				box_ex.unlock();	
			}
			else	//if top item WAS created by *this, see if there is anything in the pile that was not created by this
			{	//box_ex is still locked
				int count = 0;
				while(count < req_box.size())	//if there is something to grab, find it
				{
					if(req_box.at(count)->source != &C)	//if an item is found that was not created by *this
					{
						C.ClaimRequest(std::move(req_box.at(count)));	//claim the request
						req_box.erase(req_box.begin() + count);		//purge the empty husk

						box_ex.unlock();
						C.CheckRequest();
						box_ex.lock();
						req_box.push_back(C.SurrenderRequest());	//put updated request back into box
						break;						//got a thing, did a thing, now back to the top
					}
					count++;
				}
				box_ex.unlock();

			}
		}// if(req_box is smaller than 6)

*/
		if(C.trans_log.size() >= 5)
		{
			done_trading = true;
			//std::cout << "Done" <<std::endl;
			//could also have while(true) and a break here, but that would leave the other thread/country stuck in the while forever, which would be bad
		}
		//std::cout << "end loop" << std::endl;
	}//end (!done_trading) while loop
}
int main()
{
	bool done_trading = false;
	std::vector<std::unique_ptr<request>> req_box;			//req_box should be shared between the threads
	std::mutex box_ex;

	std::thread t1{BeCountry, std::ref(req_box), std::ref(done_trading), std::ref(box_ex)};
	std::thread t2{BeCountry, std::ref(req_box), std::ref(done_trading), std::ref(box_ex)};
	
	t1.join();
	t2.join();

	//std::cout << "Actually done" <<std::endl;
	return 0;
}

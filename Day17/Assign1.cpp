#include <iostream>
#include <cassert>
#include <type_traits>

//Exchange rates:	1.14	dollar per euro
//			0.0087	dollar per yen
//			1.37	dollar per pound
//			1.00	dollar per dollar
class money
{
	double dollar_rate;	//this is a terrible long-term method, but it will do for this excercise
	
	public:
	money(double rate, double i_amount):dollar_rate(rate), amount(i_amount){}
	
	//virtual definition must have no semicolon, may have empty body
	virtual ~money(){}	//virtual destructor still called by derived classes
	//pure virtual "definition" must have no body, = 0, and a semicolon
	//virtual ~money() = 0;	pure virtual destructor breaks everything, no real surprise, as we do actually need to destroy the money created by each derived class of money
	double amount;
	double rate()
	{
		return dollar_rate;
	}

	double operator ()()
	{
		return amount;
	}
};

class Dollar : public money
{
	public:
	Dollar(double amount):money(1.00, amount){}
};

class Euro : public money
{
	public:
	Euro(double amount):money(1.14, amount){}
	~Euro(){}
};

class Yen : public money
{
	public:
	Yen(double amount):money(0.0087, amount){}
	~Yen(){}
};

class Pound : public money
{
	public:
	Pound(double amount):money(1.37, amount){}
	~Pound(){}
};


template<class T, class C = int>
class Account
{
private:
	T amt;
public:
	//constructor
	Account(T, double amount) : amt(amount){}
	Account(double amount) : amt(amount){}


	template<class Q>
	void deposit(Q amount)
	{
		if(std::is_same_v<decltype(T), decltype(Q)>)
		{
			std::cout << "NO" << std::endl;
			return;
		}
		std::cout << "YES" << std::endl;
		return;
	}
	
	template<class Q = int>
	void withdraw(Q amount)
	{}

	//template<class R = int>
	//Account<R,C>* currency(R)
	//{}
};

int main()
{
	double amount = 5;
	Dollar d(amount);
	Account<Dollar> a(amount);
	Account<Euro> b(amount);

	a.deposit(d);
	b.deposit(d);
	//Account<Dollar> c = Account<Dollar>::Account(amount);
	//Account<Dollar> c = Account<Dollar>::Account<Dollar>(amount);		//Most Vexing Parse is indeed vexing
	//Account d(Dollar, amount);
	return 0;
}

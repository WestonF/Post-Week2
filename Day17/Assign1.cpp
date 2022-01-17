#include <iostream>
#include <cassert>
#include <type_traits>

//Exchange rates:	1.14	dollar per euro
//			0.0087	dollar per yen
//			1.37	dollar per pound
//			1.00	dollar per dollar
class money
{
	protected:
	double dollar_rate;	//this is a terrible long-term method, but it will do for this excercise
	
	public:
	money(double rate, double i_amount):dollar_rate(rate), amount(i_amount){}
	
	//virtual definition must have no semicolon, may have empty body
	virtual ~money(){}	//virtual destructor still called by derived classes
	//pure virtual "definition" must have no body, = 0, and a semicolon
	//virtual ~money() = 0;	pure virtual destructor breaks everything, no real surprise, as we do actually need to destroy the money created by each derived class of money
	double amount;
	double rate() const
	{
		return dollar_rate;
	}

	double operator ()() const
	{
		return amount;
	}
	double operator + (double input) const
	{
		return amount + input;
	}
	double operator - (double input) const
	{
		return amount - input;
	}
	money& operator += (double input)
	{
		amount = amount + input;
		return *this;
	}

	money& operator -= (double input)
	{
		amount = amount - input;
		return *this;
	}

	operator double() const
	{
		return amount;
	}
};

class Dollar : public money
{
	public:
	Dollar(double amount):money(1.00, amount){}
	Dollar(const Dollar& other):money(other.rate(), other.amount){};
	Dollar& operator = (const Dollar& other)
	{
		if(&other == this)
		{
			return *this;
		}
		dollar_rate = other.rate();
		amount = other.amount;
		return *this;
	}
	//dont really need the destructor to do anything in particular, but rule of three
	~Dollar(){};	

};

class Euro : public money
{
	public:
	Euro(double amount):money(1.14, amount){}
	Euro(const Euro& other):money(other.rate(), other.amount){};
	Euro& operator = (const Euro& other)
	{
		if(&other == this)
		{
			return *this;
		}
		dollar_rate = other.rate();
		amount = other.amount;
		return *this;
	}
	~Euro(){}
};

class Yen : public money
{
	public:
	Yen(double amount):money(0.0087, amount){}
	Yen(const Yen& other):money(other.rate(), other.amount){};
	Yen& operator = (const Yen& other)
	{
		if(&other == this)
		{
			return *this;
		}
		dollar_rate = other.rate();
		amount = other.amount;
		return *this;
	}
	~Yen(){}
};

class Pound : public money
{
	public:
	Pound(double amount):money(1.37, amount){}
	Pound(const Pound& other):money(other.rate(), other.amount){};
	Pound& operator = (const Pound& other)
	{
		if(&other == this)
		{
			return *this;
		}
		dollar_rate = other.rate();
		amount = other.amount;
		return *this;
	}
	~Pound(){}
};


//functor convert	// DO NOT RECOMMEND instanciating this template with a non-money type
template<class T>
class Convert
{
	T ref{0};
	public:
	Convert(){};

	Convert(money i_amount)
	{
		ref.amount = (i_amount() * (i_amount.rate() / ref.rate()));
	}
	
	T operator() (money i_amount)
	{
		double o_amount = (i_amount() * (i_amount.rate() / ref.rate()));
		return o_amount;
	}

	operator T() const
	{
		return ref;
	}

	operator double() const
	{
		return ref.amount;
	}
	//copy constructor
	Convert(const Convert& source): ref(source.ref) {}

	//copy assignment operator	
	Convert& operator =(const Convert& source)
	{
		if(&source == this)
		{
			return *this;
		}
		ref = source.ref;
		return *this;
	}	
	//destructor
	~Convert(){}

};


template<class T, class C = int>
class Account
{
private:
	T amt;
public:
	//constructors
	Account(T, double amount) : amt(amount){}
	Account(double amount) : amt(amount){}
	Account(const Account& source):amt(source.amt){}	//copy constructor
	Account(Account&& source)				//move constructor - really doesn't do anything practical
	{
		amt = source.amt;
		source.amt = 0;
	}
	//copy assignment operator
	Account& operator = (const Account& source)
	{
		if(&source == this)
		{
			return *this;
		}
		amt = source.amt;
		return *this;
	}
	//move assignment operator
	Account& operator = (Account&& source)
	{
		if(&source == this)
		{
			return *this;
		}
		amt = source.amt;
		source.amt = 0;
		return *this;
	}
	
	T getbalance() const
	{
		return amt;
	}


	//deposit an amount, explicitly assumed to be in whatever currency is appended in the template argument
	template<class Q>
	T deposit(double amount)
	{
		
		if(std::is_same_v<T, Q>)
		{
			amt += amount;
		}
		else
		{
			amt += Convert<T>(Q(amount));	//note to self: this looks weird because I'm constructing a money object of type Q with amount inside the constructor/functor call of Convert
		}

		return amt;
	}
	template<>
	T deposit<double>(double amount)
	{
		amt += amount;
	}
	template<>
	T deposit<float>(double amount)
	{
		amt += amount;
	}
	template<>
	T deposit<int>(double amount)
	{
		amt += amount;
	}

	//deposit an amount of a currency
	template<class Q>
	T deposit(Q amount)
	{
		
		if(std::is_same_v<T, Q>)
		{
			amt += amount;
		}
		else
		{
			amt += Convert<T>(Q(amount));
		}

		return amt;
	}

	//withdraw an amount, explicitly assumed to be in whatever currency is appended in the template argument
	template<class Q>
	T withdraw(double amount)
	{
		
		if(std::is_same_v<T, Q>)
		{
			amt -= amount;
		}
		else
		{
			amt -= Convert<T>(Q(amount));
		}

		return amt;
	}
	template<>
	T deposit<double>(double amount)
	{
		amt -= amount;
	}
	template<>
	T deposit<float>(double amount)
	{
		amt -= amount;
	}
	template<>
	T deposit<int>(double amount)
	{
		amt -= amount;
	}

	template<class Q>
	T withdraw(Q amount)
	{
		
		if(std::is_same_v<T, Q>)
		{
			amt -= amount;
		}
		else
		{
			amt -= Convert<T>(Q(amount));
		}

		return amt;
	}

	template<class R>
	Account<R>* currency() const
	{
		Account<R>* temp = new Account<R>(0);
		temp->deposit(amt);
		return temp;
	}



// ************* COMPARISON OPERATORS  **************

	//all these comparison operators are a bit sketchy when amounts are very close to equal due to limits of floating point precision
	template<class X>
	bool operator < (const Account<X>& other) const
	{
		X otherval = other.getbalance();
		T compval{0};
		if(!std::is_same_v<T,X>)
		{
			compval = Convert<T>(otherval);
		}
		else
		{
			compval = (double)otherval;
		}
		
		if(amt < compval)
		{
			return true;
		}
		return false;
	}
	
	template<class X>
	bool operator > (const Account<X>& other) const
	{
		if(other < *this)
		{
			return true;
		}
		return false;
	}
	template<class X>
	bool operator == (const Account<X>& other) const
	{
		if ((!(*this < other))&& (!(*this > other)))
		{
			return true;
		}
		return false;
	}
	template<class X>
	bool operator !=(const Account<X>& other) const
	{
		if(*this == other)	// note to self: this IS NOT guarding against self-assignment, even though it looks like I messed up while intending to do that
		{
			return false;
		}

		return true;
	}

//********** ARITHMETIC OPERATORS *********
// most of the arithmetic operators are handled by my definition of conversion to double. Have to do some silly levels of specificity due to some of the ease-of-use operators I declared for these types earlier.
	template<class X>
	X operator + (X value) const
	{
		if(std::is_arithmetic_v<X>)	//this is a great place to practice conditional compile
		{
			return (amt.amount + value);
		}

		return value;
	}
 
	template<class X>
	X operator - (X value) const
	{
		if(std::is_arithmetic_v<X>)	
		{
			return (amt.amount - value);	//if this was more difficult, I'd just multiply value by -1 and call the operator + function
		}

		return value;
	}
 
	template<class X>
	X operator * (X value) const
	{
		if(std::is_arithmetic_v<X>)	
		{
			return (amt.amount * value);	//if Account was more complicated , I'd lean toward calling the operator + function inside a loop
		}

		return value;
	}
	template<class X>
	X operator / (X value) const
	{
		if(std::is_arithmetic_v<X>)	
		{
			return (amt.amount / value);
		}

		return value;
	}
	template<class X>
	X operator % (X value) const
	{
		if(std::is_arithmetic_v<X>)	
		{
			return (amt.amount % value);
		}

		return value;
	}
	
	template<class X>
	void operator += (X value)
	{
		if(std::is_arithmetic_v<X>)	
		{
			amt.amount = amt.amount + value;
		}
	}
	
	template<class X>
	void operator -= (X value)
	{
		if(std::is_arithmetic_v<X>)	
		{
			amt.amount = amt.amount - value;
		}
	}
	
	template<class X>
	void operator *= (X value)
	{
		if(std::is_arithmetic_v<X>)	
		{
			amt.amount = amt.amount * value;
		}
	}
	
	template<class X>
	void operator /= (X value)
	{
		if(std::is_arithmetic_v<X>)	
		{
			amt.amount = amt.amount / value;
		}
	}
	
	//pretty sure a real account should have this operator explicitly deleted, as well as some of the other compound assignments
	template<class X>
	void operator %= (X value)
	{
		if(std::is_arithmetic_v<X>)	
		{
			amt.amount = amt.amount % value;
		}
	}
	
	//wrote this before cast template. Surprisingly, the templated cast seems to actually catch all the places I took advantage of this function earlier. Things working. Weird.
//	operator double() const
//	{
//		return amt;
//	}

	template<class X>
	operator X() const
	{
		if(std::is_same_v<X, Dollar>)
		{
			return Convert<Dollar>(amt);
		}
		else if(std::is_same_v<X, Euro>)
		{
			return Convert<Euro>(amt);
		}
		else if(std::is_same_v<X, Yen>)
		{
			return Convert<Yen>(amt);
		}
		else if(std::is_same_v<X, Pound>)
		{
			return Convert<Pound>(amt);
		}
		//if its not a kind of currency, just return the double in amount and hope the compiler can handle the remaining conversions
		return amt.amount;	//this was supposed to be the unspecialized version of the template, then I discovered I couldn't specialize the conversion operator(see below)
	}
	
	//template<>
	//operator Dollar() const	// different function signiture (operator Dollar vs Operator X), doesn't find base template
	//operator X<Dollar>() const	// invalid, no template named X
	//operator X()<Dollar> const	// invalid, unknown type name X
	//operator <Dollar>() const	//invalid, operator< cannot be the name of a variable or data member
//	{
//	}

};


int main()
{
	Account<Euro> a(500);
	money m(0,5);
	Account<Dollar>* d = a.currency<Dollar>();
	Account<Dollar> b(200);
	Account<Euro> c(500);
	assert(b < a);
	assert(!(a < c));
	assert(!(a > c));
	assert(a == c);
	assert(c == a);
	assert(c != b);
	
	int ifive = 5;
	float ffive = 5;
	double dfive = 5;
	assert(c - ifive == 495);
	assert(c + ffive == 505);//this might not work to to float point things
	assert(c * dfive == 2500);
	assert(c / 2 == 250);
	assert((int)c == 500);
	c += 250;
	assert((int)c == 750);
//	assert(c + "Test" == "Test");	//this fails to compile because it can't cast the output of getbalance to a string. I have no objection to this behavior

	std::cout << (char)c << std::endl;	
//	std::cout << (std::string)c << std::endl;	//I was beginning to think it would let me get away with any cast, but thankfully, there are limits. This one caused a compile error.
	std::cout << (double)*d << std::endl;
	std::cout << Convert<Dollar>(a.getbalance()) << std::endl;
	return 0;
}

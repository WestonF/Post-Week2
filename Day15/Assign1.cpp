#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>

class bad_numbers : public std::exception
{
public:
	const char * what() const throw()
	{
		return "Invalid Number(s)";
	}
};

bool floatcompare(float a, float b)	//noexcept(true)
{
	if (a != floor(a))	//effectively is_exactly_representable_as_int(float a)
	{
		throw bad_numbers();
	}
	if (b != floor(b))
	{
		throw bad_numbers();
	}

	if(a == b)
	{
		return true;
	}

	return false;
}

int main()
{
	float a = 2.0;
	float b = 3.1;
	bool match = false;
	bool except_thrown = false;
	try
	{
		match = floatcompare(a,b);
	}catch(bad_numbers& bad){
		assert(std::string(bad.what()) == "Invalid Number(s)");
		except_thrown = true;
		throw;	//this is one method of making the program intolerant of exceptions. Throw is not caught within the program and calls std::terminate
	}catch(...){
		throw;	// this is the same method above, just catching and rethrowing any other exception types. pretty sure I don't actually need this, because without it the function would be transparent to the thrown error if it was not of type bad_number 
	}

	// as expected, the scope of bad ends at the end of the catch statement
	//assert(std::string(bad.what()) == "Invalid Number(s)");	//this line causes an expected compile error when not commented out
	

	//code from here onward is not executed because I made the above code exception intolerant. commenting out the "throw"s makes it exception tolerant, and then the asserts are asserted properly
	assert(except_thrown == true);
	assert(match == false);
	except_thrown = false;

	a = 3;
	b = 3.0;
	try
	{
		match = floatcompare(a,b);
	}catch(bad_numbers& bad){
		assert(std::string(bad.what()) == "Invalid Number(s)");
		except_thrown = true;
		throw;	//see above throw comments
	}catch(...){
		throw;	//see above throw comments
	}
	
	//repeat comment because it's true here too
	//code from here onward is not executed because I made the above code exception intolerant. commenting out the "throw"s makes it exception tolerant, and then the asserts are asserted properly

	assert(except_thrown == false);
	assert(match == true);
	return 0;
}

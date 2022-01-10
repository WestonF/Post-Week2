#include <iostream>
#include <cassert>

class A {};	//implicit copy assignment works / is not applicable
class B 	//cannot implicitly copy assign with const member
{
	const int number = 5;
};
class C		//cannot copy assign due to deletion
{
	C& operator =(const C& other) = delete;
};
class D		//implicit copy can handle a member 
{
	public:
	int number;
	D(int i_number):number(i_number){}
};
class E		//explicit copy assignment operator
{

	public:
	int number;
	E(int i_number):number(i_number){}
	
	E& operator=(const E& other)
	{
		if(this == &other)
		{
			return *this;
		}

		number = other.number;
		return *this;
	}
};

int main()
{

	assert(std::is_copy_assignable<A>::value);
	assert(!std::is_copy_assignable<B>::value);
	assert(!std::is_copy_assignable<C>::value);
	assert(std::is_copy_assignable<D>::value);
	assert(std::is_copy_assignable<E>::value);
	return 0;
}

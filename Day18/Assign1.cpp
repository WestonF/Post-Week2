#include <iostream>
#include <cassert>

class out_of_range : public std::exception
{
public:
	const char * what () const throw()
	{
		return "Out of bounds";
	}
};


template<typename T>
class myvector
{
private:
	T* mData; // this is the actual data (or array)
	int mSize; // Size is how much data.  Capacity is how much memory.
	int mCapacity; // not always same as mSize, usually much larger than mSize.
	T mUndefined;// Lots of STL functions say that doing something naughty gets "undefined behavior".
public:
	myvector()
       	{
		mCapacity = 0;	//reserve references current value of mCapacity, so it needs to be defined before call
		mSize = 0;
	
		reserve(8);	//arbitrarily selected minimum capacity
	}

	//I don't really like this function, as it doesnt define the contents of the elements
	myvector(T*, int sz) // creates a vector of sz elements
       	{
		mCapacity = 0;
		reserve((4 > sz ? 8 : (2 * sz)));
		for(int i = 0; i < sz; i++)
		{
			push_back(mUndefined);	//accessing these without filling them would be "naughty"
		}
	}
	virtual ~myvector()
	{
		delete[](mData);
	}
	myvector(const myvector<T>& other)
	{
		mCapacity = 0;
		reserve(other.capacity());
		for(int i = 0; i < other.size(); i++)
		{
			push_back(other.at(i));	//push_back handles updating mSize
		}
	}
	myvector(myvector<T>&& other) 
	{
		//steal data (by reference)
		mData = &other;
		//steal meta-data (by value because they're each just single primitives)
		mSize = other.size();
		mCapacity = other.capacity();
		
		//set other to empty/junk, BUT VALID, state
		other.mData = nullptr;
		other.mSize = 0;
		other.mCapacity = 0;

	}
	myvector& operator=(const myvector<T>& other) 
	{
		if(&other == this)
		{
			return;
		}
		
		clear();
		reserve(other.size());
		for(int i = 0; i < other.size(); i++)
		{
			push_back(other.at(i));
		}
		return this;
	}
	
	//add move assignment operator to complete rule of 5
	myvector& operator =(myvector<T>&& other) 
	{
		if(&other == this)
		{
			return;
		}

		//delete and deallocate existing contents of myvector
		delete[](mData);
		//reset counters (not really required)
		clear();

		//steal data (by reference)
		mData = &other;
		//steal meta-data (by value because they're each just single primitives)
		mSize = other.size();
		mCapacity = other.capacity();
		
		//set other to empty/junk, BUT VALID, state
		other.mData = nullptr;
		other.mSize = 0;
		other.mCapacity = 0;

	}
	bool operator==(const myvector<T>& other)
       	{
		//my interpretation is that we want to check if both vectors are pointing to the same location, i.e. they are the same vector. If we wanted to check if they had identical data, then the function would look very different
		if(&other == this)
		{
			return true;
		}

		return false;
	}
	T& operator[](int offset)
	{
		if (offset >= mSize)
		{
			return mUndefined;
		}
		return (this + offset);
	}
	void push_back(const T& t) 
	{
		if (mSize == mCapacity)
		{
			reserve(mCapacity);
		}
		
		mSize++;
		*(this + mSize) = t;
	}
	void pop_back()
       	{
		if(mSize < 0)
		{
			mSize--;	//the data doesnt actually go anywhere, but it may as well be gone. Still properly managed with rest of capacity
		}

	}
	
	T& at(int offset) 
	{
		if (offset >= mSize)
		{
			throw out_of_range();
		}
		
		return (this + offset);
	}

	void clear()
	{
		//arbitrary base value
		mCapacity = 8;

		//decided I don't actually want it to do this
			//delete/deallocate existing data
			//delete[](mData);
			//make new space and associate it with mData
			//mData = new T[mCapacity];

		//reset size to zero
		mSize = 0;		
	}
	
	int size() const
	{
		return mSize;
	}
	
	void reserve(int tCount)	// use an algorithm to increase capacity as required
	{
		mCapacity += tCount;
		//make new space
		T* temp = new T[mCapacity];
		
		//copy old data to new place
		for(int i = 0; i < mSize; i++)
		{
			temp[i] = mData[i];
		}

		//delete old data/deallocate space
		delete[](mData);
		
		//store new location to mData
		mData = temp;
	}

	int capacity() const
       	{
		return mCapacity;
	}
	
	// iterator
	class iterator
	{
		friend myvector;
		myvector* myvec;
		unsigned int current; // offset 
		public:
		iterator(myvector* myvec, int offset): current(offset) {}
		void operator++()
	       	{
			current++;
		}
		void operator--()
	       	{
			current--;
		}
		bool operator==(const iterator& other)
		{
			return ((other.myvec == myvec) && (other.current == current));
		}
				
		bool operator!=(const iterator& other)
		{
			return (!((other.myvec == myvec) && (other.current == current)));
		}
		//T& operator*(); cant overload dereference operator, don't think this was asking for a multiplication operator, given the return type and lack of a parameter
	};
	iterator begin() const {return iterator(this, 0);}; // dummy return no more
	iterator end() const {return iterator(this, mSize - 1);}; // dummy return no more
};

int main()
{
	return 0;
}


	

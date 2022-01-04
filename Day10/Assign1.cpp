#include <iostream>

class Person {
    private:
	//no nameless Persons allowed
	//also not allowed to fail to populate fields if I want to use them
	Person():index(-1),count(0){};	//am I allowed to use initialization lists to populate with literals?
        int index;
	static constexpr int NAMELISTSIZE = 10;
	static constexpr int NAMEINDEXLIMIT = NAMELISTSIZE -1;

	//Person construction only allowed via getPerson()
	Person(std::string i_name, Person* old_person)
	{
		name = i_name;
		index = old_person->index;
		count = old_person->count + 1;
	}
	
	Person(std::string i_name, int i_index)
	{
		name = i_name;
		//construction without an existing pointer should have a prior count of 0
		count = 1;
		index = i_index;
	}

    public:
	std::string name;
        int count;

	//this array holds a pointer to each person
	static Person* persons[NAMELISTSIZE];
	//this whole program would make a lot more sense if this function returned the index int instead of a Person pointer, but I don't think I was supposed to modify its return type
	//this function checks the array to see if the name already exists
        static Person* getPerson(std::string name)
       	{
		//getPerson cannot access Person::persons? but why? Needs to be initialized first. Apparently declaring it is insufficient. I thought declared static variables auto-initialized to effective-zero values?


		//I could propably initialize index to zero and replace i in the loop with index, but this feels more readable	
		//turns out I can't because this is a static member function, so I can't access a non-static member without a specific pointer to it
		for (int i = 0; i < Person::NAMELISTSIZE; i++)
		{
			//if the current index has been populated	//indices will ALWAYS be populated sequentially, because I'm going to define it that way
			if(Person::persons[i] != nullptr)
			{

				if(name == Person::persons[i]->name)
				{
					//make new person
					Person* new_person = new Person(name, Person::persons[i]);
					
					delete(Person::persons[i]);
					Person::persons[i] = new_person;
					
					//return pointer
					return new_person;
				}
				//this implicit else *should* jump back to the start of the loop, but looks funky
			}
			else	// current index NOT populated	//note: because of sequential population of array, getting to a nullptr guarantees the current name is a new one
			{
				//constructor sets index to i and count to 1;	
				Person* new_person = new Person(name, i);
				
				//stuff it in the array
				Person::persons[i] = new_person;

				return new_person;
			}
		}
		
		//program should only reach this spot if all elements of persons is full AND the new name matches none of them
	
		return nullptr;


	
        }

	//no destructor implemented, don't think I really need anything from one, so the auto-gen default will do. might make sense to delete their indexed value in the array, but I think with the way the program currently works,  that would ALWAYS try to delete an already-being-deleted object
};
//required because Person::persons is static member
Person* Person::persons[] = {nullptr};


int main(void) {
	// declare and string array of names here
	const std::string names[] = {"bob", "james", "janet", "bob", "stella"};
	Person* temp;
    	for (int ii = 0; ii < 5; ii++) {
	{
	//	temp = Person::getPerson(names[ii]);
		temp = Person::getPerson(names[ii]);
		std::cout << temp->name << ": " << temp->count << ", ";
	}
	std::cout << std::endl;
        // fetch the name and print the result
    }
}

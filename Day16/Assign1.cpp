#include<iostream>
#include<sstream>
#include<cassert>
#include<vector>
#include<iomanip>


class bad_numbers : public std::exception
{
public:
	const char* what() const throw()
	{
		return "Invalid Number";
	}
};

class bad_input : public std::exception
{
public:
	const char* what() const throw()
	{
		return "Invalid Instruction";
	}
};

enum class ACTION
{
	DEC_OUT,
	HEX_OUT,
	BIN_OUT,
	DEC_IN,
	HEX_IN,
	BIN_IN,
	ADD
};



class calc
{
	int dectodec(std::string input);
	int hextodec(std::string input);
	int bintodec(std::string input);
	std::string dectodec(int input);
	std::string dectobin(int input);
	std::string dectohex(int input);
	
	enum class OUTPUT
	{
		DEC,
		HEX,
		BIN
	};

	enum class OPERATION
	{
		ADD
	};

	OUTPUT output;
	std::vector<int> operands;
	std::vector<OPERATION> operations;

	int actually_do();
	void show_results(int result);
	void what_do(std::vector<std::string> inputs);
	ACTION whats_this(std::string input);
public:
	calc(): output(OUTPUT::DEC){};
	
	void do_calculator_things(std::vector<std::string> inputs);
};
int main()
{
	calc calculator;
	std::string command = "";
	std::vector<std::string> maths;
	while(true)
	{
		maths.clear();
		std::cout << "prompt > ";
		std::getline(std::cin, command);
		if(command == "quit")
		{
			break;
		}

		// smart people use isstream i_s(command), then load from i_s using i_s >> tempstring; maths.push_back(tempstring); and loop while(!i_s.eof())
		//I don't want to ignore all spaces, as they separate parameters
		//ignore leading spaces
		int j = 0;
		while(j < command.length())
		{
			if(command[j] != ' ')
			{
				break;
			}

			j++;
		
		}

		std::string tempstring = "";
		for(int i = j; i < command.length(); i++)
		{
			if(command[i] != ' ')
			{
				tempstring.push_back(command[i]);
			}
			else
			{
				maths.push_back(tempstring);
				tempstring = "";
			}
		}
		maths.push_back(tempstring);
		tempstring = "";

	//DO CALCULATOR THINGS HERE
	calculator.do_calculator_things(maths);

	}

	//std::cout << conv::hextodec("F") << std::endl;
	//std::cout << conv::bintodec("1111") << std::endl;
	//std::cout << conv::hextodec("FF") << std::endl;
	//std::cout << conv::bintodec("11111111") << std::endl;
	//std::cout << conv::hextodec("10") << std::endl;
	//std::cout << conv::bintodec("10000") << std::endl;
	//std::cout << conv::hextodec("1f") << std::endl;
	//std::cout << conv::bintodec("11111") << std::endl;
	
	//std::cout << conv::dectobin(10) << std::endl;
	//std::cout << conv::dectohex(10) << std::endl;
	//std::cout << conv::dectobin(255) << std::endl;
	//std::cout << conv::dectohex(255) << std::endl;
	//std::cout << conv::dectobin(1999) << std::endl;
	//std::cout << conv::dectohex(1999) << std::endl;
	//std::cout << conv::dectobin(12) << std::endl;
	//std::cout << conv::dectohex(12) << std::endl;


	//std::cout << conv::dectodec(4324263) << std::endl;
	//std::cout << conv::dectodec(52) << std::endl;
	//std::cout << conv::dectodec("1000") << std::endl;
	//std::cout << conv::dectodec("63i") << std::endl;
	
	return 0;
}

int calc::dectodec(std::string input)
{
	int accum = 0;
	for(int i = input.length() - 1; i >= 0 ; i--)
	{
		int temp = 0;
		//get the value of the digit
		if((input[i] >= '0')&&(input[i] <= '9'))
		{
			temp = input[i] - '0';
		}
		else
		{
			throw bad_numbers();
			return -1;	//pretty sure this line never executes, but it still seems like it should be here
		}
		
		//account for the decimal place
		for (int j = 0; j < (input.length() - 1) - i; j++)
		{
			temp *= 10;	
		}	

		//add it to the running total
		accum += temp;
	}
	return accum;
}


int calc::hextodec(std::string input)
{
	int accum = 0;
	for(int i = input.length() - 1; i >= 0 ; i--)
	{
		int temp = 0;
		//get the value of the digit
		if((input[i] >= '0')&&(input[i] <= '9'))
		{
			temp = input[i] - '0';
		}
		else if((input[i] >= 'a')&&(input[i] <='f'))
		{
			temp = input[i] - 'a' + 10;
		}
		else if((input[i] >= 'A')&&(input[i] <='F'))
		{
			temp = input[i] - 'A' + 10;
		}
		else
		{
			throw bad_numbers();
			return -1;	//pretty sure this line never executes, but it still seems like it should be here
		}
		
		//account for the decimal place
		for (int j = 0; j < (input.length() - 1) - i; j++)
		{
			temp *= 16;	
		}	

		//add it to the running total
		accum += temp;
	}
	return accum;
}

int calc::bintodec(std::string input)
{
	int accum = 0;
	for(int i = input.length() - 1; i >= 0 ; i--)
	{
		int temp = 0;
		//get the value of the digit
		if((input[i] >= '0')&&(input[i] <= '1'))
		{
			temp = input[i] - '0';
		}
		else
		{
			throw bad_numbers();
			return -1;
		}
		
		//account for the decimal place
		for (int j = 0; j < (input.length() - 1) - i; j++)
		{
			temp *= 2;	
		}	

		//add it to the running total
		accum += temp;
	}
	return accum;
}

std::string calc::dectodec(int input)
{
	std::string backwards, outstring = "";
	int count = 0;
	while (input != 0)
	{
		if(count == 3)
		{	//improve readability
			backwards.push_back(',');
			count = 0;
		}
		backwards.push_back(static_cast<char>((input % 10) + '0'));
		input /= 10;
		count++;
	}
	
	//affix the prefix before attaching numbers
	//outstring += "";	//no prefix for decimal
	
	//no front-side buffer for decimal
	//while(4 - count > 0)
	//{
	//	outstring.push_back('0');	//make nice formatting
	//	count++;
	//}
	
	for(int i = backwards.length() - 1; i >= 0; i--)
	{
		outstring.push_back(backwards[i]);
	}

	return outstring;
}

std::string calc::dectobin(int input)
{
	std::string backwards, outstring = "";
	int count = 0;
	while (input != 0)
	{
		if(count == 4)
		{	//improve readability
			backwards.push_back(' ');
			count = 0;
		}
		backwards.push_back(static_cast<char>((input % 2) + '0'));
		//outstring << static_cast<char>((input % 2) + '0');	//maybe debug this line later
		input /= 2;
		count++;
	}
	
	//affix the prefix before attaching numbers
	outstring += "0b";
	
	while(4 - count > 0)
	{
		outstring.push_back('0');	//make nice formatting
		count++;
	}
	
	for(int i = backwards.length() - 1; i >= 0; i--)
	{
		outstring.push_back(backwards[i]);
	}

	return outstring;
}

//std::string dectobin(std::string input);

std::string calc::dectohex(int input)
{
	std::string backwards, outstring = "";
	int count = 0;
	while (input != 0)
	{
		if(count == 4)
		{	//improve readability
			backwards.push_back(' ');
			count = 0;
		}
		int temp = input % 16;
		if (temp < 10)
		{
			backwards.push_back(static_cast<char>(temp + '0'));
		}
		else if(temp < 16)
		{
			//I only approve of capitals for hexidecimal, even if I will accept lowercase
			backwards.push_back(static_cast<char>(temp - 10 + 'A'));

		}
		//outstring << static_cast<char>((input % 2) + '0');	//maybe debug this line later
		input /= 16;
		count++;
	}

	//affix the prefix before attaching numbers
	outstring += "0x";
	while(4 - count > 0)
	{
		outstring.push_back('0');	//make nice formatting
		count++;
	}

	//put numbers in right order
	for(int i = backwards.length() - 1; i >= 0; i--)
	{
		outstring.push_back(backwards[i]);
	}

	return outstring;
}


void calc::what_do(std::vector<std::string> inputs)
{
	operands.clear();
	operations.clear();

	if(inputs.size() == 0)
	{
		throw bad_input();
		return;
	}

	std::vector<ACTION> actions;
	for(int i = 0; i < inputs.size(); i++)
	{
		actions.push_back(whats_this(inputs[i]));
	}
	
	// determine output type
	switch(actions[0])
	{
	default:
		output = OUTPUT::DEC;
		break;
	}
	
	output = OUTPUT::DEC;
	//find and gather operands
	for(int i = 0; i < actions.size(); i++)
	{
		switch(actions[i])
		{
		case ACTION::DEC_IN:
			operands.push_back(dectodec(inputs[i]));
			break;
		case ACTION::HEX_IN:
			operands.push_back(hextodec(inputs[i].substr(2)));
			break;
		case ACTION::BIN_IN:
			operands.push_back(bintodec(inputs[i].substr(2)));
			break;
		case ACTION::ADD:
			// if this is not the first element
			if(i != 0)
			{
				// AND this is not the last element
				if((i + 1) < actions.size())
				{	
					// AND if the element before is a number
					if((actions[i - 1] == ACTION::DEC_IN)||(actions[i - 1] == ACTION::HEX_IN)||(actions[i - 1] == ACTION::BIN_IN))
					{
						//AND the element after is a number
						if((actions[i + 1] == ACTION::DEC_IN)||(actions[i + 1] == ACTION::HEX_IN)||(actions[i + 1] == ACTION::BIN_IN))
						{	
							//given all of those requirements are true, actually add the operation.
							operations.push_back(OPERATION::ADD);
							break;
						}
					}
				}
			}
			// if even one of those is not true, the ADD operation is invalid, which invalidates the whole command
			throw bad_input();
			break;
		//defining output type only allowed as first argument, anywhere else invalidates the whole command
		case ACTION::DEC_OUT:
			if(i == 0)
			{	
				output = OUTPUT::DEC;
			}
			else
			{
				throw bad_input();
			}
			break;
		case ACTION::HEX_OUT:
			if(i == 0)
			{	
				output = OUTPUT::HEX;
			}
			else
			{
				throw bad_input();
			}
			break;
		case ACTION::BIN_OUT:
			if(i == 0)
			{	
				output = OUTPUT::BIN;
			}
			else
			{
				throw bad_input();
			}
			break;
		default:
			break;
		}
	}
		

}

ACTION calc::whats_this(std::string input)
{
	int size = input.length();
	switch(size)
	{
	case 0:
		throw bad_input();
	case 1:
		if(input == "+")
		{
			return ACTION::ADD;
		}
		else if((input[0] >= '0') || (input[0] <= '9'))
		{
			return ACTION::DEC_IN;
		}

		throw bad_input();
	//case 2: intentionally skipped
	case 3:
		if((input == "dec")||(input == "Dec")||(input == "DEC"))
		{
			return ACTION::DEC_OUT;
		}
		else if((input == "hex")||(input == "Hex")||(input == "HEX"))
		{
			return ACTION::HEX_OUT;
		}
		else if((input == "bin")||(input == "Bin")||(input == "BIN"))
		{
			return ACTION::BIN_OUT;
		}
	default:
	break;
	}
//input now MUST be either 2 chars or at least 4;
	if(input[0] == '0')
	{
		if(input[1] == 'x')
		{
			if(size == 2)
			{
				throw bad_input();
			}
			return ACTION::HEX_IN;
		}
		else if(input[1] == 'b')
		{
			if(size == 2)
			{
				throw bad_input();
			}
			return ACTION::BIN_IN;
		}

	}
	
	return ACTION::DEC_IN;
}

int calc::actually_do()
{
	int opr1;
	int opr2;
	if (operands.size() == 0)
	{
		throw bad_input();
	}
	else if (operands.size() != operations.size() + 1)
	{
		throw bad_input();
	}

	//what_do has already converted all operands to decimal

	opr1 = operands[0];
	if(operands.size() == 0)
	{
		return opr1;
	}

	for(int i = 1; i < operands.size(); i++)
	{
		opr2 = operands[i];
		switch(operations[i-1])
		{
		case OPERATION::ADD:
			opr1 += opr2;
			break;
		//additional operations would go here if more than just ADD was defined
		default:
			throw bad_input();
			break;
		}
	}

	return opr1;
}

void calc::show_results(int result)
{
	std::cout << std::setw(20) << std::setfill(' ');
	std::string outstring;
	switch(output)
	{
	case OUTPUT::DEC:
		outstring = dectodec(result);
		break;
	case OUTPUT::HEX:
		outstring = dectohex(result);
		break;
	case OUTPUT::BIN:
		outstring = dectobin(result);
		break;
	default:
		throw bad_input();
		break;
	}

	std::cout << outstring << std::endl;

}

void calc::do_calculator_things(std::vector<std::string> inputs)
{
	try{
		what_do(inputs);
		show_results(actually_do());
	}
	catch(bad_input& bad)
	{
		std::cout << std::string(bad.what()) << std::endl;
	}
	catch(bad_numbers& bad)
	{
		std::cout << std::string(bad.what()) << std::endl;
	}
	catch(...)
	{
		throw;
	}
}
//std::string dectohex(std::string input);

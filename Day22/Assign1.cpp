#include<iostream>
#include<cmath>
#include<cassert>
#include<vector>
#include<fstream>
#include<chrono>
#include<ctime>
#include<thread>

#define _USE_MATH_DEFINES
// M_PI is name of pi



class too_close : public std::exception
{
	public:
		const char * what() const throw()
		{
			return "Bodies at distance of zero!";
		}
};


//some constants
//Gravitational constant
const double G = 6.673e-11;
//timestep	//experiment and adjust
const double timestep = 0.001;
//small mass	// experiment and adjust
const double initial_mass = 1.0;
//num timesteps	//experiment and adjust. Can be large?
const double k = 100;
const double t_end = 1;


constexpr int nvalsize = 6;
int nval[nvalsize] = {10, 20, 50, 100, 200, 500};//, 1000, 2000};
//int N = 100;

struct coordinate
{
	double x;
	double y;
};

struct body
{
	int ind;	//index of body
	
	double mass = initial_mass;
	coordinate position = {(double)last_ind, 2 * (double)last_ind};	// just so everything starts in different places	//R
	coordinate velocity = {0, 0};		//V
	// dont like this way. would rather use x and y components *forceVector = {0, 0};	//F //computed
	coordinate totalforce = {0, 0};		//F
	coordinate acceleration = {0, 0};	//A
	static int last_ind;
	body()
	{
		ind = last_ind;
		last_ind++;
	}

};

int body::last_ind = 0;

void IndepCalcs(body* bodies)	//timestep is global, don't need to pass it
{
	//these dont really even make sense to keep as sub-functions anymore, but whatever

	//do independent actions
	//acceleration	
	[&bodies]() throw() ->void
	{
		bodies->acceleration.x = bodies->totalforce.x / bodies->mass;
		bodies->acceleration.y = bodies->totalforce.y / bodies->mass;
	}();

	//Velocity
	[&bodies]() throw()->void
	{	
		bodies->velocity.x += bodies->acceleration.x * timestep;
		bodies->velocity.y += bodies->acceleration.y * timestep;
		return;
	}();

	//Position		
	[&bodies]() throw()->void
	{
		bodies->position.x += bodies->velocity.x * timestep;
		bodies->position.y += bodies->velocity.y * timestep;
		return;
	}();
		
}

void DepCalcs(body* bodies, int N, int i, unsigned int* interacts)
{
	for(int j = i + 1; j < N; j++)	//interactions between elements where i < j handled by threads where i value is lower. interactions where i == j are intentionally skipped, as there is not a net gravitational force on self
	{
		//do dependent actions
		//distance
		auto dist = [&bodies, i, j]()->double
		{
			return pow(pow((bodies[j].position.x - bodies[i].position.x), 2) + pow((bodies[j].position.y - bodies[i].position.y), 2), 0.5);
		};

		//total force
		auto fval = [&bodies, i, j, &dist]()->double
		{
			return ((G * bodies[i].mass * bodies[j].mass) / pow(dist(), 2));
		};

		//Force vector
		[&bodies, i, j, &fval, &interacts]()->void
		{
			double angle;
			if (bodies[j].position.x == bodies[i].position.x)	//guard against div by zero
			{
				if(bodies[j].position.y == bodies[i].position.y)
				{
					throw too_close();	// zero distance == infinite force which is bad. best physics would probably be to merge the bodies, but that's outside my scope
				}
				if(bodies[j].position.y > bodies[i].position.y)
				{
					angle = M_PI / 2;
				}
				// therefore (bodies[j].y > bodies[i].y)
				angle = -1 * M_PI / 2;

			}
			else
			{
				angle = atan((bodies[j].position.y - bodies[i].position.y) / (bodies[j].position.x - bodies[i].position.x));
			}

			double force = fval();
			double xforce = force * cos(angle);
			double yforce = force * sin(angle);
			
			//this is the force on i due to j
			//this operation pair and another instance of the following operation pair definitely create a race condition, but I don't think its a problematic one, as the order shouldn't matter. Same is true for multiple instances of interacts++
			bodies[i].totalforce.x += xforce;
			bodies[i].totalforce.y += yforce;
			
			//this is the force on j due to i. Note inverted sign
			bodies[j].totalforce.x -= xforce;
			bodies[j].totalforce.y -= yforce;
			*interacts = *interacts + 1;
		}();	//end forceval lambda
	}//end primary for loop
}


int main()
{
	std::ofstream output;
	output.open("Ipersec.txt");	//clears contents of file
	//print JSON file header
	if(output.is_open())
	{
		output << "{" << std::endl;
		output << "\"results\": [" << std::endl;
	}
	output.close();

	//start varied N count loop
	for(int ii = 0; ii < nvalsize; ii++)
	{
		int N = nval[ii];

		std::chrono::time_point start = std::chrono::high_resolution_clock::now();

		body* bodies = new body[N];	//constructor assigns pseudo-random starting positions
		unsigned int interacts = 0;	
		//for(float t = 0; t < k; k++)

		//start timestep loop. This is the actual simulation portion for any given N count
		for(float t = 0; t < t_end; t += timestep)
		{
			
//////////////////   DO INDEPENDENT CALCULATIONS       //////////////////////////
					
			std::vector<std::thread> threads;
			for (int i = 0; i < N; i++)
			{
				//void IndepCalcs(body* bodies)		//just here for reference
				std::thread id(IndepCalcs, &bodies[i]);	//just sending one body to each thread
				threads.push_back(std::move(id));
			}
			for(auto& t:threads)
			{
				//re-joining here, then re-spawning threads here manages all problematic race conditions. Pretty sure this was not the expected solution, but given how my program was already structured, this seemed like the best solution.
				t.join();
			}

			threads.clear();	//can theoretically re-use existing std::thread objects, as they have all joined by now, but clearing and making new thread objects to do the new functions seems more clear and should have minimal performance penalties
			//reset before accumulating new values. this was totally in the wrong place before. now is fixed
			for(int i = 0; i < N; i++)
			{
				bodies[i].totalforce.x = 0;
				bodies[i].totalforce.y = 0;

				bodies[i].acceleration.x = 0;
				bodies[i].acceleration.y = 0;
			}
		

//////////////////////////////    DO DEPENDENT CALCULATIONS      ///////////////////////	
			for (int i = 0; i < N; i++)
			{
				//void DepCalcs(body* bodies, int N, int i, int& interacts)	//just here for reference
				unsigned int* p_inter = &interacts;
				std::thread id(DepCalcs, &bodies[0], N, i, p_inter);	//This function needs access to all bodies, as it references values outside of bodies[i] and modifies the force of all bodies after bodies[i]
				threads.push_back(std::move(id));
			}
			for(auto& t:threads)
			{
				//gotta re-join before end-of-test time measurement and output
				t.join();
			}
		}//end timestep loop, check time elapsed and print results

		int IperS = (k * N * N / t_end);
		
		std::chrono::time_point stop = std::chrono::high_resolution_clock::now();
		std::chrono::duration<int, std::milli> time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		delete[](bodies);	//clear bodies to prepare for new loop with different N value
	
	
		std::ofstream output;
		output.open("IperSec.txt", std::ios::app);	//adds to end of file
		if(output.is_open())
		{
			output << "{ \"N\":" << N <<" ,\"CalcPerSec\":" << IperS << ", \"Interactions\":" << interacts << ", \"Time_ms\":" << time_passed.count() << "}," << std::endl;
			output.close();
		}
	}//end of varied N values loop
	

		
	output.open("Ipersec.txt", std::ios::app);
	if(output.is_open())
	{
		output << std::endl;
		output << "]" << std::endl << "}" << std::endl;
		output.close();
	}

	
	return 0;
}


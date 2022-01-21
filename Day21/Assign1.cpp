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
	coordinate totalforce = {0, 0};	//F
	coordinate acceleration = {0, 0};	//A
	static int last_ind;
	body()
	{
		ind = last_ind;
		last_ind++;
	}

};

int body::last_ind = 0;

void thread_task(int thread_count)
{
	for(int ii = 0; ii < nvalsize; ii++)
	{
		int N = nval[ii];

		std::chrono::time_point start = std::chrono::high_resolution_clock::now();

		body* bodies = new body[N];
		//need the bodies to have different positions
		/*bodies[0].position.x = 4;
		bodies[0].position.y = 3;

		bodies[1].position.x = -5;
		bodies[1].position.y = 2;
		
		bodies[2].position.x = 1;
		bodies[2].position.y = -8;
		
		bodies[3].position.x = 0;
		bodies[3].position.y = 0;
		
		bodies[4].position.x = -3;
		bodies[4].position.y = -3;
		*/
		unsigned int interacts = 0;	
		//for(float t = 0; t < k; k++)
		for(float t = 0; t < t_end; t += timestep)
		{
			//reset before accumulating new values
			for(int i = 0; i < N; i++)
			{
				bodies[i].totalforce.x = 0;
				bodies[i].totalforce.y = 0;

				bodies[i].acceleration.x = 0;
				bodies[i].acceleration.y = 0;
			}
			
			
			for(int i = 0; i < N; i++)
			{
				//do independent actions
				//acceleration	
				[&bodies, i]() mutable throw() ->void
				{
					bodies[i].acceleration.x = bodies[i].totalforce.x / bodies[i].mass;
					bodies[i].acceleration.y = bodies[i].totalforce.y / bodies[i].mass;
				}();

				//Velocity
				[&bodies, i]() mutable throw()->void
				{	
					bodies[i].velocity.x += bodies[i].acceleration.x * timestep;
					bodies[i].velocity.y += bodies[i].acceleration.y * timestep;
					return;
				}();

				//Position		
				[&bodies, i]() mutable throw()->void
				{
					bodies[i].position.x += bodies[i].velocity.x * timestep;
					bodies[i].position.y += bodies[i].velocity.y * timestep;
					return;
				}();
			}		
				
			for(int i = 0; i < N; i++)
			{
				
				
				for(int j = i; j < N; j++)
				{
					if( i == j)
					{
						//force exerted on self is zero, proceed to next body
						continue;
					}
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
						bodies[i].totalforce.x += xforce;
						bodies[i].totalforce.y += yforce;
						//this is the force on j due to i
						bodies[j].totalforce.x -= xforce;
						bodies[j].totalforce.y -= yforce;
						interacts++;
					}();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	//	int num_bodies[8] = {10, 20, 50, 100, 200, 500, 1000, 2000};
	//	auto IperS = [=, &num_bodies](int iter) mutable throw()->int
	//	{
			//IperS = k * N * N / D
	//		return (k * num_bodies[iter] * num_bodies[iter] / t_end);
	//	};

		int IperS = (k * N * N / t_end);
		
		std::chrono::time_point stop = std::chrono::high_resolution_clock::now();
		std::chrono::duration<int, std::milli> time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		delete[](bodies);
	
	
		std::ofstream output;
		output.open("IperSec.txt", std::ios::app);	//adds to end of file
		if(output.is_open())
		{
			output << "{ \"N\":" << N <<" ,\"CalcPerSec\":" << IperS << ", \"Interactions\":" << interacts << ", \"Time_ms\":" << time_passed.count() << ", \"ThreadCount\":" << thread_count << "}," << std::endl;
			output.close();
		}
	}
}

int main()
{
	std::ofstream output;
	int threadlimit = 12;
	output.open("Ipersec.txt");	//clears contents of file
	if(output.is_open())
	{
		//output << "N, " << "Calc interac/sec,  " << "Interactions, " << "Time Taken(ms), " << "Thread Count" << std::endl;
		output << "{" << std::endl;
		output << "\"results\": [" << std::endl;
	}
	output.close();

	for (int threadcount = 1; threadcount < threadlimit; threadcount++)
	{
		std::vector<std::thread> threads;
		for (int i = 0; i < threadcount; i++)
		{
			std::thread id(thread_task, threadcount);
			threads.push_back(std::move(id));
		}
		for(auto& t:threads)
		{
			
			t.join();
		}

		output.open("Ipersec.txt", std::ios::app);
			if(output.is_open())
			{
				if(threadcount == threadlimit - 1)
				{
					output << std::endl;
					output << "]" << std::endl << "}" << std::endl;
				}
				output.close();
			}

	}

	return 0;
}

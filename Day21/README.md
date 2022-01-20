The results seen here are rather odd. The computer in use has 8 cores, and based on the output of the nproc command from inside the docker container, it seems to have access to all of them. Given this I expected to see a minor increase in the time to execute each thread up until 8, then a sharp increase for 9, as it seemed like one core would then have to complete two threads sequentially, rather than the one thread per core of all previous tests.
Instead, I saw a minor increase in time required per thread for all values, with no abrupt shifts. The increase per thread did grow for each increment, but in a fairly steady manner. This leads me to suspect that the computer was virtualizing cores such that it was creating its own threads to share the work of the threads within the c++ program.
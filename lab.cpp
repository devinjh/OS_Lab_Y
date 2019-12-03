

#include <iostream>
#include <pthread.h>
#include <stdio.h>

// Global variables
//
// Semaphores
int agentSem = 1;
int tobacco = 0;
int paper = 0;
int match = 0;
int mutex = 1;
// Material available
bool isTobacco = false;
bool isPaper = false;
bool isMatch = false;

// Function headers
void V(int* x);
void P(int* x);
void* agent(void* arg);
void* pusher(void* arg);
void* smoker(void* arg);

// Main
int main(int argc, char const *argv[])
{
	// Saying we started
	std::cout << "Started." << std::endl;

	// Creating the p_threads
	int numOfPeople = 3;
	pthread_t agents[numOfPeople];
	pthread_t pushers[numOfPeople];
	pthread_t smokers[numOfPeople * 2];

	// Starting threads of the agents
	for (int i = 0; i < numOfPeople; i++)
	{
		// Causes an error
		pthread_create(&agents[i], NULL, agent, NULL);
	}

	// Starting the threads of the pushers
	for (int i = 0; i < numOfPeople; i++)
	{
		pthread_create(&pushers[i], NULL, pusher, NULL);
	}

	// Starting the threads of the smokers
	for (int i = 0; i < numOfPeople * 2; i++)
	{
		pthread_create(&smokers[i], NULL, smoker, NULL);
	}

	// Stall until all of the smokers terminate
	for (int i = 0; i < numOfPeople * 2; i++)
	{
		pthread_join(smokers[i], NULL);
	}

	// Stall until all of the pushers terminate
	for (int i = 0; i < numOfPeople; i++)
	{
		// Not necessary?? I think
		//pthread_join(pushers[i], NULL);
	}

	// Stall until all of the agents terminate
	for (int i = 0; i < numOfPeople; i++)
	{
		// Not necessary?? I think
		// pthread_join(agents[i], NULL);
	}

	// Saying we finished
	std::cout << "Finished!" << std::endl;
	
	// Ending successfully
	return 0;
}

// Agent
void* agent(void* arg)
{
	//
}

// Pusher
void* pusher(void* arg)
{
	//
}

// Smoker
void* smoker(void* arg)
{
	//
}

// V
void V(int* x)
{
	// Increment the semaphore
	(*x) += 1;
}

// P
void P(int* x)
{
	// Wait until the resource gets released
	while ((*x) == 0) {};

	// Set it back to 0 to make sure no other threads take the resource
	(*x) -= 1;
}








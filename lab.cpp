#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// Global variables
//
// Semaphores
int agentSem = 1;
int paper = 0;
int match = 0;
int mutex = 1;
// Material available
bool isTobacco = false;
bool isPaper = false;
bool isMatch = false;

// Function headers
void V(int *x);
void P(int *x);
void *agent(void *arg);
void *pusher(void *arg);
void *smoker(void *arg);

// Struct table
struct table {
  int tobacco = 0;
  int paper = 0;
  int matches = 0;
};

// All of our pthread mutexes
table shared_table;
pthread_mutex_t agentTableMutex;
pthread_mutex_t pusherAtTableMutex;
pthread_mutex_t tobaccoGuy;
pthread_mutex_t paperGuy;
pthread_mutex_t matchesGuy;

// Testing
pthread_mutex_t agentSetUpMutex;

// Main
int main(int argc, char const *argv[])
{
  // Saying we started
  std::cout << "Started." << std::endl;
  const int NUM_AGENTS = 3;
  const int NUM_SMOKERS = 6;

  std::cout << "Here 1" << std::endl;
  pthread_mutex_init(&agentTableMutex, 0);
  pthread_mutex_init(&pusherAtTableMutex,0);
  pthread_mutex_init(&tobaccoGuy,0);
  pthread_mutex_init(&paperGuy,0);
  pthread_mutex_init(&matchesGuy,0);
  pthread_mutex_lock(&tobaccoGuy);
  pthread_mutex_lock(&paperGuy);
  pthread_mutex_lock(&matchesGuy);

  // Testing
  pthread_mutex_init(&agentSetUpMutex,0);

  std::cout << "Here 2" << std::endl;

  // Creating the p_threads
  pthread_t agents[NUM_AGENTS];
  pthread_t pushers[NUM_AGENTS];
  pthread_t smokers[NUM_SMOKERS];

  std::cout << "Here 3" << std::endl;

  // Starting threads of the agents
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_create(&agents[i], NULL, agent, (void *)i);
  }

  std::cout << "Here 4" << std::endl;

  // Starting the threads of the pushers
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_create(&pushers[i], NULL, pusher, NULL);
  }

  std::cout << "Here 5" << std::endl;

  // Starting the threads of the smokers
  for (int i = 0; i < NUM_SMOKERS; i++) {
    pthread_create(&smokers[i], NULL, smoker, (void *)(i%3));
  }

  std::cout << "Here 6" << std::endl;

  // Stall until all of the smokers terminate
  for (int i = 0; i < NUM_SMOKERS; i++) {
    pthread_join(smokers[i], NULL);
  }

  // Stall until all of the pushers terminate
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_join(pushers[i], NULL);
  }

  // Stall until all of the agents terminate
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_join(agents[i], NULL);
  }

  // Saying we finished
  std::cout << "Finished!" << std::endl;

  // Ending successfully
  return 0;
}

// Agent
void *agent(void *arg)
{
  std::cout << "Here 3-a" << std::endl;
  pthread_mutex_lock(&agentSetUpMutex);
  std::cout << "Here 3a" << std::endl;
  //
  int agent_num = (*(int*)arg);
  //int agent_num = 0;
  pthread_mutex_unlock(&agentSetUpMutex);
  std::cout << "Here 3aa" << std::endl;
  int tobacco = 6;
  std::cout << "Here 3aaa" << std::endl;
  int matches = 6;
  std::cout << "Here 3aaaa" << std::endl;
  int paper = 6;

  std::cout << "Here 3b" << std::endl;

  if(agent_num == 0) {
    paper = 0;
  }
  else if(agent_num == 1) {
    matches = 0;
  }
  else {
    tobacco = 0;
  }

  std::cout << "Here 3c" << std::endl;

  while((tobacco + matches + paper) > 0) {
    //wait for table to be open
    std::cout << "Here 3d" << std::endl;
    pthread_mutex_lock(&agentTableMutex);
    std::cout << "Here 3e" << std::endl;

	//put stuff on table
    if(agent_num == 0) {
      tobacco--;
      matches--;
      shared_table.tobacco++;
      shared_table.matches++;
    }
    else if(agent_num == 1) {
      paper--;
      tobacco--;
      shared_table.paper++;
      shared_table.tobacco++;
    }
    else {
      paper--;
      matches--;
      shared_table.paper++;
      shared_table.matches++;
    }
    std::cout << "Here 3f" << std::endl;
  }

  //pthread_exit(3);
}

// Pusher
void *pusher(void *arg)
{
	//only 1 pusher at table
	pthread_mutex_lock(&pusherAtTableMutex);
	while(true)
	{
		if(shared_table.paper == 1 && shared_table.tobacco == 1)
		{
			//signal matches guy
			shared_table.paper--;
			shared_table.tobacco--;
			pthread_mutex_unlock(&matchesGuy);
			break;
		}else if(shared_table.paper == 1 && shared_table.matches == 1)
		{
			//signal the tobacco guy
			shared_table.paper--;
			shared_table.matches--;
			pthread_mutex_unlock(&tobaccoGuy);
			break;
		}else if(shared_table.matches == 1 && shared_table.tobacco == 1)
		{
			//signal the paper guy
			shared_table.matches--;
			shared_table.tobacco--;
			pthread_mutex_unlock(&paperGuy);
			break;
		}
	}
	pthread_mutex_unlock(&pusherAtTableMutex);

    //table has stuff on it
	pthread_mutex_unlock(&agentTableMutex);
}

// Smoker
void *smoker(void *arg)
{
	int cigsSmoked = 0;
	int type = (*(int*)arg);

	while(cigsSmoked != 3)
	{
		if(type == 0)
		{
			pthread_mutex_lock(&tobaccoGuy);
		}else if(type == 1)
		{
			pthread_mutex_lock(&paperGuy);
		}else if (type == 2)
		{
			pthread_mutex_lock(&matchesGuy);
		}
		cigsSmoked++;
		sleep(200);
	}

	//pthread_exit(3);
}

// V
void V(int *x)
{
  // Increment the semaphore
  (*x) += 1;
}

// P
void P(int *x)
{
  // Wait until the resource gets released
  while ((*x) == 0) {};

  // Set it back to 0 to make sure no other threads take the resource
  (*x) -= 1;
}








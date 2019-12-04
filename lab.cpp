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

// Main
int main(int argc, char const *argv[])
{
  // Saying we started
  std::cout << "Started." << std::endl;
  const int NUM_AGENTS = 3;
  const int NUM_SMOKERS = 6;
  
  pthread_mutex_init(&agentTableMutex, 0);
  pthread_mutex_init(&pusherAtTableMutex,0);
  pthread_mutex_init(&tobaccoGuy,0);
  pthread_mutex_init(&paperGuy,0);
  pthread_mutex_init(&matchesGuy,0);
  pthread_mutex_lock(&tobaccoGuy);
  pthread_mutex_lock(&paperGuy);
  pthread_mutex_lock(&matchesGuy);

  // Creating the p_threads
  pthread_t agents[NUM_AGENTS];
  pthread_t pushers[NUM_AGENTS];
  pthread_t smokers[NUM_SMOKERS];

  // Starting threads of the agents
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_create(&agents[i], NULL, agent, &i);
  }

  // Starting the threads of the pushers
  for (int i1 = 0; i1 < NUM_AGENTS; i1++) {
    pthread_create(&pushers[i1], NULL, pusher, NULL);
  }

  // Starting the threads of the smokers
  for (int i2 = 0; i2 < NUM_SMOKERS; i2++) {
    pthread_create(&smokers[i2], NULL, smoker, &i2);
  }

  // Stall until all of the smokers terminate
  for (int i3 = 0; i3 < NUM_SMOKERS; i3++) {
    pthread_join(smokers[i3], NULL);
  }

  // Stall until all of the pushers terminate
  for (int i4 = 0; i4 < NUM_AGENTS; i4++) {
    pthread_join(pushers[i4], NULL);
  }

  // Stall until all of the agents terminate
  for (int i5 = 0; i5 < NUM_AGENTS; i5++) {
    pthread_join(agents[i5], NULL);
  }

  // Saying we finished
  std::cout << "Finished!" << std::endl;

  // Ending successfully
  return 0;
}

// Agent
void *agent(void *arg)
{
  //
  int agent_num = (*(int*)arg);
  std::cout << "agent_num: " << agent_num << std::endl;
  //
  int tobacco = 6;
  int matches = 6;
  int paper = 6;

  if(agent_num == 0) {
    paper = 0;
  }
  else if(agent_num == 1) {
    matches = 0;
  }
  else {
    tobacco = 0;
  }

  while((tobacco + matches + paper) > 0) {
    //wait for table to be open
    pthread_mutex_lock(&agentTableMutex);

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
  }

  //pthread_exit(3);
}

// Pusher
void *pusher(void *arg)
{	
  //bool loopRunner = true;
  for (int x = 0; x < 6; ++x)
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
        std::cout << "Created a cig for matches guy." << std::endl;
	  		pthread_mutex_unlock(&matchesGuy);
	  		break;
        //loopRunner = false;
	  	}else if(shared_table.paper == 1 && shared_table.matches == 1)
	  	{
	  		//signal the tobacco guy
	  		shared_table.paper--;
	  		shared_table.matches--;
        std::cout << "Created a cig for tobacco guy." << std::endl;
	  		pthread_mutex_unlock(&tobaccoGuy);
	  		break;
        //loopRunner = false;
	  	}else if(shared_table.matches == 1 && shared_table.tobacco == 1)
	  	{
	  		//signal the paper guy
	  		shared_table.matches--;
	  		shared_table.tobacco--;
        std::cout << "Created a cig for paper guy." << std::endl;
	  		pthread_mutex_unlock(&paperGuy);
	  		break;
        //loopRunner = false;
	  	}
	  }
	  pthread_mutex_unlock(&pusherAtTableMutex);

    //table has stuff on it
	  pthread_mutex_unlock(&agentTableMutex);  
  }
}

// Smoker
void *smoker(void *arg)
{
	int cigsSmoked = 0;
	int type = (*(int*)arg) % 3;
  std::cout << "smoker type: " << type << std::endl;

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







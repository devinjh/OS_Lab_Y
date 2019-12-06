#include <iostream>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

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

//semaphores for holding cigarettes
int matchesCigs = 0;
int paperCigs = 0;
int tobaccoCigs = 0;

// semaphore for deciding which kind of smoker/agent
int smokerType = 0;
int agentType = 0;

// All of our pthread mutexes
table shared_table;

//pthread_mutex_t agentTableMutex;
pthread_mutex_t pusherAtTableMutex;
pthread_mutex_t tobaccoGuy;
pthread_mutex_t paperGuy;
pthread_mutex_t matchesGuy;

// locks for the Type assignment semaphores
pthread_mutex_t smokerDecider;
pthread_mutex_t agentDecider;

// Testing
pthread_mutex_t personAtTable;

// Main
int main(int argc, char const *argv[])
{
  // Saying we started
  std::cout << "Started." << std::endl;
  const int NUM_AGENTS = 3;
  const int NUM_SMOKERS = NUM_AGENTS * 2;

  // Intialize the mutexes
  //pthread_mutex_init(&agentTableMutex, 0);
  //pthread_mutex_init(&pusherAtTableMutex, 0);
  pthread_mutex_init(&tobaccoGuy, 0);
  pthread_mutex_init(&paperGuy, 0);
  pthread_mutex_init(&matchesGuy, 0);
  pthread_mutex_init(&smokerDecider,0);
  pthread_mutex_init(&agentDecider,0);

  // Lock the cig mutexes
  /*pthread_mutex_lock(&tobaccoGuy);
  pthread_mutex_lock(&paperGuy);
  pthread_mutex_lock(&matchesGuy);*/
  //pthread_mutex_lock(&pusherAtTableMutex);

  // Testing
  pthread_mutex_init(&personAtTable, 0);

  // Creating the p_threads
  pthread_t agents[NUM_AGENTS];
  pthread_t pushers[NUM_AGENTS];
  pthread_t smokers[NUM_SMOKERS];

  // Starting threads of the agents
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_create(&agents[i], NULL, agent, NULL);
  }

  // Starting the threads of the pushers
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_create(&pushers[i], NULL, pusher, NULL);
  }

  // Starting the threads of the smokers
  for (int i = 0; i < NUM_SMOKERS; i++) {
    pthread_create(&smokers[i], NULL, smoker, NULL);
  }

  std::cout << "Here 1" << std::endl;

  // Stall until all of the smokers terminate
  for (int i = 0; i < NUM_SMOKERS; i++) {
    pthread_join(smokers[i], NULL);
  }

  std::cout << "Here 2" << std::endl;

  // Stall until all of the pushers terminate
  for (int i = 0; i < NUM_AGENTS; i++) {
    pthread_join(pushers[i], NULL);
  }

  std::cout << "Here 3" << std::endl;

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
  int agent_num;
  // assign a unique agent type
  pthread_mutex_lock(&agentDecider);
  agent_num = agentType;
  agentType++;
  pthread_mutex_unlock(&agentDecider);

  //std::cout << "agent num: " << agent_num << std::endl;
  int tobacco = 6;
  int matches = 6;
  int paper = 6;

  //Each agent only has two of the materials
  if(agent_num == 0) {
    paper = 0;
  }
  else if(agent_num == 1) {
    matches = 0;
  }
  else {
    tobacco = 0;
  }

  // while the agent still has stuff to give away
  while((tobacco + matches + paper) > 0) {
    //Agent waits for the table to open up
    pthread_mutex_lock(&personAtTable);
    //agent checks if the table is empty
    if(shared_table.tobacco + shared_table.matches + shared_table.paper == 0) {
      // if the table is empty, the agent puts his respective stuff on it
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
    //agent leaves the table
    pthread_mutex_unlock(&personAtTable);
  }

  // agent is out of inregients
  std::cout << "agent done" << std::endl;

  pthread_exit(0);
}

// Pusher
void *pusher(void *arg)
{
  for (int x = 0; x < 6;) {
    // wait for the table to be open
    pthread_mutex_lock(&personAtTable);

    // check for pairs of items at the table and increase their respective cigarette count
    if(shared_table.paper == 1 && shared_table.tobacco == 1) {
      pthread_mutex_lock(&matchesGuy);
      shared_table.paper--;
      shared_table.tobacco--;
      matchesCigs++;
      pthread_mutex_unlock(&matchesGuy);
      ++x;
    }
    else if(shared_table.paper == 1 && shared_table.matches == 1) {
      //signal the tobacco guy
      pthread_mutex_lock(&tobaccoGuy);
      shared_table.paper--;
      shared_table.matches--;
      tobaccoCigs++;
      pthread_mutex_unlock(&tobaccoGuy);
      ++x;
    }
    else if(shared_table.matches == 1 && shared_table.tobacco == 1) {
      //signal the paper guy
      pthread_mutex_lock(&paperGuy);
      shared_table.matches--;
      shared_table.tobacco--;
      paperCigs++;
      pthread_mutex_unlock(&paperGuy);
      ++x;
    }
    pthread_mutex_unlock(&personAtTable);
  }

  pthread_exit(0);
}

// Smoker
void *smoker(void *arg)
{
  static int totalCigs = 0;
  int cigsSmoked = 0;
  int type;

  // decide smoker type
  pthread_mutex_lock(&smokerDecider);
  type = smokerType % 3;
  smokerType++;
  pthread_mutex_unlock(&smokerDecider);

  std::cout << "smoker: " << type <<"\n";

  while(cigsSmoked != 3) {
    /*
    Depending on what type of the smoker, you will take control
    of the proper semaphore and check if the pusher has a cig
    ready for you. You then smoke the cig and sleep for a few milliseconds
    */
    if(type == 0) {
      pthread_mutex_lock(&tobaccoGuy);
      if(tobaccoCigs > 0) {
        std::cout << "tobacco cig smoked" << std::endl;
        tobaccoCigs--;
        cigsSmoked++;
        pthread_mutex_unlock(&tobaccoGuy);
        usleep(200);
      }
      else {
        pthread_mutex_unlock(&tobaccoGuy);
      }
    }
    else if(type == 1) {
      pthread_mutex_lock(&paperGuy);
      if(paperCigs > 0) {
        std::cout << "paper cig smoked" << std::endl;
        paperCigs--;
        cigsSmoked++;
        pthread_mutex_unlock(&paperGuy);
        usleep(200);
      }
      else {
        pthread_mutex_unlock(&paperGuy);
      }
    }
    else if (type == 2) {
      pthread_mutex_lock(&matchesGuy);
      if(matchesCigs > 0) {
        std::cout << "matches cig smoked" << std::endl;
        matchesCigs--;
        cigsSmoked++;
        pthread_mutex_unlock(&matchesGuy);
        usleep(200);
      }
      else {
        pthread_mutex_unlock(&matchesGuy);
      }
    }
  }
  std::cout << "smoker done\n";
  pthread_exit(0);
}







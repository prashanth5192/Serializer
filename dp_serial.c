#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "dp_serial.h"

int N;

int state[100];

void Init_dp(int nphilosophers)
{
	int i = 0;
	N = nphilosophers;
	for (i=0; i < nphilosophers; i++)
		state[i] = 0; //philospher thinking
	serial = Create_Serial();  
	eating_crowd = Create_Crowd(serial);
	thinking_crowd = Create_Crowd(serial);
	waiting_queue = Create_Queue(serial);
}


void *Eat(int phil_id, void *(*model_eat)())
{
	cond_t eat_cond()
	{
		//printf("*** %d\n",phil_id);
		int left,right;
		if(phil_id > 0)
			left = (phil_id-1);
		else
			left = N-1;	   
		right = (phil_id + 1)%N;
		if(state[phil_id] == 1 && state[left]!=2 && state[right]!=2)
		{
			state[phil_id] = 2;//eat
			return 1;
		}
		else
			return 0;
	}
	void * eating()
	{
		model_eat(phil_id);
		state[phil_id] = 0;//think aft eating

	}

	Serial_Enter(serial);
	state[phil_id] = 1; // hungry
	Serial_Enqueue(serial, waiting_queue, &eat_cond, 0);
	Serial_Join_Crowd(serial, eating_crowd,  &eating);
	Serial_Exit(serial);
	//pthread_exit(NULL);
}

void *Think(int phil_id, void* (*model_think)())
{
	cond_t think_cond()
	{
		if(state[phil_id] == 0)
		{

			return 1;
		}
		else
			return 0; 
	}
	void *thinking()
	{
		model_think(phil_id);
	}

	Serial_Enter(serial);
	Serial_Enqueue(serial, waiting_queue, &think_cond, 0);	
	Serial_Join_Crowd(serial, thinking_crowd, &thinking);
	Serial_Exit(serial);
	//pthread_exit(NULL);
}

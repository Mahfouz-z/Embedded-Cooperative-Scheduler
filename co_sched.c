#include <stdlib.h>
#include "co_sched.h"
#include "co_sched_API.h"
#include <string.h>

struct Qnode * readyQ_root;    
struct Qnode * delayedQ_root;
struct Qnode running;
int rq_e, dq_e, rq_len, dq_len;
volatile int ms_counter;
volatile int coSched_tick_counter;

int Init()
{
		// Make  a dynamic array of initial size 20 for each of the readQ and the delayedQ
		rq_len = 20;
		dq_len = 20;
		readyQ_root = (struct Qnode *)calloc(rq_len, sizeof(struct Qnode));
		delayedQ_root = (struct Qnode *)calloc(dq_len, sizeof(struct Qnode));
		rq_e = 0;
		dq_e = 0;
		return 0;
}

int QueTask(void (*task_ptr), unsigned int priority)  //inserting a task into the readyQueue according to the task priority 
{
		int i;
		struct Qnode * update_readyQ_root;
		if(rq_e < rq_len)
		{
				// insert at the end of the queue 
				readyQ_root[rq_e].priority = priority;
				readyQ_root[rq_e].sleep_time = 0;
				readyQ_root[rq_e].task_ptr = task_ptr;
				
				
				//keep sorted on priotity
				for(i = rq_e; i>0; i--)
				{
						if(readyQ_root[i-1].priority > readyQ_root[i].priority)
						{
								swap(&readyQ_root[i-1], &readyQ_root[i]);
						}
				}
				// increment end
				rq_e++;
		}
		else
		{
				update_readyQ_root = (struct Qnode *)calloc(2 * rq_len, sizeof(struct Qnode));
				while(update_readyQ_root == NULL)
				{
						continue;
				}
				memcpy(update_readyQ_root, readyQ_root, sizeof(readyQ_root[0])*rq_len); 
				free(readyQ_root);
				readyQ_root = update_readyQ_root;
				rq_len *= 2;
				QueTask(task_ptr, priority);
		}
		
		return 0;
}


void swap(struct Qnode *A, struct Qnode *B){
    struct Qnode temp = *A;
    *A = *B;
    *B = temp;
}




int ReRunMe(int delay)  //storing the running task in the delayedQueue according to the task delay
{
		int j;
		struct Qnode * update_delayedQ_root;
		if(delay == 0)
		{
				QueTask(running.task_ptr, running.priority);
		}
		else
		{
				if(dq_e < dq_len)
				{
						// insert at the end of the queue 
						delayedQ_root[dq_e].priority = running.priority;
						delayedQ_root[dq_e].sleep_time = delay;
						delayedQ_root[dq_e].task_ptr = running.task_ptr;
						
						
						//keep sorted on delay
						for(j = dq_e; j>0; j--)
						{
								if(delayedQ_root[j-1].sleep_time > delayedQ_root[j].sleep_time)
								{
										swap(&delayedQ_root[j-1], &delayedQ_root[j]);
										continue;
								}
						}
						// increment dq end
						dq_e++;
				}
				else
				{
						update_delayedQ_root = (struct Qnode *)calloc(2 * dq_len, sizeof(struct Qnode));
						while(update_delayedQ_root == NULL)
						{
								continue;
						}
						memcpy(update_delayedQ_root, delayedQ_root, sizeof(delayedQ_root[0])*rq_len); 
						free(delayedQ_root);
						delayedQ_root = update_delayedQ_root;
						dq_len *= 2;
						ReRunMe(delay);
				}
		}
		return 0;
}

int Dispatch() //calling the task at the top of the readyQueue for execution 
{
		int k;
		if(rq_e > 0)
		{
				// extract the task
				running.priority = readyQ_root[0].priority;
				running.task_ptr = readyQ_root[0].task_ptr;
			
				// decrement rq end
				rq_e--;
				
				// remove the task from the queue with sort keeping
				for(k = 0; k < rq_e; k++)
				{
						swap(&readyQ_root[k], &readyQ_root[k+1]);
				}
				
				// run the task
				((void(*)())running.task_ptr)();
		}
		return 0;
}

void coop_sched_tick(void) //using systick ticks to produce scheduler ticks at a different rate 
{
		ms_counter++;
		if(ms_counter == 50)
		{
				ms_counter = 0;
				coSched_tick_counter+=1;
		}
		if(dq_e > 0)
		{
				if (delayedQ_root[0].sleep_time <= coSched_tick_counter)
				{
					decrementAll(coSched_tick_counter);
					coSched_tick_counter = 0;
				}
		}
		else	coSched_tick_counter = 0;
}

int decrementAll(int counter)  //decrementing the sleeping time for all tasks in the delayedQueue
{
		int m;
		int n;
		int swap_count;
		swap_count = 0;
		for(m=0; m <dq_e; m++)
		{
				delayedQ_root[m].sleep_time-=counter;
				if(delayedQ_root[m].sleep_time <= 0)
				{
						QueTask(delayedQ_root[m].task_ptr,delayedQ_root[m].priority);
						swap_count++;
				}
		}
		
		for(n = 0; n<swap_count; n++)
		{
				for(m=0; m <(dq_e-1); m++)
				{
						swap(&delayedQ_root[m], &delayedQ_root[m+1]);
						continue;
				}
				dq_e-=1;
		}
		return 0;
}




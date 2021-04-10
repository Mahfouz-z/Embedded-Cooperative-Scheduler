#include <stdlib.h>
#include "co_sched.h"
#include "co_sched_API.h"


volatile struct Qnode * readyQ_root;
volatile struct Qnode * delayedQ_root;
volatile struct Qnode * running;
volatile int ms_counter;
volatile int coSched_tick_counter;

int Init()
{
    int state = 0;
		ms_counter = 0;
		coSched_tick_counter = 0;
    readyQ_root = NULL; 
    delayedQ_root = NULL;
		running = NULL;
    return state;
}
int QueTask(void (*task_ptr), unsigned int priority)
{
    int state = 0;
    struct Qnode * node = (struct Qnode *) malloc(sizeof(struct Qnode));
    struct Qnode * itr = readyQ_root;

    if (node != NULL)
    {
        if (itr != NULL)
        {    while (itr->next->priority <= priority)
            {
                itr=itr->next;
                if (itr->next == NULL) break;

            }
        }
				node -> priority = priority;
				node -> task_ptr = task_ptr;
				node->sleep_time = 0;
        if (itr == NULL)
				{
						node -> next = NULL;
						readyQ_root = node;	
				}
        else 
				{
						node -> next = itr->next;
						itr->next = node;
				}					
    }
    else state = -1; //memory allocation error
    return state;
    
}

int ReRunME(int delay)
{
    int state = 0;
    if (delay == 0) 
    {
        state = QueTask(running->task_ptr, running->priority);
		}
    else 
    {
        struct Qnode * node = (struct Qnode *) malloc(sizeof(struct Qnode));
        struct Qnode * itr = delayedQ_root;

        if (node != NULL)
        {
            while (itr != NULL)
            {
							if(itr->next != NULL)
							{
									if(itr->next->sleep_time <= delay)itr = itr->next;
							}
							else break;
            }      
						
            node -> priority = running->priority;
            node -> task_ptr = running->task_ptr;
						node->sleep_time = delay;
						if (itr == NULL)
						{
								node -> next = NULL;
								delayedQ_root = node;	
						}
						else 
							if (itr->sleep_time > delay)
							{
								node -> next = itr->next;
								itr = node;
							
							}
							else
							{
									node -> next = itr->next;
									itr->next = node;
							}			
							
        }
        else state = -1; //memory allocation error
    }
    return state;
}

int Dispatch()
{
    int state = 0;
    if (readyQ_root != NULL)
    {
        running = readyQ_root;
        readyQ_root = readyQ_root->next;
        ((void(*)())running->task_ptr)();
        if(running != NULL)
				{
						free((void *)running);
						running = NULL;
				}
    } 
    else state = 1; //idle

    return state;
}

void coop_sched_tick(void)
{
		ms_counter++;
		if(ms_counter == 50)
		{
				ms_counter = 0;
				coSched_tick_counter+=1;
		}
		if(delayedQ_root != NULL)
		{
				if (delayedQ_root->sleep_time == coSched_tick_counter)
				{
					decrementAll(coSched_tick_counter);
					coSched_tick_counter = 0;
				}
		}
		else	coSched_tick_counter = 0;
}

int decrementAll(int counter)
{
		volatile struct Qnode * itr;
		itr = delayedQ_root;
		while (itr != NULL)
		{
			itr ->sleep_time = itr ->sleep_time - counter;
			if (itr ->sleep_time == 0)
			{
				QueTask(itr -> task_ptr, itr->priority);
				if (itr->next == NULL) 
				{
					free((void *)delayedQ_root);
					delayedQ_root = NULL;
				}
				else 
				{
					delayedQ_root = delayedQ_root->next;
				}
				itr = delayedQ_root;
			}
			else 
				itr = itr -> next;
		}
		return counter;
}

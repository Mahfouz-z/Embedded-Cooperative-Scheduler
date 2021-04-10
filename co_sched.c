#include <stdio.h>
#include <stdlib.h>
#include "co_sched.h"
#include "co_sched_API.h"


 struct Qnode * readyQ_root;
 struct Qnode * delayedQ_root;
 struct Qnode * running;



int Init()
{
    int state = 0;
    readyQ_root = NULL; 
		readyQ_root -> next ->priority = 15;
    delayedQ_root = NULL;
    return state;
}
int QueTask(volatile void (*task_ptr), unsigned int priority)
{
    int state = 0;
    struct Qnode * node = (struct Qnode *) malloc(sizeof(struct Qnode));
    struct Qnode * itr = readyQ_root;


    if (node != NULL)
    {
      if (itr != NULL)  
			if (itr ->priority < 8)
        {    
						while (itr->next->priority <= priority)
							{
									itr=itr->next;
									if (itr->next == NULL) 
										break;
            }
					
        }
        
        node -> priority = priority;
        node -> task_ptr = task_ptr;
        node -> next = itr->next;
        node->sleep_time = 0;
        if (itr ->priority >= 8) readyQ_root = node;
        else itr->next = node;
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
					if (itr != NULL)
            if (itr ->priority < 8)
            {    while (itr->next->sleep_time <= delay)
                {
                    itr=itr->next;
                    if (itr->next == NULL) break;

                }
            }
            
            node -> priority = running->priority;
            node -> task_ptr = running->task_ptr;
            node -> next = itr->next;
            node->sleep_time = delay;
            if (itr ->priority >= 8) delayedQ_root = node;
            else itr->next = node;
        }
        else state = -1; //memory allocation error
    }
		free(running);
    return state;
}

int Dispatch()
{
    int state = 0;
    if (readyQ_root -> priority < 8)
    {
        running = readyQ_root;
				
				if (readyQ_root->next == NULL || readyQ_root->next->priority > 8 ) 
				{
					readyQ_root = NULL;
					free(readyQ_root);
					
				}
				else 
				{
					readyQ_root = readyQ_root->next;
				}
        ((void(*)())running->task_ptr)();
        free(running);

    } 
    else state = 1; //idle

    return state;
}

int decrementAll(int counter)
{
	volatile struct Qnode * itr;
	itr = delayedQ_root;
		while (itr ->priority <8)
		{
			itr ->sleep_time = itr ->sleep_time - counter;
			counter = 0;
			if (itr ->sleep_time == 0)
			{
				QueTask(itr -> task_ptr, itr->priority);
				if (itr->next == NULL || itr->next->priority > 8 ) 
				{
					delayedQ_root = NULL;
					free(delayedQ_root);
					
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

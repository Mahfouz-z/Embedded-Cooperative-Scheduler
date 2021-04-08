#include <stdio.h>
#include "co_sched.h"
#include "co_sched_API.h"


struct Qnode * readyQ_root;
struct Qnode * delayedQ_root;
struct Qnode * running;



int Init()
{
    int state = 0;
    readyQ_root = NULL; 
    delayedQ_root = NULL;
    return state;
}
int QueTask(void (*task_ptr), unsigned int priority)
{
    int state = 0;
    struct Qnode * node = (struct Qnode *) malloc(sizeof(struct Qnode));
    struct Qnode * itr = readyQ_root->next;

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
        node -> next = itr->next;
        node->sleep_time = 0;
        if (itr == NULL) itr = node;
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
        free(running);
    }
    else 
    {
        struct Qnode * node = (struct Qnode *) malloc(sizeof(struct Qnode));
        struct Qnode * itr = delayedQ_root->next;

        if (node != NULL)
        {
            if (itr != NULL)
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
            if (itr == NULL) itr = node;
            else itr->next = node;
            free(running);
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
        free(running);

    } 
    else state = 1; //idle

    return state;
}


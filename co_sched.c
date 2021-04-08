#include <stdio.h>
#include "co_sched.h"
#include "co_sched_API.h"


struct Qnode * readyQ_root;
struct Qnode * delayedQ_root;


int Init()
{
    int error = 0;
    readyQ_root = NULL; 
    delayedQ_root = NULL;
    return error;
}
int QueTask(void (*task_ptr), unsigned int priority)
{
    int error = 0;
    
    return error;
}

int ReRunME(int delay)
{
    int error = 0;

    return error;
}

int Dispatch()
{
    int error = 0;

    return error;
}


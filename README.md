# Embedded-Cooperative-Scheduler
This is a repo for the first project for the Embedded systems course.
This scheduler was developed on NUCLEO-L432KC MCU board. 


## Usage
### Including

This scheduler uses 'co_sched.h', 'co_sched_API.h'. Both must be included in the main program.
Inside the ```SysTick_Handler``` insert the function ```coop_sched_tick(); ``` or replace the ```stm32l4xx_it.c``` file with the one in this repo.


### Scheduling Tasks
Initialise the scheduler using ```Init()```

Insert your tasks into the queue using ```QueTask()``` that accepts the function name and its priority.

Use ```Dispatch()``` to execute the ready tasks.



## Implementation Details

### Data structures

`Qnode`

A data structure that contains information about a task in the scheduler. This information is the `task_ptr`, `priority`, and `sleeping_time`.

`readyQ`

This is a dynamic array of type Qnode that contains all the tasks in the scheduler that are ready to be executed. The tasks are sorted ascendingly in order of their priorities. 

`delayedQ`

This is a dynamic array of type Qnode that contains all the tasks in the scheduler that are waiting for a certain delay of `sleeping_time` to be executed. The tasks are sorted ascendingly  in order of their sleeping_time. 

### Helper Functions

`swap`

A helper function in sorting the task queues by swapping two Qnodes inside a queue. It accepts two arguments which are pointers to the Qnodes desired to swap.

`decrementAll`

A helper function that decrements the sleeping_time value for all nodes in the delayedQ by the value `counter` which is the argument it accepts. 


## API Details


`Init`

This function initializes the system.
Creates the first node in the readyQ and the delayedQ. It allocates memory for both queues. Then it initializes the variables representing each queue length. 


`QueTask`

This function handles the readyQ. It accepts two arguments; `task_ptr` which is a pointer to a VOID task (essentially the task name) and `priority` which is a number representing the task priority. 
The insertion into the ReadyQ is done by creating a new Qnode and assigning the task and priority arguments to this node. This node is inserted into the back of the readyQ. Then the readyQ is sorted according to the priorities at each new insertion. This function also handles allocating new memory for the readyQ if the allocated memory was fully used. 


`ReRunMe`

This function handles the delayedQ. It accepts one argument; `delay` which is the time this task is supposed to wait in the delayed queue before being sent to the readyQ. The task_ptr and priority for this task are acquired from the `running` task.
If the `delay` is `0` then the task is sent directly to the readyQ using `QueTask`  
The insertion into the delayedQ is done by creating a new Qnode and assigning the task and priority arguments to this node. This node is inserted into the back of the delayedQ. Then the delayedQis sorted according to the `sleeping_time` at each new insertion. This function also handles allocating new memory for the delayedQ if the allocated memory was fully used.

`Dispatch`

This function is responsible for executing tasks from the readyQ when they are available. It also releases the Qnode for this task in readyQ.
This function is called inside the main `while(1)` loop, so it gets called after every system tick. At every calling of this function it executes the highest-priority task in the readyQ. 

`coop_sched_tick` 

This function uses the system ticks to produce scheduler ticks at a different rate. The scheduler ticks equal 50 ms. 
At every scheduler tick, this function checks the delayedQ minimum sleeping_time and it decrements the sleeping_time for all Qnodes in the delayedQ using the function `decrementAll` when the number of scheduler ticks=min sleeping_time. 

## Unit Tests

### readyQ_unit_test

This test is designed to test the following:
- correct initialization of the scheduler 
- sorting of readyQ is done correctly
- correct functionality of `QueTask`
- correct functionality of `Dispatch`

The following tasks are inserting into the readyQ using `QueTask`.

**Task1 ->** Blinking the on-board LED 4 times with delay 500 ms.

**Task2 ->** Blinking the on-board LED 4 times with delay 1000 ms.

**Task3 ->** Blinking the on-board LED 4 times with delay 100 ms.

**Task4 ->** Blinking the on-board LED 4 times with delay 300 ms.

**Task5 ->** Blinking the on-board LED 4 times with delay 50 ms.

They are inserted into the readyQ with priorities = their number (ie; Task1 has priority 1) in an order different to their priorities. 

**Insertion order:** Task5 -> Task3 -> Task1 -> Task2 -> Task4

**Execution order:** Ascending according to their priorities 

Task1 -> Task2 -> Task3 -> Task4 -> Task5 

Each task blinks the on-board LED 4 times with different delays, assuimg correct readyQ they delays will be in this order:

500 -> 1000 -> 100 -> 300 -> 50

# insert video

### readyQ_unit_test

This test is designed to test the following:
- correct initialization of the scheduler
- sorting of delayedQ is done correctly on sleeping_time
- sorting of readyQ is done correctly on priority
- correct functionality of `ReRunMe`
- correct functionality of `QueTask`
- correct functionality of `Dispatch`

The following tasks are inserting into the readyQ using `QueTask` with different priorities.

**Task1 ->** Blinking the on-board LED 4 times with delay 250 ms and ReRuns itself with delay 15 ticks. Priority = 0.

**Task2 ->** Blinking the on-board LED 4 times with delay 500 ms and ReRuns itself with delay 10 ticks. Priority = 1.

**Task3 ->** Blinking the on-board LED 4 times with delay 50 ms and ReRuns itself with delay 0 ticks. Priority = 1.

**Task4 ->** Blinking the on-board LED 4 times with delay 300 ms. Priority = 0.

**Task5 ->** Blinking the on-board LED 4 times with delay 75 ms.
Priority = 0.


**Insertion order:** Task5 -> Task4 -> Task3 -> Task1 -> Task2

**Execution order:** assuming correct readyQ priorites, the tasks should be performed in the following order at the first cycle of each:
    
Task5 -> Task4 -> Task1 -> Task2 -> Task3


Each task blinks the on-board LED 4 times with different delays, assuming correct readyQ they delays will be in this order:

75 -> 300 -> 250 -> 50 -> 500

After the First round of executions the first three tasks will keep running as they insert themselves into the delayedQ using ReRunMe. 
According to their delay times, order of first execution, and priorties, they will continue execution in the following order: 
**Task1 -> Task2 -> Task3**
with delays **250 -> 50 -> 500**
# insert video

## License
[MIT](https://choosealicense.com/licenses/mit/)
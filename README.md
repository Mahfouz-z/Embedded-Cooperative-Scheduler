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

[unit test Video](https://github.com/Mahfouz-z/Embedded-Cooperative-Scheduler/blob/main/media/ReadyQ_unit_test.mp4)



### delayedQ_unit_test

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
According to their delay times, order of first execution, and priorities, they will continue execution in the following order: 
**Task1 -> Task2 -> Task3**
with delays **250 -> 50 -> 500**

[unit test video](https://github.com/Mahfouz-z/Embedded-Cooperative-Scheduler/blob/main/media/delayedQ_unit_test.mp4)


## Application 1: Ambient temperature monitor
This application aims at measuring the temperature from a temperature sensor and compare it to a critical temperature. This critical temperature is preset to 30 degrees Celsius in the application code. The application pulls the temperature from the temperature sensor each 30 seconds, after comparing it with the critical temperature, the micro controller fires an alarm (an LED) if the measured temperature is above the critical temperature. The alarm is only turned off when the temperature is back to lower than the critical value. Furthermore, the application allows a user to edit the predefined critical temperature by connecting the micro controller’s UART 1 through an FTDI to a terminal emulator on any desktop (in our case Tera Term). We implement this application using the implemented cooperative scheduler and cubemx.

### Application 1 Demo
[Demo Video](https://drive.google.com/file/d/1IqMptFGJMs-nHC_BAYOpRjc3JRlMp5VQ/view?usp=sharing)

### Components and Connections
We use STM32L432 as our microcontroller unit, an FTDI, and DS3231. The microcontroller reads the temperature from the DS3231 chip through i2c3 peripheral. Furthermore, it is connected to the PC through the FTDI as discussed in the above section.

![alt text](https://github.com/Mahfouz-z/Embedded-Cooperative-Scheduler/blob/main/media/application_1_circuit_diagram.jpg?raw=true)

### Application API Explanation

`void init_I2C_UART_task(void)`

The above function enables the interrupt for UART 1 that operates on a baud rate of 9600 with 8N1 serial configuration. Furthermore, it performs a check on the I2C connections to make sure that the temperature sensor is connected and ready to send the data. This function . This function is enqueued in the main before the loop and runs for one time.

`void update_temprature_task(void)`

This is a periodic task that requeues itself with a delay of 300 ticks equivalent to 30 seconds. It is enqueued one time in the main function, then it keeps requeuing itself based on the periodic time mentioned above. This task calls “float read_temprature(void)” routine that returns the temperature value. This task would enqueue “void alarm_led_toggle_task(void)” task in case the temperature violated the critical value.

`float read_temprature(void)`

This function initiates an I2C communication with the temperature sensor to read the temperature value. Before reading the temperature, this function forces the sensor to update its internal register to obtain a fresh value from the register. We note that the sensor automatically updates itself on a period of 64 seconds and since we need the data each 30 seconds, we do the force conversion to get fresh data.

`void alarm_led_toggle_task(void)`

This task toggles the led and request itself till its flag is zeroed from either the update_temprature_task() or update_critical_temprature(). When the flag is zeroed, this task enqueues “void alarm_led_off_task(void)” that will make sure that the led alarm is turned off.

`void alarm_led_off_task(void)`

A task enqueued by the toggle function to turn off the LED.

`void receive_uart(void)`

This function is enqueued into the ready queue from the UART interrupt handler. It receives the UART data byte by byte into a buffer till it finds windows endl characters “\r\n”. Then it enqueues the “update_critical_temprature” that uses this buffer, convert it to float, and update the value of critical temperature.

`void update_critical_temprature(void)`

Converts the critical temperature value characters received on the UART to a float and use it to update the temperature critical value. It echoes the updated value on the serial port. Furthermore, it could enqueue the alarm task if the current temperature is more than the new critical temperature. Also, it could lower the flag checked by the alarm toggling task to terminate it in case of a critical temperature higher than the current temperature.

### Priority Choices

Tasks that run at initialization time only were given the highest priority. After that comes the communication tasks at equal priority for both the UART and the I2C tasks. Finally the least priority was given to the most periodic task the LED blinking.




## Application 2: Parking sensor
 The program reads data from the ultrasound sensor and calculates the distance of the nearest objects from this data. Then the buzzer is used to produce beeps, the frequency of the beeps reflect the distance of the object from the sensor. 

### Application 2 Demo
[Demo Video](https://drive.google.com/file/d/1vbePU137_2JWaHCBa_vhKhS03ltz6JWw/view?usp=sharing)

### Components and Connections
In this application, we use the MCU board, a buzzer, and an Ultrasound sensor.

![alt text](https://github.com/Mahfouz-z/Embedded-Cooperative-Scheduler/blob/main/media/application2_circuit_diagram.png?raw=true)
### Application API Explanation

This application consists of two tasks; one for reading from the sensor and processing its data, and another one for running the buzzer at the appropriate delays. 

`hcsr04_read`

This function handles the ultrasound sensor. it pulls the pin connected to the TRIG pin of the ultrasonic sensor HIGH for 10 us then it receives data from the ECHO pin. the ECHO pin is set to high for a time period proportional to the distance of the detected object. a flag is raised inside this function for the time the ECHO pin is set to HIGH. 
during this time, the SytTickHandler interrupt is used to count this time by calling another function `measure_time` 
distance is calculated using the time that has been incremented during the flag raise. 
This task uses ReRunMe in order to read continuously from the ultrasound sensor and update the distance

`measure_time`

This function is called inside the SytTickHandler, it checks if the flag is raised and increments the value of time if it is. 

`buzzer_task`

This function checks if the measured distance is less than a certain threshold. if it is, it toggles the buzzer pin and uses ReRunMe to run again after a time period proportional to the distance. 
if the distance is not within the threshold, the function disables the buzzer and ReRuns again with delay 10 ticks to validate the new distance.

### Priority Choices

The highest priority was given to the bigger and less frequent task which is reading from the ultrasound sensor, while lower priority was given to the buzzer task. 
 


## License
[MIT](https://choosealicense.com/licenses/mit/)

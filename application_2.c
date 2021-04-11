#include "main.h"
#include <stdio.h>
#include "co_sched_API.h"
#include "application_2.h"
#include "co_sched.h"


volatile uint32_t distance=200;
volatile uint32_t time=0;
volatile uint32_t flag=0;

void buzzer_task()
{
	int err = 0;
	if (distance < 20 )
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		ReRunMe(distance*2);
	}
	else 
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		ReRunMe(10);
	}
	
}

void measure_time()
{
	if (flag == 1 )    // while the pin is high
	{
		time++;   // measure time for which the pin is high
	}
}

void hcsr04_read ()
{
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);  // pull the TRIG pin HIGH
 HAL_Delay(2);  // wait for 2 us
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // pull the TRIG pin HIGH
 HAL_Delay(10);  // wait for 10 us
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);  // pull the TRIG pin low

 // read the time for which the pin is high

 while (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)));  // wait for the ECHO pin to go high
	while ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))) 
		flag = 1;
	flag = 0;
	
 distance = time * 3.4/2;
 time = 0;
 ReRunMe(4);
}

	
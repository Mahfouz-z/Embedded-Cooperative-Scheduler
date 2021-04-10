#include "main.h"
#include "application1.h"
#include <stdio.h>
#include "co_sched_API.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
volatile float temp = 0;
volatile int alarm_flag = 0;
volatile float critical_temp = 22;

void init_I2C_task(void)
{
		//check that device is ready to operate
		char success_message[] = {'I','2','C',' ','S','U','C','C','E','S','S','\r','\n'};
		char failure_message[] = {'I','2','C',' ','F','A','I','L','E','D','\r','\n'};
		if (HAL_I2C_IsDeviceReady(&hi2c1, 0xD0, 10, HAL_MAX_DELAY) == HAL_OK)
		{
				HAL_UART_Transmit(&huart2, (unsigned char *)success_message, sizeof(success_message), HAL_MAX_DELAY);
		}
		else
		{
				HAL_UART_Transmit(&huart2, (unsigned char *)failure_message, sizeof(failure_message), HAL_MAX_DELAY);
		}
}

void alarm_led_toggle_task(void)
{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		if(alarm_flag == 1) ReRunMe(10);
		else QueTask(alarm_led_off_task, 7);
}

void alarm_led_off_task(void)
{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void update_temprature_task(void)
{
		float temp = read_temprature();
		if(temp >= critical_temp && alarm_flag == 0) 
		{
				alarm_flag = 1;
				QueTask(alarm_led_toggle_task, 2);
		}
		else if(temp < critical_temp && alarm_flag == 1) alarm_flag = 0;
			
		ReRunMe(60); // rerun after 3 seconds
		// ReRunMe(600) rerun after 30 seconds 
}

float read_temprature(void)
{
		uint8_t temp_int_buffer[2], temp_float_buffer[2], ctrl_reg[2], status_reg[2];
		
		// force temprature calculation to read live temprature
		status_reg[0] = 0x0F; 	// Status Register Address	
		ctrl_reg[0] = 0x0E; 		// Control Register Address
		// read status register
		HAL_I2C_Mem_Read(&hi2c1, 0xD0, status_reg[0], 1, &status_reg[1], 1,10);

		// If the controller is not executing TCXO function, force it.
		if((status_reg[1]&0x4) == 0)
		{
				// read current status register to rewrite it
				HAL_I2C_Mem_Read(&hi2c1, 0xD0, ctrl_reg[0], 1, &ctrl_reg[1], 1,10);
				ctrl_reg[1] |= 0x20; // set the conv bit to force temprature conversion of temprature to digital value
				HAL_I2C_Mem_Write(&hi2c1, 0xD0, ctrl_reg[0], 1, &ctrl_reg[1], 1, 10);
		}

		//read temprature
		temp_int_buffer[0] = 0x11;    // Address of int part of temprature
		temp_float_buffer[0] = 0x12;	// Address of float part of temprature
		HAL_I2C_Mem_Read(&hi2c1, 0xD0, temp_int_buffer[0], 1, &temp_int_buffer[1], 1, 10);
		HAL_I2C_Mem_Read(&hi2c1, 0xD0, temp_float_buffer[0], 1, &temp_float_buffer[1], 1, 10);
		
//		if((temp_int_buffer[1] & 7) == 0)
//		{
//				// positive number
//				temp = temp_int_buffer[1]  + (temp_float_buffer[1]>>6) / 4.0; 
//				HAL_Delay(1);
//		}
//		else
//		{
//				// negative number
//				temp_int_buffer[1] = ~temp_int_buffer[1] + 1;
//				temp = - temp_int_buffer[1]  - (temp_float_buffer[1]>>6) / 4.0; 
//				HAL_Delay(1);
//		}
		temp = temp_int_buffer[1]  + (temp_float_buffer[1]>>6) / 4.0; 
		HAL_Delay(1);
		return temp;
	
			//send temprature integer part register address 11h to read from
//		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, temp_int_buffer, 1, 10);

//		//read data of register 11h to temp_int_buffer[1]
//		HAL_I2C_Master_Receive(&hi2c1, 0xD1, temp_int_buffer+1, 1, 10);
//	
//		//send temprature float part register address 12h to read from
//		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, temp_float_buffer, 1, 10);

//		//read data of register 12h to temp_float_buffer[1]
//		HAL_I2C_Master_Receive(&hi2c1, 0xD1, temp_float_buffer+1, 1, 10);
	
}	

